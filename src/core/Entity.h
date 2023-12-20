#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

namespace core {
    class Entity {
    public:
        virtual ~Entity() {}
        virtual void onEvent(const sf::RenderWindow&, const sf::Event&) = 0;
        virtual void update(const sf::RenderWindow&, float dt) = 0;
        virtual void draw(sf::RenderWindow&) const = 0;
    };
}
