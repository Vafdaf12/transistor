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

    // Moves the widget by passed-in amount
    virtual void move(sf::Vector2f delta) = 0;

    virtual void setPosition(sf::Vector2f) = 0;
    virtual sf::Vector2f getPosition() const = 0;

    // Sets the anchor of the widget, optionally adjusting the position of the widget accordingly.
    virtual void setAnchor(sf::Vector2f p, bool adjust = true) {
        if (adjust) {
            move(p - m_anchor);
        }
        m_anchor = p;
    }

    // Gets the anchor of the widget
    inline sf::Vector2f getAnchor() const { return m_anchor; }

protected:
    sf::Vector2f m_anchor = {0, 0};
};
} // namespace ui
