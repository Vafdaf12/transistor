#include "SelectionTool.h"

SelectionTool::SelectionTool(const sf::RenderWindow& window, GameWorld& world) : _window(window), _world(world) {
    _selector.setFillColor({66, 135, 245, 100});
    _target.bind(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }

        _selection.clear();

        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        _selector.setPosition(worldPos);
        _selector.setSize({0, 0});
        _active = true;
    });
    _target.bind(sf::Event::MouseMoved, [&](const sf::Event& event) {
        if (!_active) {
            return;
        }
        _selection = world.collideCircuit(_selector.getGlobalBounds());
    });

    _target.bind(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        if(_onSelect)  {
            _onSelect(_selection);
        }
        _active = false;
    });
}

void SelectionTool::update() {
    if(_active) {
        sf::Vector2i pos = sf::Mouse::getPosition(_window);
        sf::Vector2f worldPos = _window.mapPixelToCoords(pos);
        sf::Vector2f size = worldPos - _selector.getPosition();
        _selector.setSize(size);
    }
}

void SelectionTool::draw(sf::RenderTarget& target, sf::RenderStates) const {
    if(_active) {
        target.draw(_selector);
    }
    for (const Circuit* c : _selection) {
        sf::RectangleShape outline;
        sf::FloatRect rect = c->getBoundingBox();
        outline.setSize(rect.getSize());
        outline.setPosition(rect.getPosition());
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineColor({66, 135, 245, 150});
        outline.setOutlineThickness(5);
        target.draw(outline);
    }
}
