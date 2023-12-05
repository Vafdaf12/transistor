#include "Wire.h"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/RenderTarget.hpp"

Wire::Wire(Pin* p1, Pin* p2) {
    if (p1 < p2) {
        _pins = {p1, p2};
    } else {
        _pins = {p2, p1};
    }
    if (p1->type == Pin::Output) {
        p2->setState(p1->getState());
    }
    if (p2->type == Pin::Output) {
        p1->setState(p2->getState());
    }

    p1->connect(this);
    p2->connect(this);
}
void Wire::draw(sf::RenderTarget& target, sf::RenderStates) const {
    if (!isValid()) {
        return;
    }
    sf::Vertex edge[2] = {sf::Vertex({0, 0}, sf::Color::White)};
    edge[0].position = _pins.first->getCenter();
    edge[1].position = _pins.second->getCenter();
    if (_pins.first->getState()) {
        edge[0].color = sf::Color::Red;
        edge[1].color = sf::Color::Red;
    }

    target.draw(edge, 2, sf::Lines);
}

void Wire::update(Pin* pin) {
    if (!isValid()) {
        return;
    }
    int state = pin->getState();
    if (_pins.first == pin) {
        _pins.second->setState(state);
    } else {
        _pins.first->setState(state);
    }
}

void Wire::onRemove(Pin* pin) {
    if (_pins.first == pin) {
        _pins.first = nullptr;
    }
    if (_pins.second == pin) {
        _pins.second = nullptr;
    }
}

bool Wire::isEndpoint(const Pin* pin) const { return _pins.first == pin || _pins.second == pin; }

bool Wire::isValid() const { return _pins.first && _pins.second; }

bool Wire::operator==(const Wire& rhs) const {
    return _pins.first == rhs._pins.first && _pins.second == rhs._pins.second;
}
