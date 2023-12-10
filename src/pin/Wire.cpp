#include "Wire.h"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/RenderTarget.hpp"

Wire::Wire(Pin* p1, Pin* p2) {
    switch (p1->getType()) {
    case Pin::Input:
        _from = p2;
        _to = p1;
        break;
    case Pin::Output:
        _from = p1;
        _to = p2;
        break;
    }
    _from->connect(&_flag);
    _to->setValue(_from->getValue());
}
Wire::~Wire() {
    if (_flag != Pin::Dead) {
        _from->disconnect(&_flag);
    }
}
void Wire::draw(sf::RenderTarget& target, sf::RenderStates) const {
    static sf::Vertex edge[2] = {sf::Vertex({0, 0}, sf::Color::White)};

    edge[0].position = _from->getWorldSpacePosition(target);
    edge[1].position = _to->getWorldSpacePosition(target);
    if (_from->getValue()) {
        edge[0].color = sf::Color::Red;
        edge[1].color = sf::Color::Red;
    } else {
        edge[0].color = sf::Color::White;
        edge[1].color = sf::Color::White;
    }

    target.draw(edge, 2, sf::Lines);
}

void Wire::update(const sf::RenderWindow&) {
    if (_flag == Pin::Dirty) {
        _to->setValue(_from->getValue());
        _flag = Pin::None;
    }
}

bool Wire::isEndpoint(const Pin* pin) const { return _from == pin || _to == pin; }
bool Wire::operator==(const Wire& rhs) const { return _from == rhs._from && _to == rhs._to; }