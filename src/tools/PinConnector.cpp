#include "PinConnector.h"

PinConnector::PinConnector(CircuitEditor& world) : _editor(world) {}

void PinConnector::update(const sf::RenderWindow& window, float) {
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
        _firstPin = _editor.collidePin(window, mousePos);

        Wire* wire = _editor.getWire(_firstPin);

        if (_firstPin && _firstPin->getType() == Pin::Input && wire) {
            _firstPin = const_cast<Pin*>(wire->getFrom());
            _editor.removeWire(_firstPin);
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

        Pin* nextPin = _editor.collidePin(window, mousePos);

        if (!nextPin || nextPin == _firstPin || !_firstPin->canConnect(*nextPin) ||
            (nextPin->getType() == Pin::Input && _editor.getWire(nextPin))) {
            _firstPin = nullptr;
            return;
        }
        _editor.addWire(_firstPin, nextPin);

        _firstPin = nullptr;
        return;
    }
}

void PinConnector::draw(sf::RenderWindow& window) const {
    if (isActive()) {
        window.draw(_vertices, 2, sf::Lines);
    }
}
