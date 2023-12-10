#pragma once

#include "SFML/Graphics/Drawable.hpp"

#include "Pin.h"

class Wire : public sf::Drawable {
public:
    Wire(Pin*, Pin*);
    ~Wire();

    void draw(sf::RenderTarget&, sf::RenderStates) const override;
    void update(const sf::RenderWindow&);

    bool isEndpoint(const Pin* pin) const;

    bool operator==(const Wire&) const;
    bool operator!=(const Wire& rhs) const { return !(*this == rhs); }


    inline const Pin* getFrom() const { return _from; }
    inline const Pin* getTo() const { return _to; }
    inline bool isValid() const { return _flag != Pin::Dead; }

private:
    Pin *_from;
    Pin *_to;
    Pin::PinFlag _flag = Pin::None;
};