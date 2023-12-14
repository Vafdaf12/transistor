#include "CircuitDragger.h"
#include "circuit/Circuit.h"
#include <iostream>
#include <ostream>

CircuitDragger::CircuitDragger(GameWorld& world, const DragBoard& board) : _world(world), _board(board) {}

bool CircuitDragger::isActive() const { return _dragger.isDragging(); }

void CircuitDragger::onEvent(const sf::RenderWindow& window, const sf::Event& e) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = {e.mouseButton.x, e.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        bool canDrag = false;
        for (const Circuit* c : _board.getSelection()) {
            if (c->collide(worldPos)) {
                canDrag = true;
                break;
            }
        }
        if (!canDrag) {
            Circuit* c = _world.collideCircuit(worldPos);
            if (c) {
                startDragging(window, {c});
            }
        } else {
            startDragging(window, _board.getSelection());
        }
    }
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        _dragger.endDrag();
    }
}

void CircuitDragger::update(const sf::RenderWindow& window) {
    sf::Vector2i pos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pos);
    _dragger.update(worldPos);
}

void CircuitDragger::startDragging(const sf::RenderWindow& window, const std::vector<Circuit*> selection) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

    std::vector<sf::Transformable*> components;
    for (Circuit* c : selection) {
        std::vector<sf::Transformable*> children = c->getTransforms();
        std::copy(children.begin(), children.end(), std::back_inserter(components));
    }
    if(components.empty()) {
        std::cout << "Nothing to drag" << std::endl;
        return;
    }
    std::cout << "Begin Drag" << std::endl;
    _dragger.beginDrag(components, worldPos);
}
