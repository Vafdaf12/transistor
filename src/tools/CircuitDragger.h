#pragma once

#include "app/CircuitEditor.h"
#include "tools/Tool.h"
#include "util/DragBoard.h"

class CircuitDragger : public Tool {
public:
    CircuitDragger(CircuitEditor& editor, DragBoard& board);

    void onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float) override;
    void startDragging(const sf::RenderWindow& window, const std::vector<Circuit*> selection);

private:
    bool isActive() const;
    std::vector<std::pair<sf::Vector2f, Circuit*>> m_circuits;

    CircuitEditor& m_editor;
    DragBoard& m_board;
};
