#include "Button.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Mouse.hpp"

#include "ui/Widget.h"
#include "ui/WidgetDecorator.h"

namespace ui {
Button::Button(Widget* w) : WidgetDecorator(w) {}
Button::Button(Widget* w, Handler&& h) : WidgetDecorator(w), m_onClick(h) {}

void Button::update(const sf::RenderWindow& w, float) {
    sf::Vector2i pos = sf::Mouse::getPosition(w);
    sf::Vector2f worldPos = w.mapPixelToCoords(pos);

    if (m_inner->getBoundingBox().contains(worldPos)) {
        m_state |= Hover;
    } else {
        m_state ^= (m_state & Hover);
    }
}
bool Button::onEvent(const sf::RenderWindow& w, const sf::Event& e) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        if (m_state & Hover) {
            m_state |= Dragging;
        }
    }
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        if ((m_state & Dragging) && (m_state & Hover) && m_onClick) {
            m_onClick();
        }

        m_state ^= (m_state & Dragging);
    }
    return isActive();
}

} // namespace ui
