#include "Camera.h"

#include "SFML/Window/Event.hpp"

void Camera::onEvent(const sf::RenderWindow& w, const sf::Event& event) {
    switch (event.type) {

    case sf::Event::Resized: {
        float zoom = m_view.getSize().x / event.size.width;
        m_view.setSize(event.size.width * zoom, event.size.height * zoom);
        break;
    }

    case sf::Event::KeyPressed: {
        if (event.key.code == sf::Keyboard::Space) {
            m_panning = true;
        }
        break;
    }
    case sf::Event::KeyReleased: {
        if (event.key.code == sf::Keyboard::Space) {
            m_panning = false;
        }
        break;
    }
    case sf::Event::MouseWheelScrolled: {
        m_view.zoom(1.0 - (event.mouseWheelScroll.delta * 0.1f));
        break;
    }

    default: break;
    }
}
void Camera::update(const sf::RenderWindow& w, float dt) {
    sf::Vector2i currentMousePos = sf::Mouse::getPosition(w);

    if (m_panning && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2f prevWorldPos = w.mapPixelToCoords(m_mousePosition, m_view);
        sf::Vector2f worldPos = w.mapPixelToCoords(currentMousePos, m_view);

        m_view.move(prevWorldPos - worldPos);
    }

    m_mousePosition = currentMousePos;
}
