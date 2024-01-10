#include "CircuitButton.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Mouse.hpp"

#include "circuit/Circuit.h"

#include <iostream>

namespace ui {
CircuitButton::CircuitButton(CircuitEditor& editor, Circuit* circuit, Widget* w)
    : WidgetDecorator(w), m_circuit(circuit), m_editor(editor) {}


void CircuitButton::update(const sf::RenderWindow& w, float) {
    sf::Vector2i pos = sf::Mouse::getPosition(w);
    sf::Vector2f worldPos = w.mapPixelToCoords(pos);

    if (m_inner->getBoundingBox().contains(worldPos)) {
        m_state |= Hover;
    } else {
        m_state ^= (m_state & Hover);
    }
}
bool CircuitButton::onEvent(const sf::RenderWindow& w, const sf::Event& e) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        if (m_state & Hover) {
            m_state |= Dragging;
        }
    }
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        if((m_state & Dragging) && !(m_state & Hover)) {
            sf::Vector2i pos(e.mouseButton.x, e.mouseButton.y);
            sf::Vector2f worldPos = w.mapPixelToCoords(pos, m_editor.getWorldView());

            Circuit* c = m_circuit->clone(m_editor.getCircuitId(m_circuit->getId()));
            worldPos -= c->getBoundingBox().getSize() / 2.f;
            c->setPosition(worldPos);
            m_editor.addCircuit(c);
            std::cout << "Spawn circuit in world: " << c->getId() << std::endl;
        }

        m_state ^= (m_state & Dragging);
    }
    return isActive();
}

} // namespace ui
