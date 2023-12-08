#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "game/GameWorld.h"
#include "pin/Pin.h"
#include "tools/Tool.h"

class PinConnector : public Tool {
public:
    PinConnector(const sf::RenderWindow& window, GameWorld& world, Pin* pin = nullptr);

    void update() override;
    bool isActive() const override { return _firstPin; }
    
private:
    void draw(sf::RenderTarget&, sf::RenderStates) const override;


    Pin* _firstPin;
    GameWorld& _world;

    const sf::RenderWindow& _window;
    sf::Vertex _vertices[2] = {sf::Vertex({0, 0}, sf::Color::White)};
};