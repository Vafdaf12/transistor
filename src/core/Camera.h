#pragma once


#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/View.hpp"

class Camera {
public:
    Camera() = default;
    Camera(const sf::View& initialView) : m_view(initialView) {}

    void onEvent(const sf::RenderWindow&, const sf::Event&);
    void update(const sf::RenderWindow&, float dt);

    inline const sf::View& getView() const { return m_view; }

    inline void setTarget(sf::Vector2f pos) { m_view.setCenter(pos); }
private:
    sf::View m_view;
    sf::Vector2i m_mousePosition;

    bool m_panning = false;
};
