#include "Wire.h"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "util/util.h"
#include <iostream>

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
    _from->connect(&_flags[0]);
    _to->setValue(_from->getValue());
    _to->connect(&_flags[1]);
}
Wire::~Wire() {
    if (_flags[0] != Pin::Dead) {
        _from->disconnect(&_flags[0]);
    }
    if (_flags[1] != Pin::Dead) {
        _to->disconnect(&_flags[1]);
        _to->setValue(false);
    }
}
void Wire::draw(sf::RenderWindow& target) const {
    static sf::Vertex edge[2] = {sf::Vertex({0, 0}, sf::Color::White)};

    edge[0].position = util::projectToWorldSpace(target, _from->getScreenSpacePosition(target));
    edge[1].position = util::projectToWorldSpace(target, _to->getScreenSpacePosition(target));


    if (_from->getValue()) {
        edge[0].color = sf::Color(Pin::COLOR_ACTIVE);
        edge[1].color = sf::Color(Pin::COLOR_ACTIVE);
    } else {
        edge[0].color = sf::Color(Pin::COLOR_INACTIVE);
        edge[1].color = sf::Color(Pin::COLOR_INACTIVE);
    }

    target.draw(edge, 2, sf::Lines);
}

void Wire::update(const sf::RenderWindow&, float) {
    if (_flags[0] == Pin::Dirty) {
        _to->setValue(_from->getValue());
        _flags[0] = Pin::None;
        _flags[1] = Pin::None;
    }
}

bool Wire::isEndpoint(const Pin* pin) const { return _from == pin || _to == pin; }
bool Wire::operator==(const Wire& rhs) const { return _from == rhs._from && _to == rhs._to; }
