#include "PinConnector.h"

PinConnector::PinConnector(GameWorld& world) : _world(world) {}

void PinConnector::update(const sf::RenderWindow& window) {
    if (isActive()) {
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        _vertices[1].position = window.mapPixelToCoords(pos);
    }
}

void PinConnector::onEvent(const sf::RenderWindow& window, const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        _firstPin = _world.collidePin(window, mousePos);
        const Wire* wire = _world.getConnectedWire(_firstPin);

        if (_firstPin && _firstPin->getType() == Pin::Input && wire) {
            _firstPin = const_cast<Pin*>(wire->getTo());
            _world.removeWire(wire);
        }
        if (_firstPin) {
            _vertices[0].position = _firstPin->getWorldSpacePosition(window);
        }
    }
    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        Pin* nextPin = _world.collidePin(window, mousePos);

        if (!nextPin || nextPin == _firstPin || !_firstPin->canConnect(*nextPin) ||
            (nextPin->getType() == Pin::Input && _world.isPinConnected(nextPin))) {
            _firstPin = nullptr;
            return;
        }
        _world.connectPins(_firstPin, nextPin);

        _firstPin = nullptr;
        return;
    }
}

void PinConnector::draw(sf::RenderWindow& window) const {
    if (isActive()) {
        window.draw(_vertices, 2, sf::Lines);
    }
}
