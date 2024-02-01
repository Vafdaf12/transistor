#pragma once

#include "Tool.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/View.hpp"

class CameraController : public Tool {
public:
    CameraController() = default;
    CameraController(sf::View& view) : m_view(&view) {}

    void onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float dt) override;

    void setView(sf::View& view) { m_view = &view; }
    inline const sf::View* getView() const { return m_view; }

    inline void setTarget(sf::Vector2f pos) {
        if (m_view) {
            m_view->setCenter(pos);
        }
    }

private:
    sf::View* m_view = nullptr;
    sf::Vector2i m_mousePosition;

    bool m_active = false;
};
