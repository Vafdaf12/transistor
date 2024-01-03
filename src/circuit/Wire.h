#pragma once

#include "Pin.h"

class Wire {
public:
    Wire(const Pin* source, Pin* target);

    void update(const sf::RenderWindow&, float dt);
    void draw(sf::RenderWindow&) const;

    bool isEndpoint(const Pin* pin) const;

    bool operator==(const Wire&) const;
    bool operator!=(const Wire& rhs) const { return !(*this == rhs); }

    inline const Pin* getFrom() const { return m_source; }
    inline const Pin* getTo() const { return m_target; }

private:
    const Pin* m_source;
    Pin* m_target;
};
