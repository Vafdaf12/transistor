#include "PinConnector.h"

#include <iostream>

PinConnector::PinConnector(const sf::RenderWindow& window, GameWorld& world, Pin* pin)
    : _world(world), _window(window), _firstPin(pin) {

    if(!pin) {
        _target.bind(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
            if (event.mouseButton.button != sf::Mouse::Left) {
                return;
            }
            sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
            sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

            _firstPin = world.collidePin(worldPos);
            if(_firstPin) {
                _vertices[0].position = _firstPin->getCenter();
            }
        });
    }
    else {
        _vertices[0].position = _firstPin->getCenter();
    }


    _target.bind(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        std::cout << "Disconnected" << std::endl;
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        Pin* nextPin = world.collidePin(worldPos);

        if (!nextPin || nextPin == _firstPin || !_firstPin->canConnect(*nextPin)) {
            _firstPin = nullptr;
            return;
        }
        world.connectPins(_firstPin, nextPin);

        _firstPin = nullptr;
        return;
    });
    }

void PinConnector::update() {
    if(isActive()) {
        sf::Vector2i pos = sf::Mouse::getPosition(_window);
        _vertices[1].position = _window.mapPixelToCoords(pos);
    }
}

void PinConnector::draw(sf::RenderTarget& target, sf::RenderStates) const {
    if(isActive()) {
        target.draw(_vertices, 2, sf::Lines);
    }
}
