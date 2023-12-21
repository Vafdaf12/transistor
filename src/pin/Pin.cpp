#include "Pin.h"

#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

#include "SFML/Window/Event.hpp"
#include "circuit/Circuit.h"



Pin::Pin(const std::string& id, PinType type, sf::Vector2f pos, int state)
    : _type(type), _value(state), _id(id) {
    _graphic.setPosition(pos - sf::Vector2f(RADIUS, RADIUS));
    _graphic.setOutlineColor(sf::Color(COLOR_OUTLINE));
    _graphic.setOutlineThickness(1);
    _graphic.setRadius(10);

    if (_value) {
        _graphic.setFillColor(sf::Color(COLOR_ACTIVE));
    } else {
        _graphic.setFillColor(sf::Color(COLOR_INACTIVE));
    }
}
Pin::~Pin() {
    for (PinFlag* flag : _flags) {
        *flag = Dead;
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
void Pin::setValue(bool value) {
    if (_value == value) {
        return;
    }
    _value = value;
    changed();
}

sf::Vector2f Pin::getCenter() const {
    float r = _graphic.getRadius();
    return _graphic.getPosition() + sf::Vector2f(r, r);
}
sf::Vector2f Pin::getWorldSpacePosition(const sf::RenderTarget& target) const {
    sf::Vector2f orgPos = getCenter();
    if (!_view) {
        return orgPos;
    }

    sf::Vector2i screenSpace = target.mapCoordsToPixel(orgPos, *_view);
    return target.mapPixelToCoords(screenSpace);
}

sf::Vector2i Pin::getScreenSpacePosition(const sf::RenderTarget& target) const {
    sf::Vector2f orgPos = getCenter();
    if (!_view) {
        return target.mapCoordsToPixel(orgPos);
    }
    return target.mapCoordsToPixel(orgPos, *_view);
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
bool Pin::collide(const sf::RenderTarget& target, sf::Vector2i pos) const {
    if(!_view) {
        return collide(target.mapPixelToCoords(pos));
    }
    return collide(target.mapPixelToCoords(pos, *_view));
}
sf::Transformable& Pin::getTransform() { return _graphic; }
void Pin::draw(sf::RenderWindow& window) const { window.draw(_graphic); }

bool Pin::connect(PinFlag* obs) { return _flags.insert(obs).second; }
bool Pin::disconnect(PinFlag* obs) { return _flags.erase(obs) > 0; }

void Pin::update(const sf::RenderWindow& window, float dt) {
    if(_value) {
        _graphic.setFillColor(sf::Color(COLOR_ACTIVE));
    } else {
        _graphic.setFillColor(sf::Color(COLOR_INACTIVE));
    }

    if(collide(window, sf::Mouse::getPosition(window))) {
        sf::Color c = _graphic.getFillColor();
        c.r = std::min(c.r + COLOR_MUL, 255);
        c.g = std::min(c.g + COLOR_MUL, 255);
        c.b = std::min(c.b + COLOR_MUL, 255);
        _graphic.setFillColor(c);
    }
}

std::string Pin::getFullPath() const {
    if (_parent) {
        return _parent->getId() + "/" + _id;
    }
    return _id;
}
void Pin::changed() {
    for (PinFlag* flag : _flags) {
        if (*flag == Dirty) {
            *flag = None;
        }
        if (*flag == None) {
            *flag = Dirty;
        }
    }
}
