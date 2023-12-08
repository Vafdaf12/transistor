#pragma once

#include "SFML/Graphics/Drawable.hpp"
#include "event/EventTarget.h"

class Tool : public sf::Drawable {
public:
    inline const EventTarget* getEventTarget() const { return &_target; }

    virtual void update() {}
    virtual bool isActive() const = 0;

protected:
    virtual void draw(sf::RenderTarget&, sf::RenderStates) const override {}

    EventTarget _target;
};