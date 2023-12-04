#pragma once

#include "SFML/Graphics/Drawable.hpp"

#include <utility>

#include "Pin.h"

class Wire : public sf::Drawable {
public:
    Wire(const Pin*, const Pin*);

    void draw(sf::RenderTarget&, sf::RenderStates) const override;

    bool isEndpoint(const Pin* pin) const;

    bool operator==(const Wire&) const;
    bool operator!=(const Wire& rhs) const { return !(*this == rhs); }


private:
    std::pair<const Pin*, const Pin*> _pins;
};