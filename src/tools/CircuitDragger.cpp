#include "CircuitDragger.h"
#include "circuit/Circuit.h"
#include <iostream>
#include <ostream>

CircuitDragger::CircuitDragger(GameWorld& world, const DragBoard& board)
    : _world(world), _board(board) {}

bool CircuitDragger::isActive() const { return !_circuits.empty(); }

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
        _circuits.clear();
    }
}

void CircuitDragger::update(const sf::RenderWindow& window, float) {
    sf::Vector2i pos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pos);

    for (auto& [delta, c] : _circuits) {
        c->setPosition(worldPos + delta);
    }
}

void CircuitDragger::startDragging(
    const sf::RenderWindow& window, const std::vector<Circuit*> selection
) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

    std::transform(selection.begin(), selection.end(), _circuits.begin(), [&](Circuit* c) {
        return std::make_pair(c->getPosition() - worldPos, c);
    });

    std::cout << "Begin Drag" << std::endl;
}
