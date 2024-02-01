#pragma once

#include "app/CircuitEditor.h"
#include "circuit/Circuit.h"
#include "ui/WidgetDecorator.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "spdlog/logger.h"


namespace ui {

class CircuitButton : public WidgetDecorator {
public:
    CircuitButton(CircuitEditor& editor, Circuit* circuit, Widget* w);

    void update(const sf::RenderWindow&, float) override;
    bool onEvent(const sf::RenderWindow&, const sf::Event&) override;

    inline bool isActive() const { return m_state & (Hover | Dragging); }

private:
    WidgetState m_state = 0;

    std::unique_ptr<Circuit> m_circuit;
    CircuitEditor& m_editor;
    std::shared_ptr<spdlog::logger> m_logger;
};
} // namespace ui
