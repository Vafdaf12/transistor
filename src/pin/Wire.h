#pragma once

#include "SFML/Graphics/Drawable.hpp"

#include <utility>

#include "Pin.h"

class Wire : public sf::Drawable, public PinObserver {
public:
    Wire(Pin*, Pin*);

    void draw(sf::RenderTarget&, sf::RenderStates) const override;
    void update(Pin* pin) override;
    void onRemove(Pin* pin) override;

    bool isEndpoint(const Pin* pin) const;
    bool isValid() const;

    bool operator==(const Wire&) const;
    bool operator!=(const Wire& rhs) const { return !(*this == rhs); }


private:
    std::pair<Pin*, Pin*> _pins;
};