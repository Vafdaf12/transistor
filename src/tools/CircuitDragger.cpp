#include "CircuitDragger.h"
#include "circuit/Circuit.h"
#include <iostream>
#include <ostream>

bool CircuitDragger::isActive() const {
    return _dragger.isDragging();
}


void CircuitDragger::onEvent(const sf::RenderWindow& window, const sf::Event& e) {
    if(e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = {e.mouseButton.x, e.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        bool canDrag = false;
        for (const Circuit* c : _selected) {
            if(c->collide(worldPos)) {
                canDrag = true;
                break;
            }
        }
        if(canDrag) {
            std::vector<sf::Transformable*> components;
            for (Circuit* c : _selected) {
                std::vector<sf::Transformable*> children = c->getTransforms();
                std::copy(children.begin(), children.end(), std::back_inserter(components));
            }

            _dragger.beginDrag(components, worldPos);
            std::cout << "Begin" << std::endl;
        }

    }
    if(e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        _dragger.endDrag();
    }
}

void CircuitDragger::update(const sf::RenderWindow& window) {
    sf::Vector2i pos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pos);
    _dragger.update(worldPos);
}

void CircuitDragger::setSelection(std::vector<Circuit*> circuits) {
    _selected = circuits;
    std::cout << "Selection" << std::endl;
}
