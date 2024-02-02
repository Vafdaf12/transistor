#pragma once

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"

namespace ui {

enum WidgetStateFlags {
    Hover = 1 << 0,
    Dragging = 1 << 1,
};
using WidgetState = uint8_t;

class Widget {
public:
    virtual ~Widget() = default;

    // Handles an event emitted by a window, returning whether the event was consumed
    virtual bool onEvent(const sf::RenderWindow& window, const sf::Event& event) {
        return false;
    }

    // Called every frame with the time (in seconds) since the last update
    virtual void update(const sf::RenderWindow&, float dt) {}

    // Draws the widget to the passed-in window
    virtual void draw(sf::RenderWindow&) const {}

    // Adds a widget as a child and returns true if successful
    virtual bool addWidget(Widget* child) { return false; }

    // Removes a child widget returns true if successful
    virtual bool removeWidget(Widget* child) { return false; }

    // Retrieves the i-th child widget
    virtual Widget* getWidget(size_t i) { return nullptr; }

    // Calculates the total bounding box of the widget
    virtual sf::FloatRect getBoundingBox() const = 0;

    virtual void setPosition(sf::Vector2f) = 0;
    virtual sf::Vector2f getPosition() const = 0;

    virtual void setAnchor(sf::Vector2f anchor, bool move = true) {
        if(anchor == m_anchor) {
            return;
        }
        if(move) {
            sf::Vector2f delta = getPosition() - m_anchor;
            setPosition(anchor + delta);
        }
        m_anchor = anchor;
    }
    inline sf::Vector2f getAnchor() const { return m_anchor; }

    // Triggered when the bounding box of a widget is changed
    virtual void invalidateRect() {
        if(m_parent) {
            m_parent->invalidateRect();
        }
    }

protected:
    // Helper method to get around using friend classes
    static void setParent(Widget* widget, Widget* parent) {
        widget->m_parent = parent;
    }

    Widget* m_parent = nullptr;

    sf::Vector2f m_anchor;
};
} // namespace ui
