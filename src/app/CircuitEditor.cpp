#include "CircuitEditor.h"
#include "circuit/Circuit.h"

#include "tools/CircuitDragger.h"
#include "tools/PinConnector.h"
#include "tools/SelectionTool.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

CircuitEditor::CircuitEditor(const sf::View& screen) : _screenSpace(screen), m_camera(screen) {
    layoutPins();
    m_camera.setTarget({0, 0});
    _tools.emplace_back(new PinConnector(*this));
    _tools.emplace_back(new CircuitDragger(*this, _board));
    _tools.emplace_back(new SelectionTool(*this, _board));
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
        removeConnectedWires(pin);
        layoutPins();
    }
    return removed;
}

bool CircuitEditor::addOutput(const std::string& id) {
    bool exists = std::any_of(_outputs.begin(), _outputs.end(), [&id](const Pin& p) {
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
        removeConnectedWires(pin);
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
        for (Pin* p : c->getAllPins()) {
            p->setView(&m_camera.getView());
        }
    }
    return !exists;
}
bool CircuitEditor::removeCircuit(const Circuit* c) {
    std::vector<Pin*> circuitPins = const_cast<Circuit*>(c)->getAllPins();

    bool removed = std::erase_if(_circuits, [&c](const auto& p) { return c == p.get(); }) > 0;
    if (removed) {
        using namespace std::placeholders;
        for (Pin* p : circuitPins) {
            removeConnectedWires(p);
        }
    }
    return removed;
}

bool CircuitEditor::addWire(Pin* from, Pin* to) {
    if (from->getType() == Pin::Input) {
        std::swap(from, to);
    }

    auto it = std::find_if(_wires.begin(), _wires.end(), [&](const Wire& w) {
        if (w.getTo() == to) {
            return true;
        }
        return w == Wire(from, to);
    });
    if (it != _wires.end()) {
        return false;
    }
    _wires.emplace_back(from, to);
    return true;
}
bool CircuitEditor::removeWire(Pin* from, Pin* to) {
    if (!to) {
        return std::erase_if(_wires, [from](const Wire& w) { return w.isEndpoint(from); }) > 0;
    }
    return std::erase_if(_wires, [from, to](const Wire& w) {
               return w.getFrom() == from && w.getTo() == to;
           }) > 0;
}
Wire* CircuitEditor::getWire(Pin* from, Pin* to) {
    auto it = _wires.end();
    if (!to) {
        it = std::find_if(_wires.begin(), _wires.end(), [from](const auto& w) {
            return w.isEndpoint(from);
        });
    } else {
        Wire wire(from, to);
        it = std::find_if(_wires.begin(), _wires.end(), [&wire](const Wire& w) {
            return w == wire;
        });
    }
    if (it != _wires.end()) {
        return &(*it);
    } else {
        return nullptr;
    }
}

Circuit* CircuitEditor::collideCircuit(sf::Vector2f pos) {
    for (const auto& c : _circuits) {
        if (c->collide(pos)) {
            return c.get();
        }
    }
    return nullptr;
}
std::vector<Circuit*> CircuitEditor::collideCircuit(sf::FloatRect rect) {
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

Pin* CircuitEditor::collidePin(const sf::RenderWindow& w, sf::Vector2i pixel, bool worldOnly) {
    for (auto& c : _circuits) {
        for (Pin* p : c->getAllPins()) {
            if (p->collide(w, pixel)) {
                return p;
            }
        }
    }

    if (worldOnly) {
        return nullptr;
    }

    sf::Vector2f pos = w.mapPixelToCoords(pixel, _screenSpace);
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

void CircuitEditor::removeConnectedWires(const Pin* pin) {
    int count = std::erase_if(_wires, [pin](const Wire& w) { return w.isEndpoint(pin); });
    if (count > 0) {
        std::cout << "[INFO/CircuitEditor] Removed " << count << " wires" << std::endl;
    }
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

    size_t i = 0;
    for (Pin& p : _inputs) {
        p.setPosition(inputBase + sf::Vector2f(0, (i++) * pinStep));
    }
    i = 0;
    for (Pin& p : _outputs) {
        p.setPosition(outputBase + sf::Vector2f(0, (i++) * pinStep));
    }
}
void CircuitEditor::onEvent(const sf::RenderWindow& w, const sf::Event& e) {
    m_camera.onEvent(w, e);
    if (e.type == sf::Event::Resized) {
        _screenSpace.setSize(e.size.width, e.size.height);
        _screenSpace.setCenter(e.size.width / 2.f, e.size.height / 2.f);

        layoutPins();
    }
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::Up) {
            if (e.key.alt) {
                std::string id = getInputId("in");
                addInput(id);
                std::cout << "Add input: " << id << std::endl;
            } else {
                std::string id = getOutputId("out");
                addOutput(id);
                std::cout << "Add output: " << id << std::endl;
            }
        }
        if (e.key.code == sf::Keyboard::Down) {
            if (e.key.alt && _inputs.size() > 0) {
                removeInput(&_inputs.back());
                std::cout << "Remove input" << std::endl;
            } else if (_outputs.size() > 0) {
                removeOutput(&_outputs.back());
                std::cout << "Remove output" << std::endl;
            }
        }
        if (e.key.code == sf::Keyboard::Delete) {
            for (Circuit* c : _board.getSelection()) {
                removeCircuit(c);
            }
            _board.clearSelection();
        }
        if (e.key.control && e.key.code == sf::Keyboard::C) {
            _board.setClipboard(_board.getSelection());
            std::cout << "Copy " << _board.getClipboard().size() << std::endl;
        }
        if (e.key.control && e.key.code == sf::Keyboard::V) {
            std::vector<Circuit*> copied = _board.getClipboard();
            std::transform(copied.begin(), copied.end(), copied.begin(), [this](Circuit* c) {
                Circuit* clone = c->clone(getCircuitId(c->getId()));
                clone->setPosition(clone->getPosition() + sf::Vector2f(10, 10));
                addCircuit(clone);
                return clone;
            });
            _board.setSelection(copied);
            _board.setClipboard(copied);
            std::cout << "Paste " << _board.getClipboard().size() << std::endl;
        }
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
    for (auto& tool : _tools) {
        tool->onEvent(w, e);
        if (tool->isActive()) {
            break;
        }
    }
}
void CircuitEditor::update(const sf::RenderWindow& w, float dt) {
    m_camera.update(w, dt);
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
    for (auto& tool : _tools) {
        tool->update(w, dt);
        if (tool->isActive()) {
            break;
        }
    }

    sf::Vector2f topLeft = m_camera.getView().getCenter() - m_camera.getView().getSize() / 2.f;
    sf::Vector2f bottomRight = m_camera.getView().getCenter() + m_camera.getView().getSize() / 2.f;
    float dist = w.mapCoordsToPixel({100, 0}, m_camera.getView()).x -
                 w.mapCoordsToPixel({0, 0}, m_camera.getView()).x;
    const int zoom = std::min(m_camera.getView().getSize().x / w.getSize().x / 4, 3.f);
    const float gridSize = 10 * 1024 / std::pow(2, ceil(log2(dist)));
    const sf::Color dimColor = sf::Color(0xffffff18);

    _grid.clear();
    _grid.setPrimitiveType(sf::Lines);
    int range = ceil((bottomRight.x - topLeft.x) / gridSize);
    float start = floor(topLeft.x / gridSize) * gridSize;
    for (int i = 0; i <= range; i++) {
        sf::Vertex top({start + gridSize * i, topLeft.y});
        sf::Vertex bot({start + gridSize * i, bottomRight.y});

        if (top.position.x == 0) {
            top.color = sf::Color::Green;
            bot.color = sf::Color::Green;
        } else {
            top.color = dimColor;
            bot.color = dimColor;
        }
        _grid.append(top);
        _grid.append(bot);
    }

    range = ceil((bottomRight.y - topLeft.y) / gridSize);
    start = floor(topLeft.y / gridSize) * gridSize;
    for (int i = 0; i <= range; i++) {
        sf::Vertex top({topLeft.x, start + gridSize * i});
        sf::Vertex bot({bottomRight.x, start + gridSize * i});

        if (top.position.y == 0) {
            top.color = sf::Color::Red;
            bot.color = sf::Color::Red;
        } else {
            top.color = dimColor;
            bot.color = dimColor;
        }
        _grid.append(top);
        _grid.append(bot);
    }
}
void CircuitEditor::draw(sf::RenderWindow& w) const {
    w.setView(m_camera.getView());
    w.draw(_grid);
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

    w.setView(m_camera.getView());
    for (auto& tool : _tools) {
        tool->draw(w);
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
    std::vector<Pin*> pins = c->getAllPins();
    auto it =
        std::find_if(pins.begin(), pins.end(), [&pin](const Pin* p) { return p->getId() == pin; });
    return it == pins.end() ? nullptr : *it;
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
    for (const auto& c : _circuits) {
        std::string cid = c->getId().substr(0, id.size());
        count++;
    }
    if (count == 0) {
        return id;
    }
    return id + std::to_string(count);
}
std::string CircuitEditor::getInputId(const std::string& id) const {
    int count = 0;
    for (const auto& c : _inputs) {
        std::string cid = c.getId().substr(0, id.size());
        count++;
    }
    if (count == 0) {
        return id;
    }
    return id + std::to_string(count);
}
std::string CircuitEditor::getOutputId(const std::string& id) const {
    int count = 0;
    for (const auto& c : _outputs) {
        std::string cid = c.getId().substr(0, id.size());
        count++;
    }
    if (count == 0) {
        return id;
    }
    return id + std::to_string(count);
}
void CircuitEditor::toJson(nlohmann::json& j) const {
    std::transform(
        _inputs.begin(),
        _inputs.end(),
        std::back_inserter(j["inputs"]),
        [](const Pin& pin) { return pin.getId(); }
    );
    std::transform(
        _outputs.begin(),
        _outputs.end(),
        std::back_inserter(j["outputs"]),
        [](const Pin& pin) { return pin.getId(); }
    );
    std::transform(
        _circuits.begin(),
        _circuits.end(),
        std::back_inserter(j["elements"]),
        [](const auto& c) {
            nlohmann::json j;
            c->toJson(j);
            return j;
        }
    );

    std::transform(_wires.begin(), _wires.end(), std::back_inserter(j["wires"]), [](const auto& w) {
        return nlohmann::json{
            {"from", w.getFrom()->getFullPath()},
            {"to", w.getTo()->getFullPath()},
        };
    });
}
