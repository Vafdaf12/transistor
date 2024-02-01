#include "PinConnector.h"

PinConnector::PinConnector(CircuitEditor& world) : m_editor(world) {}

void PinConnector::update(const sf::RenderWindow& window, float dt) {
    if (m_firstPin) {
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        m_vertices[1].position = window.mapPixelToCoords(pos, m_editor.getWorldView());
    } else {
        Tool::update(window, dt);
    }
}

void PinConnector::onEvent(const sf::RenderWindow& window, const sf::Event& ev) {
    if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i pos = {ev.mouseButton.x, ev.mouseButton.y};
        Pin* pin = m_editor.collidePin(window, pos);

        if (pin && pin->getType() == Pin::Input) {
            Wire* wire = m_editor.getWire(pin);
            if (wire) {
                Pin* other = const_cast<Pin*>(wire->getFrom());
                m_editor.removeWire(other, const_cast<Pin*>(wire->getTo()));
                pin = other;
            }
        }
        if (pin) {
            m_firstPin = pin;
            sf::Vector2i pos = m_firstPin->getScreenSpacePosition(window);
            m_vertices[0].position = window.mapPixelToCoords(pos, m_editor.getWorldView());
        }
    }

    if (m_firstPin && ev.type == sf::Event::MouseButtonReleased &&
        ev.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2i pos = {ev.mouseButton.x, ev.mouseButton.y};
        Pin* pin = m_editor.collidePin(window, pos);

        if (pin && pin != m_firstPin && Pin::isCompatible(m_firstPin->getType(), pin->getType())) {
            m_editor.addWire(m_firstPin, pin);
        }
        m_firstPin = nullptr;
    }
    if (!m_firstPin) {
        Tool::onEvent(window, ev);
    }
}

void PinConnector::draw(sf::RenderWindow& window) const {
    if (m_firstPin) {
        window.draw(m_vertices, 2, sf::Lines);
    }
    Tool::draw(window);
}
