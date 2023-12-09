#pragma once

#include "SFML/Graphics/Drawable.hpp"

#include <utility>

#include "Pin.h"

class Wire : public sf::Drawable {
public:
    Wire(Pin*, Pin*);

    void draw(sf::RenderTarget&, sf::RenderStates) const override;
    void update(const sf::RenderWindow&);

    bool isEndpoint(const Pin* pin) const;
    bool isValid() const;

    bool operator==(const Wire&) const;
    bool operator!=(const Wire& rhs) const { return !(*this == rhs); }

    inline std::pair<const Pin*, const Pin*> getPins() const { return _pins; }

private:
    std::pair<Pin*, Pin*> _pins;
    bool _flags[2] = {false};
};