#include "SelectionTool.h"
#include "SFML/Graphics/RenderWindow.hpp"

SelectionTool::SelectionTool( GameWorld& world, DragBoard& board) : _world(world), _board(board) {
    _selector.setFillColor({66, 135, 245, 100});
}

void SelectionTool::onEvent(const sf::RenderWindow& window, const sf::Event& event) {
    if(event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }

        _board.clearSelection();

        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        if(!_world.collideCircuit(worldPos) && !_world.collidePin(worldPos)) {
            _selector.setPosition(worldPos);
            _selector.setSize({0, 0});
            _active = true;
        }
    }
    if(event.type == sf::Event::MouseMoved) {
        if (!_active) {
            return;
        }
        _board.setSelection(_world.collideCircuit(_selector.getGlobalBounds()));
    }

    if(event.type == sf::Event::MouseButtonReleased) {
        _active = false;
    }
    
}

void SelectionTool::update(const sf::RenderWindow& window, float) {
    if(_active) {
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(pos);
        sf::Vector2f size = worldPos - _selector.getPosition();
        _selector.setSize(size);
    }
}

void SelectionTool::draw(sf::RenderWindow& window) const {
    if(_active) {
        window.draw(_selector);
    }
    for (const Circuit* c : _board.getSelection()) {
        sf::RectangleShape outline;
        sf::FloatRect rect = c->getBoundingBox();
        outline.setSize(rect.getSize());
        outline.setPosition(rect.getPosition());
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineColor({66, 135, 245, 150});
        outline.setOutlineThickness(5);
        window.draw(outline);
    }
}
