#include "CameraController.h"

#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"

void CameraController::onEvent(const sf::RenderWindow& w, const sf::Event& event) {


    switch (event.type) {

    case sf::Event::Resized: {
        float zoom = m_view->getSize().x / event.size.width;
        m_view->setSize(event.size.width * zoom, event.size.height * zoom);
        break;
    }
    case sf::Event::MouseButtonPressed: {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            m_active = true;
        }
        break;
    }
    case sf::Event::MouseButtonReleased: {
        m_active = false;
    }

    case sf::Event::MouseWheelScrolled: {
        m_view->zoom(1.0 - (event.mouseWheelScroll.delta * 0.1f));
        break;
    }

    default: break;
    }
    if(!m_active) {
        Tool::onEvent(w, event);
    }
}
void CameraController::update(const sf::RenderWindow& w, float dt) {
    sf::Vector2i currentMousePos = sf::Mouse::getPosition(w);

    if (m_active) {
        sf::Vector2f prevWorldPos = w.mapPixelToCoords(m_mousePosition, *m_view);
        sf::Vector2f worldPos = w.mapPixelToCoords(currentMousePos, *m_view);

        m_view->move(prevWorldPos - worldPos);
    }

    m_mousePosition = currentMousePos;
    if(!m_active) {
        Tool::update(w, dt);
    }
}
