#pragma once

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

namespace ui {
using State = uint8_t;
enum StateFlags {
    Hover = 1,
    Dragging = 2,
};

class Widget {
public:
    virtual ~Widget() {}
    virtual bool onEvent(const sf::RenderWindow&, const sf::Event&) = 0;

    virtual void update(const sf::RenderWindow&, float dt) = 0;
    virtual void draw(sf::RenderWindow&) const = 0;
    virtual sf::FloatRect getBoundingBox() const = 0;

    virtual void setPosition(sf::Vector2f) = 0;
    virtual sf::Vector2f getPosition() const = 0;
};
} // namespace ui
