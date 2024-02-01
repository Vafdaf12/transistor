#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "app/CircuitEditor.h"
#include "circuit/Pin.h"
#include "tools/Tool.h"

class PinConnector : public Tool {
public:
    PinConnector(CircuitEditor& world);


    void onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float) override;
    void draw(sf::RenderWindow&) const override;

private:
    CircuitEditor& m_editor;

    Pin* m_firstPin = nullptr;

    sf::Vertex m_vertices[2] = {sf::Vertex({0, 0}, sf::Color::White)};
};
