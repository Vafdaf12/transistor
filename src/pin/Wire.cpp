#include "Wire.h"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "SFML/Graphics/RenderTarget.hpp"

Wire::Wire(const Pin* p1, const Pin* p2) {
    if (p1 < p2) {
        _pins = {p1, p2};
    } else {
        _pins = {p2, p1};
    }
}
void Wire::draw(sf::RenderTarget& target, sf::RenderStates) const {
    sf::Vertex edge[2] = {sf::Vertex({0, 0}, sf::Color::White)};
    edge[0].position = _pins.first->getCenter();
    edge[1].position = _pins.second->getCenter();

    if (_pins.first->getState()) {
        edge[0].color = sf::Color::Red;
        edge[1].color = sf::Color::Red;
    }
    target.draw(edge, 2, sf::Lines);
}

bool Wire::isEndpoint(const Pin* pin) const {
    return _pins.first == pin || _pins.second == pin;
}

bool Wire::operator==(const Wire& rhs) const {
    return _pins.first == rhs._pins.first && _pins.second == rhs._pins.second;
}
