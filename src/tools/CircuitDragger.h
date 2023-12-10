#pragma once

#include "ComponentDragger.h"
#include "game/GameWorld.h"
#include "pin/Pin.h"
#include "tools/Tool.h"

class CircuitDragger : public Tool {
public:
    CircuitDragger(GameWorld& world);
    bool isActive() const override;

    void onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&) override;
    void setSelection(std::vector<Circuit*> circuits);

private:
    std::vector<Circuit*> _selected;
    ComponentDragger _dragger;
    GameWorld& _world;
};