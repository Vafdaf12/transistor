#pragma once

#include "app/CircuitEditor.h"
#include "tools/Tool.h"
#include "util/DragBoard.h"

class CircuitDragger : public Tool {
public:
    CircuitDragger(CircuitEditor& editor, const DragBoard& board);
    bool isActive() const override;

    void onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float) override;
    void startDragging(const sf::RenderWindow& window, const std::vector<Circuit*> selection);

private:
    std::vector<std::pair<sf::Vector2f, Circuit*>> _circuits;

    CircuitEditor& _editor;
    const DragBoard& _board;
};
