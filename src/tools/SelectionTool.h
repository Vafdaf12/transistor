#pragma once

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

#include "app/CircuitEditor.h"
#include "tools/Tool.h"
#include "util/DragBoard.h"

class SelectionTool : public Tool{
public:
    SelectionTool(CircuitEditor& editor, DragBoard& board);

    void onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float) override;
    void draw(sf::RenderWindow&) const override;

    bool isActive() const override { return _active; }
private:

    bool _active = false;

    CircuitEditor& _editor;
    DragBoard& _board;

    sf::RectangleShape _selector;

};
