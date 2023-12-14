#include "CircuitButton.h"

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/Window/Mouse.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/System/Vector2.hpp"

#include "circuit/Circuit.h"
#include "game/GameWorld.h"

#include <iostream>

CircuitButton::CircuitButton(GameWorld& world, const Circuit& circuit, CircuitDragger& dragger)
    : _circuit(circuit), _world(world), _dragger(dragger) {
    _shape.setSize({100, 100});
}

void CircuitButton::update(const sf::RenderWindow& w) {
    const sf::View& view = _view ? *_view : w.getView();
    sf::Vector2i pos = sf::Mouse::getPosition(w);
    sf::Vector2f worldPos = w.mapPixelToCoords(pos, view);

    if(_state == 0 && _shape.getGlobalBounds().contains(worldPos)) {
        _state |= HOVER;
    }
    if(_state & HOVER && !_shape.getGlobalBounds().contains(worldPos)) {
        _state ^= HOVER;
        if(_state & DRAGGING) {
            worldPos = w.mapPixelToCoords(pos);
            Circuit* c = _circuit.clone(_world.assignCircuitId(_circuit.getId()));
            worldPos -= c->getBoundingBox().getSize()/2.f;
            for(sf::Transformable* t : c->getTransforms()) {
                t->move(worldPos);
            }
            _world.addCircuit(c);
            _dragger.startDragging(w, {c});
            std::cout << "Spawn circuit in preview mode:" << c->getId() << std::endl;
        }
    }
}
void CircuitButton::onEvent(const sf::RenderWindow& w, const sf::Event& e) {
    if(e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        if (_state & HOVER) {
            _state |= DRAGGING;
        }
    }
    if(e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        if((_state ^ DRAGGING) == 0) {
            std::cout << "Place circuit in world" << std::endl;
        }
        _state &= !DRAGGING;
    }
}
void CircuitButton::draw(sf::RenderWindow& window) const {
    window.draw(_shape);
}
