#include "CircuitButton.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Mouse.hpp"

#include "circuit/Circuit.h"

#include <iostream>

namespace ui {
CircuitButton::CircuitButton(CircuitEditor& editor, Circuit* circuit, Widget* w)
    : _circuit(circuit), _editor(editor), _widget(w) {}

void CircuitButton::update(const sf::RenderWindow& w, float) {
    sf::Vector2i pos = sf::Mouse::getPosition(w);
    sf::Vector2f worldPos = w.mapPixelToCoords(pos);

    if (_widget->getBoundingBox().contains(worldPos)) {
        _state |= Hover;
    } else {
        _state ^= (_state & Hover);
    }
}
bool CircuitButton::onEvent(const sf::RenderWindow& w, const sf::Event& e) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        if (_state & Hover) {
            _state |= Dragging;
        }
    }
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        if((_state & Dragging) && !(_state & Hover)) {
            sf::Vector2i pos(e.mouseButton.x, e.mouseButton.y);
            sf::Vector2f worldPos = w.mapPixelToCoords(pos, _editor.getWorldView());

            Circuit* c = _circuit->clone(_editor.getCircuitId(_circuit->getId()));
            worldPos -= c->getBoundingBox().getSize() / 2.f;
            c->setPosition(worldPos);
            _editor.addCircuit(c);
            std::cout << "Spawn circuit in world: " << c->getId() << std::endl;
        }

        _state ^= (_state & Dragging);
    }
    return isActive();
}
void CircuitButton::draw(sf::RenderWindow& window) const { _widget->draw(window); }

} // namespace ui
