#include "Wire.h"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/RenderTarget.hpp"

Wire::Wire(Pin* p1, Pin* p2) {
    if (p1 < p2) {
        _pins = {p1, p2};
    } else {
        _pins = {p2, p1};
    }
    if (p1->getType() == Pin::Output) {
        p2->setValue(p1->getValue());
    }
    if (p2->getType() == Pin::Output) {
        p1->setValue(p2->getValue());
    }

    p1->connect(_flags);
    p2->connect(_flags + 1);
}
Wire::~Wire() {
    if(_flags[0] != Pin::Dead) {
        _pins.first->disconnect(_flags);
    }
    if(_flags[1] != Pin::Dead) {
        _pins.second->disconnect(_flags + 1);
    }
}
void Wire::draw(sf::RenderTarget& target, sf::RenderStates) const {
    if (!isValid()) {
        return;
    }
    sf::Vertex edge[2] = {sf::Vertex({0, 0}, sf::Color::White)};

    edge[0].position = _pins.first->getWorldSpacePosition(target);
    edge[1].position = _pins.second->getWorldSpacePosition(target);
    if (_pins.first->getValue()) {
        edge[0].color = sf::Color::Red;
        edge[1].color = sf::Color::Red;
    }

    target.draw(edge, 2, sf::Lines);
}

void Wire::update(const sf::RenderWindow&) {
    if (_flags[0] == Pin::Dirty) {
        _pins.second->setValue(_pins.first->getValue());
        _flags[0] = Pin::None;
    } else if (_flags[1] == Pin::Dirty) {
        _pins.first->setValue(_pins.second->getValue());
        _flags[1] = Pin::None;
    }
}

bool Wire::isEndpoint(const Pin* pin) const { return _pins.first == pin || _pins.second == pin; }

bool Wire::isValid() const { return _flags[0] != Pin::Dead && _flags[1] != Pin::Dead; }

bool Wire::operator==(const Wire& rhs) const {
    return _pins.first == rhs._pins.first && _pins.second == rhs._pins.second;
}