#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "app/CircuitEditor.h"
#include "circuit/Pin.h"
#include "tools/Tool.h"

class PinConnector : public Tool {
public:
    PinConnector(CircuitEditor& world);

    bool isActive() const override { return _firstPin; }

    void onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float) override;
    void draw(sf::RenderWindow&) const override;

private:
    CircuitEditor& _editor;

    Pin* _firstPin = nullptr;

    sf::Vertex _vertices[2] = {sf::Vertex({0, 0}, sf::Color::White)};
};
