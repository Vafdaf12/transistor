#include "PinConnector.h"

PinConnector::PinConnector(CircuitEditor& world) : _editor(world) {}

void PinConnector::update(const sf::RenderWindow& window, float) {
    if (isActive()) {
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        _vertices[1].position = window.mapPixelToCoords(pos, _editor.getWorldView());
    }
}

void PinConnector::onEvent(const sf::RenderWindow& window, const sf::Event& e) {
    sf::Event::MouseButtonEvent event = e.mouseButton;
    sf::Vector2i eventPos = {event.x, event.y};

    if (e.type == sf::Event::MouseButtonPressed && event.button == sf::Mouse::Left) {
        Pin* pin = _editor.collidePin(window, eventPos);
        if (!pin) {
            return;
        }

        Wire* wire = _editor.getWire(pin);
        if (wire && pin->getType() == Pin::Input) {
            _firstPin = const_cast<Pin*>(wire->getFrom());
            _editor.removeWire(_firstPin, const_cast<Pin*>(wire->getTo()));
        } else {
            _firstPin = pin;

        }
        sf::Vector2i pos = _firstPin->getScreenSpacePosition(window);
        _vertices[0].position = window.mapPixelToCoords(pos, _editor.getWorldView());
    }
    if (_firstPin && e.type == sf::Event::MouseButtonReleased && event.button == sf::Mouse::Left) {
        Pin* pin = _editor.collidePin(window, eventPos);
        if (pin && pin != _firstPin && _firstPin->canConnect(*pin)) {
            _editor.addWire(_firstPin, pin);
        }
        _firstPin = nullptr;
    }
}

void PinConnector::draw(sf::RenderWindow& window) const {
    if (isActive()) {
        window.draw(_vertices, 2, sf::Lines);
    }
}
