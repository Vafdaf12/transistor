#include "Pin.h"

#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

#include "SFML/Window/Event.hpp"
#include "circuit/Circuit.h"
#include "util/util.h"

Pin::Pin(const std::string& id, PinType type, sf::Vector2f pos, int state)
    : Entity(id), _type(type), _value(state) {
    _graphic.setPosition(pos - sf::Vector2f(RADIUS, RADIUS));
    _graphic.setRadius(10);

    if (_value) {
        _graphic.setFillColor(sf::Color(COLOR_ACTIVE));
    } else {
        _graphic.setFillColor(sf::Color(COLOR_INACTIVE));
    }
}
void Pin::onEvent(const sf::RenderWindow& w, const sf::Event& e) {
    if (!_editable) {
        return;
    }
    if (e.type != sf::Event::MouseButtonReleased) {
        return;
    }
    if (e.mouseButton.button != sf::Mouse::Left) {
        return;
    }
    sf::Vector2i pos = {e.mouseButton.x, e.mouseButton.y};
    sf::Vector2f worldPos;
    if (_view) {
        worldPos = w.mapPixelToCoords(pos, *_view);
    } else {
        worldPos = w.mapPixelToCoords(pos);
    }

    if (_graphic.getGlobalBounds().contains(worldPos)) {
        setValue(!getValue());
    }
}
void Pin::setValue(bool value) { _value = value; }

sf::Vector2i Pin::getScreenSpacePosition(const sf::RenderTarget& target) const {
    return util::projectToScreenSpace(target, getPosition(), _view);
}

sf::Vector2f Pin::getPosition() const {
    return _graphic.getPosition() + sf::Vector2f(RADIUS, RADIUS);
}
void Pin::setPosition(sf::Vector2f pos) {
    _graphic.setPosition(pos - sf::Vector2f(RADIUS, RADIUS));
}
sf::Transformable& Pin::getTransform() { return _graphic; }

bool Pin::canConnect(const Pin& other) const {
    return isCompatible(_type, other._type);
}
bool Pin::collide(sf::Vector2f pos) const { return _graphic.getGlobalBounds().contains(pos); }
bool Pin::collide(const sf::RenderTarget& target, sf::Vector2i pos) const {
    return collide(util::projectToWorldSpace(target, pos, _view));
}

void Pin::draw(sf::RenderWindow& window) const { window.draw(_graphic); }

void Pin::update(const sf::RenderWindow& window, float dt) {
    if (_value) {
        _graphic.setFillColor(sf::Color(COLOR_ACTIVE));
    } else {
        _graphic.setFillColor(sf::Color(COLOR_INACTIVE));
    }

    if (_editable && collide(window, sf::Mouse::getPosition(window))) {
        sf::Color c = _graphic.getFillColor();
        c.r = std::min(c.r + HOVER_MOD, 255);
        c.g = std::min(c.g + HOVER_MOD, 255);
        c.b = std::min(c.b + HOVER_MOD, 255);
        _graphic.setFillColor(c);
    }
}

std::string Pin::getFullPath() const {
    if (_parent) {
        return _parent->getId() + "/" + getId();
    }
    return getId();
}
bool Pin::isCompatible(PinType a, PinType b) {
    switch (a) {
    case Input: return b == Output;
    case Output: return b == Input;
    }
    return false;
}
