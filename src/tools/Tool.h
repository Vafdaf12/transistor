#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

class Tool {
public:
    virtual ~Tool() {}
    virtual bool isActive() const = 0;

    virtual void update(const sf::RenderWindow&, float dt) {}
    virtual void onEvent(const sf::RenderWindow&, const sf::Event&) {}
    virtual void draw(sf::RenderWindow&) const {}
};
