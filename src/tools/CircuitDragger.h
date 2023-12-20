#pragma once

#include "game/GameWorld.h"
#include "pin/Pin.h"
#include "tools/Tool.h"

#include "util/DragBoard.h"

class CircuitDragger : public Tool {
public:
    CircuitDragger(GameWorld& world, const DragBoard& board);
    bool isActive() const override;

    void onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&) override;
    void startDragging(const sf::RenderWindow& window, const std::vector<Circuit*> selection);

private:
    std::vector<std::pair<sf::Vector2f, Circuit*>> _circuits;

    GameWorld& _world;
    const DragBoard& _board;
};
