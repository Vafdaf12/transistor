#pragma once

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"

#include "circuit/Circuit.h"
#include "tools/CircuitDragger.h"
#include "game/GameWorld.h"

#include <stdint.h>

class CircuitButton {
public:
    CircuitButton(GameWorld& world, const Circuit& circuit, CircuitDragger& dragger);

    // Detect when drag leaves the button
    void update(const sf::RenderWindow&);

    // Handle mouse being clicked
    void onEvent(const sf::RenderWindow&, const sf::Event&);

    // Draw the button
    void draw(sf::RenderWindow&) const;

    inline void setView(const sf::View* view) { _view = view; }

    inline sf::RectangleShape& getShape() { return _shape; }

    inline bool isActive() const { return _state; }

private:
    enum State {
        HOVER = 1 << 0,
        DRAGGING = 1 << 1
    };

    GameWorld& _world;
    CircuitDragger& _dragger;
    const Circuit& _circuit;

    sf::RectangleShape _shape;
    const sf::View* _view = nullptr;

    uint8_t _state = 0;
};
