#include "Pin.h"

#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

#include "SFML/Window/Event.hpp"
#include "circuit/Circuit.h"

Pin::Pin(const std::string& id, PinType type, sf::Vector2f pos, int state)
    : _type(type), _value(state), _id(id) {
    _graphic.setPosition(pos - sf::Vector2f(RADIUS, RADIUS));
    _graphic.setFillColor(sf::Color::Black);
    _graphic.setOutlineThickness(1);
    _graphic.setRadius(10);

    switch (type) {
    case Input: _graphic.setOutlineColor(sf::Color(0x888888ff)); break;
    case Output: _graphic.setOutlineColor(sf::Color::White); break;
    }
    if (_value) {
        _graphic.setFillColor(sf::Color::Red);
    } else {
        _graphic.setFillColor(sf::Color::Black);
    }
}
void Pin::onEvent(const sf::RenderWindow& w, const sf::Event& e) {
    if (!_editable) {
        return;
    }
    if (e.type != sf::Event::MouseButtonReleased) {
        return;
    }
    if(e.mouseButton.button != sf::Mouse::Left) {
        return;
    }
    sf::Vector2i pos = {e.mouseButton.x, e.mouseButton.y};
    sf::Vector2f worldPos = w.mapPixelToCoords(pos);

    if(_graphic.getGlobalBounds().contains(worldPos)) {
        setValue(!getValue());
    }
}
void Pin::setValue(bool value) {
    if (_value == value) {
        return;
    }
    _value = value;
    if (_value) {
        _graphic.setFillColor(sf::Color::Red);
    } else {
        _graphic.setFillColor(sf::Color::Black);
    }
    changed();
}

sf::Vector2f Pin::getCenter() const {
    float r = _graphic.getRadius();
    return _graphic.getPosition() + sf::Vector2f(r, r);
}
void Pin::setCenter(sf::Vector2f pos) {
    float r = _graphic.getRadius();
    _graphic.setPosition(pos - sf::Vector2f(r, r));
}
bool Pin::canConnect(const Pin& other) const {
    switch (_type) {
    case Input: return other._type == Output;
    case Output: return other._type == Input;
    }
    return false;
}
bool Pin::collide(sf::Vector2f pos) const { return _graphic.getGlobalBounds().contains(pos); }
sf::Transformable& Pin::getTransform() { return _graphic; }
void Pin::draw(sf::RenderWindow& window) const { window.draw(_graphic); }

bool Pin::connect(bool* obs) { return _dirtyFlags.insert(obs).second; }
bool Pin::disconnect(bool* obs) { return _dirtyFlags.erase(obs) > 0; }

std::string Pin::getFullPath() const {
    if (_parent) {
        return _parent->getId() + "/" + _id;
    }
    return _id;
}
void Pin::changed() {
    for (bool* flag : _dirtyFlags) {
        *flag = !*flag;
    }
}