#pragma once

#include "Pin.h"

class Wire {
public:
    Wire(Pin*, Pin*);
    ~Wire();

    void update(const sf::RenderWindow&, float dt);
    void draw(sf::RenderWindow&) const;

    bool isEndpoint(const Pin* pin) const;

    bool operator==(const Wire&) const;
    bool operator!=(const Wire& rhs) const { return !(*this == rhs); }

    inline const Pin* getFrom() const { return _from; }
    inline const Pin* getTo() const { return _to; }
    inline bool isValid() const { return _flags[0] != Pin::Dead && _flags[1] != Pin::Dead; }

private:
    Pin* _from;
    Pin* _to;
    Pin::PinFlag _flags[2] = {Pin::None};
};
