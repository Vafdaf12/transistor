#include "CircuitEditor.h"

#include "circuit/Circuit.h"

#include <algorithm>
#include <vector>

CircuitEditor::CircuitEditor(const sf::View& screen, const sf::View& world)
    : _worldSpace(world), _screenSpace(screen) {
        layoutPins();
    }

bool CircuitEditor::addInput(const std::string& id) {
    bool exists = std::any_of(_inputs.begin(), _inputs.end(), [&id](const Pin& p) {
        return p.getId() == id;
    });
    if (!exists) {
        _inputs.emplace_back(id, Pin::Output);
        _inputs.back().setView(&_screenSpace);
        _inputs.back().setEditable(true);
        layoutPins();
    }
    return !exists;
}
bool CircuitEditor::removeInput(Pin* pin) {
    bool removed = std::erase_if(_inputs, [&pin](const Pin& p) { return &p == pin; }) > 0;
    if (removed) {
        updateWires();
        layoutPins();
    }
    return removed;
}

bool CircuitEditor::addOutput(const std::string& id) {
    bool exists = std::any_of(_inputs.begin(), _inputs.end(), [&id](const Pin& p) {
        return p.getId() == id;
    });
    if (!exists) {
        _outputs.emplace_back(id, Pin::Input);
        _outputs.back().setView(&_screenSpace);
        layoutPins();
    }
    return !exists;
}
bool CircuitEditor::removeOutput(Pin* pin) {
    bool removed = std::erase_if(_outputs, [&pin](const Pin& p) { return &p == pin; }) > 0;
    if (removed) {
        updateWires();
        layoutPins();
    }
    return removed;
}

bool CircuitEditor::addCircuit(Circuit* c) {
    bool exists = std::any_of(_circuits.begin(), _circuits.end(), [&c](const auto& p) {
        return p->getId() == c->getId();
    });
    if (!exists) {
        _circuits.emplace_back(c);
        c->setView(_worldSpace);
    }
    return !exists;
}
bool CircuitEditor::removeCircuit(Circuit* c) {
    bool removed = std::erase_if(_circuits, [&c](const auto& p) { return c == p.get(); }) > 0;
    if (removed) {
        updateWires();
    }
    return removed;
}

bool CircuitEditor::addWire(Pin* from, Pin* to) {
    Wire wire(from, to);
    bool exists =
        std::any_of(_wires.begin(), _wires.end(), [&wire](const Wire& w) { return wire == w; });
    if (!exists) {
        _wires.emplace_back(std::move(wire));
    }
    return !exists;
}
bool CircuitEditor::removeWire(Pin* from, Pin* to) {
    if (!to) {
        return std::erase_if(_wires, [from](const Wire& w) { return w.isEndpoint(from); }) > 0;
    }
    Wire wire(from, to);
    return std::erase_if(_wires, [&wire](const Wire& w) { return w == wire; }) > 0;
}

Pin* CircuitEditor::collidePin(const sf::RenderWindow& w, sf::Vector2i pixel, bool worldOnly) {
    sf::Vector2f pos = w.mapPixelToCoords(pixel, _worldSpace);
    for (auto& c : _circuits) {
        if (Pin* pin = c->collidePin(pos)) {
            return pin;
        }
    }

    if (worldOnly) {
        return nullptr;
    }

    pos = w.mapPixelToCoords(pixel, _screenSpace);
    for (auto& p : _inputs) {
        if (p.collide(pos)) {
            return &p;
        }
    }
    for (auto& p : _outputs) {
        if (p.collide(pos)) {
            return &p;
        }
    }
    return nullptr;
}

void CircuitEditor::updateWires() {
    std::erase_if(_wires, [](const Wire& w) { return !w.isValid(); });
}
void CircuitEditor::layoutPins() {
    static constexpr float pinPadding = 30;
    static constexpr float pinMargin = 20;
    static constexpr float pinStep = 2 * Pin::RADIUS + pinPadding;

    float inputHeight = _inputs.size() * pinStep - pinPadding;
    float outputHeight = _outputs.size() * pinStep - pinPadding;

    sf::Vector2f size = _screenSpace.getSize();
    sf::Vector2f inputBase = {Pin::RADIUS + pinMargin, (size.y - inputHeight) / 2};
    sf::Vector2f outputBase = {size.x - Pin::RADIUS - pinMargin, (size.y - outputHeight) / 2.f};

    for (size_t i = 0; i < _inputs.size(); i++) {
        _inputs[i].setCenter(inputBase + sf::Vector2f(0, i * pinStep));
    }
    for (size_t i = 0; i < _outputs.size(); i++) {
        _outputs[i].setCenter(outputBase + sf::Vector2f(0, i * pinStep));
    }
}
void CircuitEditor::onEvent(const sf::RenderWindow& w, const sf::Event& e) {
    if (e.type == sf::Event::Resized) {
        _screenSpace.setSize(e.size.width, e.size.height);
        _screenSpace.setCenter(e.size.width / 2.f, e.size.height / 2.f);

        float zoom = _worldSpace.getSize().x / e.size.width;
        _worldSpace.setSize(e.size.width * zoom, e.size.height * zoom);
        layoutPins();
    }

    for (auto& p : _wires) {
        p.onEvent(w, e);
    }
    for (auto& p : _inputs) {
        p.onEvent(w, e);
    }
    for (auto& p : _outputs) {
        p.onEvent(w, e);
    }
    for (auto& c : _circuits) {
        c->onEvent(w, e);
    }
}
void CircuitEditor::update(const sf::RenderWindow& w, float dt) {
    for (auto& p : _wires) {
        p.update(w, dt);
    }
    for (auto& p : _inputs) {
        p.update(w, dt);
    }
    for (auto& p : _outputs) {
        p.update(w, dt);
    }
    for (auto& c : _circuits) {
        c->update(w, dt);
    }
}
void CircuitEditor::draw(sf::RenderWindow& w) const {
    w.setView(_worldSpace);
    for (const auto& p : _wires) {
        p.draw(w);
    }
    for (const auto& c : _circuits) {
        c->draw(w);
    }

    w.setView(_screenSpace);
    for (const auto& p : _inputs) {
        p.draw(w);
    }
    for (const auto& p : _outputs) {
        p.draw(w);
    }
}
Pin* CircuitEditor::queryPin(const std::string& path) {
    size_t i = path.find('/');
    if (i == std::string::npos) {
        for (auto& p : _inputs) {
            if (path == p.getId()) {
                return &p;
            }
        }
        for (auto& p : _outputs) {
            if (path == p.getId()) {
                return &p;
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

Circuit* CircuitEditor::queryCircuit(const std::string& path) {
    for (const auto& c : _circuits) {
        if (c->getId() == path) {
            return c.get();
        }
    }
    return nullptr;
}
std::string CircuitEditor::getCircuitId(const std::string& id) const {
    int count = 0;
    for(const auto& c : _circuits) {
        std::string cid = c->getId().substr(0, id.size());
        count++;
    }
    if(count == 0) {
        return id;
    }
    return id + std::to_string(count);

}
