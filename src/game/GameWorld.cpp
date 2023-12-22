#include "GameWorld.h"
#include "SFML/System/Vector2.hpp"
#include "circuit/BinaryGate.h"
#include "circuit/NotGate.h"
#include "json.hpp"
#include <fstream>
#include <iomanip>
#include <vector>

using json = nlohmann::json;

bool GameWorld::loadFromFile(
    const std::string& path, const CircuitRegistry<std::string>& registry
) {
    return false;
}

bool GameWorld::saveToFile(const std::string& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    json data;
    for (const auto& p : _inputs) {
        data["inputs"].push_back(p->getId());
    }
    for (const auto& p : _outputs) {
        data["outputs"].push_back(p->getId());
    }
    for (const auto& circuit : _circuits) {
        json elem;
        elem["id"] = circuit->getId();

        sf::Vector2f pos = circuit->getBoundingBox().getPosition();
        elem["position"]["x"] = pos.x;
        elem["position"]["y"] = pos.y;

        if (dynamic_cast<void*>(circuit.get())) {
            assert(false && "Not Implemented");
            elem["type"] = "nand_gate";
        } else if (dynamic_cast<NotGate*>(circuit.get())) {
            elem["type"] = "not_gate";
        } else if (auto c = dynamic_cast<const BinaryGate*>(circuit.get())) {
            if (c->getFunc() == BinaryGate::And) {
                elem["type"] = "and_gate";
            } else if (c->getFunc() == BinaryGate::Or) {
                elem["type"] = "or_gate";
            } else if (c->getFunc() == BinaryGate::Xor) {
                elem["type"] = "xor_gate";
            }
        }
        data["elements"].push_back(elem);
    }
    for (const auto& w : _wires) {
        json elem;
        elem["from"] = w.getFrom()->getFullPath();
        elem["to"] = w.getTo()->getFullPath();
        data["wires"].push_back(elem);
    }
    file << std::setw(4) << data;
    return true;
}

void GameWorld::addPin(Pin* p) {
    switch (p->getType()) {

    case Pin::Input: _outputs.emplace_back(p); break;
    case Pin::Output: _inputs.emplace_back(p); break;
    }
}

void GameWorld::addCircuit(Circuit* c) { _circuits.emplace_back(c); }
void GameWorld::removeCircuit(Circuit* circuit) {
    // Remove bodies
    std::erase_if(_circuits, [&](const auto& c) { return c.get() == circuit; });

    // Remove Wires
    std::erase_if(_wires, [&](const Wire& w) { return !w.isValid(); });
}

void GameWorld::removeWire(const Wire* wire) {
    std::erase_if(_wires, [&](const Wire& w) { return &w == wire; });
}

const Wire* GameWorld::getConnectedWire(const Pin* pin) {
    if (!pin) {
        return nullptr;
    }
    for (const auto& wire : _wires) {
        if (wire.isEndpoint(pin)) {
            return &wire;
        }
    }
    return nullptr;
}

void GameWorld::connectPins(Pin* p1, Pin* p2) { _wires.emplace_back(p1, p2); }
Pin* GameWorld::collidePin(sf::Vector2f pos, uint8_t filter) {
    if (filter & SINGLE) {
        for (const auto& p : _inputs) {
            if (p->collide(pos)) {
                return p.get();
            }
        }
        for (const auto& p : _outputs) {
            if (p->collide(pos)) {
                return p.get();
            }
        }
    }
    if (filter & CIRCUIT) {
        for (const auto& c : _circuits) {
            if (Pin* p = c->collidePin(pos)) {
                return p;
            }
        }
    }
    return nullptr;
}

Pin* GameWorld::collidePin(const sf::RenderWindow& window, sf::Vector2i pos, uint8_t filter) {
    if (filter & SINGLE) {
        for (const auto& p : _inputs) {
            if (p->collide(window, pos)) {
                return p.get();
            }
        }
        for (const auto& p : _outputs) {
            if (p->collide(window, pos)) {
                return p.get();
            }
        }
    }
    sf::Vector2f worldSpace = window.mapPixelToCoords(pos);
    if (filter & CIRCUIT) {
        for (const auto& c : _circuits) {
            if (Pin* p = c->collidePin(worldSpace)) {
                return p;
            }
        }
    }
    return nullptr;
}

bool GameWorld::isPinConnected(const Pin* pin) const {
    for (const auto& wire : _wires) {
        if (wire.isEndpoint(pin))
            return true;
    }
    return false;
}
Circuit* GameWorld::collideCircuit(sf::Vector2f pos) {
    for (const auto& c : _circuits) {
        if (!c->collide(pos)) {
            continue;
        }
        if (!c->collidePin(pos)) {
            return c.get();
        } else {
            return nullptr;
        }
    }
    return nullptr;
}
std::vector<Circuit*> GameWorld::collideCircuit(sf::FloatRect rect) {
    std::vector<Circuit*> contained;
    for (const auto& c : _circuits) {
        sf::Vector2f tl = c->getBoundingBox().getPosition();
        sf::Vector2f br = tl + c->getBoundingBox().getSize();
        if (rect.contains(tl) && rect.contains(br)) {
            contained.push_back(c.get());
        }
    }
    return contained;
}

void GameWorld::onEvent(const sf::RenderWindow& w, const sf::Event& e) {
    for (auto& p : _inputs) {
        p->onEvent(w, e);
    }
    for (auto& p : _outputs) {
        p->onEvent(w, e);
    }
}

void GameWorld::draw(sf::RenderWindow& window) const {
    for (const auto& c : _circuits) {
        c->draw(window);
    }
    for (const auto& wire : _wires) {
        wire.draw(window);
    }
    sf::View view = window.getView();
    window.setView(_guiView);
    for (const auto& pin : _outputs) {
        pin->draw(window);
    }
    for (const auto& pin : _inputs) {
        pin->draw(window);
    }
    window.setView(view);
}

void GameWorld::update(const sf::RenderWindow& w, float dt) {
    for (auto& wire : _wires) {
        wire.update(w, dt);
    }
    for (auto& c : _circuits) {
        c->update(w, dt);
    }
    for (auto& p : _inputs) {
        p->update(w, dt);
    }
    for (auto& p : _outputs) {
        p->update(w, dt);
    }
    sf::Vector2u size = w.getSize();
    _guiView.setSize(size.x, size.y);
    _guiView.setCenter(size.x / 2.f, size.y / 2.f);

    layoutPins(w);
}

Pin* GameWorld::queryPin(const std::string& path) {
    size_t i = path.find('/');
    if (i == std::string::npos) {
        for (const auto& p : _inputs) {
            if (path == p->getId()) {
                return p.get();
            }
        }
        for (const auto& p : _outputs) {
            if (path == p->getId()) {
                return p.get();
            }
        }
        return nullptr;
    }

    std::string circuit = path.substr(0, i);
    std::string pin = path.substr(i + 1);

    Circuit* c = queryCircuit(circuit);
    if (!c) {
        return nullptr;
    }
    Pin* p = c->queryPin(pin);

    return p;
}

Circuit* GameWorld::queryCircuit(const std::string& path) {
    for (const auto& c : _circuits) {
        if (c->getId() == path) {
            return c.get();
        }
    }
    return nullptr;
}

void GameWorld::layoutPins(const sf::RenderWindow& window) {
    static constexpr float pinPadding = 30;
    static constexpr float pinMargin = 20;
    static constexpr float pinStep = 2 * Pin::RADIUS + pinPadding;

    float inputHeight = _inputs.size() * pinStep - pinPadding;
    float outputHeight = _outputs.size() * pinStep - pinPadding;

    sf::Vector2u size = window.getSize();
    sf::Vector2f inputBase = window.mapPixelToCoords(
        {int(Pin::RADIUS + pinMargin), int((size.y - inputHeight) / 2)}, _guiView
    );
    sf::Vector2f outputBase = window.mapPixelToCoords(
        {int(size.x - Pin::RADIUS - pinMargin), int((size.y - outputHeight) / 2.f)}, _guiView
    );

    for (size_t i = 0; i < _inputs.size(); i++) {
        _inputs[i]->setCenter(inputBase + sf::Vector2f(0, i * pinStep));
    }
    for (size_t i = 0; i < _outputs.size(); i++) {
        _outputs[i]->setCenter(outputBase + sf::Vector2f(0, i * pinStep));
    }
}

std::string GameWorld::assignCircuitId(const std::string& id) const {
    int count = 0;
    for (const auto& c : _circuits) {
        std::string cid = c->getId().substr(0, id.size());
        count++;
    }
    if (count == 0) {
        return id;
    }
    return id + std::to_string(count);
}
