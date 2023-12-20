#pragma once

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

#include "game/GameWorld.h"
#include "tools/Tool.h"
#include "util/DragBoard.h"

class SelectionTool : public Tool{
public:
    SelectionTool(GameWorld& world, DragBoard& board);

    void onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float) override;
    void draw(sf::RenderWindow&) const override;

    bool isActive() const override { return _active; }
private:

    bool _active = false;

    GameWorld& _world;
    DragBoard& _board;

    sf::RectangleShape _selector;

};
