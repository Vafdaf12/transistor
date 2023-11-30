#include "Pin.h"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/System/Vector2.hpp"

Pin::Pin(PinType type, sf::Vector2f pos, int state) : _type(type), _state(state) {
    _shape.setPosition(pos - sf::Vector2f(RADIUS, RADIUS));
    _shape.setFillColor(sf::Color::Black);
    _shape.setOutlineThickness(1);
    _shape.setRadius(10);

    switch (type) {
    case Input: _shape.setOutlineColor(sf::Color(0x888888ff)); break;
    case Output: _shape.setOutlineColor(sf::Color::White); break;
    }
}
void Pin::setState(int s) {
    if (_state == s) {
        return;
    }
    _state = s;
    if (_state) {
        _shape.setFillColor(sf::Color::Red);
    } else {
        _shape.setFillColor(sf::Color::Black);
    }
}
int Pin::getState() const { return _state; }

sf::Vector2f Pin::getCenter() const {
    float r = _shape.getRadius();
    return _shape.getPosition() + sf::Vector2f(r, r);
}
void Pin::setCenter(sf::Vector2f pos) {
    float r = _shape.getRadius();
    _shape.setPosition(pos - sf::Vector2f(r, r));
}
bool Pin::canConnect(const Pin& other) const {
    switch (_type) {
    case Input: return other._type == Output;
    case Output: return other._type == Input;
    }
    return false;
}
bool Pin::collide(sf::Vector2f pos) const {
    return _shape.getGlobalBounds().contains(pos);
}
sf::Transformable& Pin::getTransform() {
    return _shape;
}
void Pin::draw(sf::RenderTarget& target, sf::RenderStates) const {
    target.draw(_shape);
}
