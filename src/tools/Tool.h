#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"
#include "core/Entity.h"

class Tool : public core::Entity {
public:
    virtual bool isActive() const = 0;

    virtual void update(const sf::RenderWindow&, float dt) override {}
    virtual void onEvent(const sf::RenderWindow&, const sf::Event&) override {}
    virtual void draw(sf::RenderWindow&) const override {}
};
