#include "GameWorld.h"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/System/Vector2.hpp"
#include "circuit/BinaryGate.h"
#include "circuit/NandCircuit.h"
#include "circuit/PassthroughCircuit.h"
#include "json.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using json = nlohmann::json;

GameWorld::GameWorld()
{
    _orTexture.loadFromFile("assets/sprites/gate_or.png");
    _xorTexture.loadFromFile("assets/sprites/gate_xor.png");
    _andTexture.loadFromFile("assets/sprites/gate_and.png");
}

bool GameWorld::loadFromFile(const std::string& path, const sf::Font& font) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    json data = json::parse(file);

    size_t i = 0;

    for (auto id : data["inputs"]) {
        Pin* pin = new Pin(id.get<std::string>(), Pin::Output, sf::Vector2f(-400, i * 30));
        _pins.emplace_back(pin);
        i++;
    }

    i = 0;
    for (auto id : data["outputs"]) {
        Pin* pin = new Pin(id.get<std::string>(), Pin::Input, sf::Vector2f(400, i * 30));
        _pins.emplace_back(pin);
        i++;
    }

    for (auto elem : data["elements"]) {
        std::string type = elem["type"].get<std::string>();
        std::string id = elem["id"].get<std::string>();
        float x = elem["position"]["x"].get<float>();
        float y = elem["position"]["y"].get<float>();

        if(type == "passthrough") {
            size_t pins = elem["pins"].get<size_t>();
            auto channels = elem["color"].get<std::array<uint8_t, 3>>();

            auto c = new PassthroughCircuit(id, pins, {x, y});
            c->setColor(sf::Color(channels[0], channels[1], channels[2]));
            _circuits.emplace_back(c);
        }
        else if(type == "nand_gate") {
            _circuits.emplace_back(new NandCircuit(id, font, {x, y}));
        }
        else if(type == "and_gate") {
            _circuits.emplace_back(new BinaryGate(id, _andTexture, BinaryGate::And, {x, y}));
        }
        else if(type == "xor_gate") {
            _circuits.emplace_back(new BinaryGate(id, _xorTexture, BinaryGate::Xor, {x, y}));

        }
        else if(type == "or_gate") {
            _circuits.emplace_back(new BinaryGate(id, _orTexture, BinaryGate::Or, {x, y}));
        }
        else {
            // Unsupported pin type
            return false;
        }

    }
    for(auto w : data["wires"]) {
        Pin* from = queryPin(w["from"].get<std::string>());
        if(!from) {
            std::cout << "[ERROR] Cannot find pin \"" << w["from"].get<std::string>() << "\"" << std::endl;
            return false;
        }
        Pin* to = queryPin(w["to"].get<std::string>());
        if(!to) {
            std::cout << "[ERROR] Cannot find pin \"" << w["to"].get<std::string>() << "\"" << std::endl;
            return false;
        }
        _wires.emplace_back(from, to);
        std::cout << "Added wire" << std::endl;
    }

    return true;
}

bool GameWorld::saveToFile(const std::string& path) {
    std::ofstream file(path);
    if(!file.is_open()) {
        return false;
    }

    json data;
    for(const auto& p : _pins) {
        switch (p->type) {

        case Pin::Input: {
            data["outputs"].push_back(p->getId());
            break;
        }

        case Pin::Output: {
            data["inputs"].push_back(p->getId());
            break;
        }
        }
    }
    for(const auto& circuit : _circuits) {
        json elem;
        elem["id"] = circuit->getId();

        sf::Vector2f pos = circuit->getBoundingBox().getPosition();
        elem["position"]["x"] = pos.x;
        elem["position"]["y"] = pos.y;

        if(auto c = dynamic_cast<const PassthroughCircuit*>(circuit.get())) {
            elem["type"] = "passthrough";
            sf::Color color = c->getColor();
            elem["pins"] = c->getSize();
            elem["color"] = {color.r, color.g, color.b};
        }
        else if(dynamic_cast<NandCircuit*>(circuit.get())) {
            elem["type"] = "nand_gate";
        }
        else if(auto c = dynamic_cast<const BinaryGate*>(circuit.get())) {
            if(c->getFunc() == BinaryGate::And) {
                elem["type"] = "and_gate";
            }
            else if(c->getFunc() == BinaryGate::Or) {
                elem["type"] = "or_gate";
            }
            else if(c->getFunc() == BinaryGate::Xor) {
                elem["type"] = "xor_gate";
            }

        }
        data["elements"].push_back(elem);
    }
    for(const auto& w : _wires) {
        json elem;
        auto [from, to] = w.getPins();
        elem["from"] = from->getFullPath();
        elem["to"] = to->getFullPath();
        data["wires"].push_back(elem);
    }
    file << std::setw(4) << data;
    return true;
}

void GameWorld::addPin(Pin* p) { _pins.emplace_back(p); }

void GameWorld::addCircuit(Circuit* c) { _circuits.emplace_back(c); }
void GameWorld::removeCircuit(Circuit* circuit) {
    // Remove bodies
    std::erase_if(_circuits, [&](const auto& c) { return c.get() == circuit; });

    // Remove Wires
    std::erase_if(_wires, [&](const Wire& w) { return !w.isValid(); });
}
void GameWorld::connectPins(Pin* p1, Pin* p2) { _wires.emplace_back(p1, p2); }
Pin* GameWorld::collidePin(sf::Vector2f pos, uint8_t filter) {
    if (filter & SINGLE) {
        for (const auto& p : _pins) {
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
Circuit* GameWorld::collideCircuit(sf::Vector2f pos) {
    for (const auto& c : _circuits) {
        if (c->collide(pos)) {
            return c.get();
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

void GameWorld::draw(sf::RenderTarget& target, sf::RenderStates) const {
    for (const auto& c : _circuits) {
        target.draw(*c);
    }
    for (const auto& pin : _pins) {
        target.draw(*pin);
    }
    for (const auto& wire : _wires) {

        target.draw(wire);
    }
}

Pin* GameWorld::queryPin(const std::string& path) {
    size_t i = path.find('/');
    if(i == std::string::npos) {
        for (const auto& p : _pins) {
            if(path == p->getId()) {
                return p.get();
            }
        }
        return nullptr;
    }


    std::string circuit = path.substr(0, i);
    std::string pin = path.substr(i+1);

    Circuit* c = queryCircuit(circuit);
    if(!c) {
        return nullptr;
    }
    Pin* p = c->queryPin(pin);

    return p;
}

Circuit* GameWorld::queryCircuit(const std::string& path) {
    for (const auto& c: _circuits) {
        if(c->getId() == path) {
            return c.get();
        }
    }
    return nullptr;
}
