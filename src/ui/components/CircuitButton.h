#pragma once

#include "SFML/Graphics/RenderWindow.hpp"

#include "app/CircuitEditor.h"
#include "circuit/Circuit.h"
#include "ui/Widget.h"

namespace ui {

class CircuitButton : public Widget {
public:
    CircuitButton(CircuitEditor& editor, Circuit* circuit, Widget* w);

    void draw(sf::RenderWindow&) const override;
    void update(const sf::RenderWindow&, float) override;
    bool onEvent(const sf::RenderWindow&, const sf::Event&) override;

    inline sf::FloatRect getBoundingBox() const override { return _widget->getBoundingBox(); }

    inline void move(sf::Vector2f p) override { _widget->move(p); }
    inline void setPosition(sf::Vector2f p) override { _widget->setPosition(p); }
    inline sf::Vector2f getPosition() const override { return _widget->getPosition(); }

    inline bool isActive() const { return _state & (Hover | Dragging); }

private:
    State _state = 0;

    std::unique_ptr<Widget> _widget;

    std::unique_ptr<Circuit> _circuit;

    CircuitEditor& _editor;
};
} // namespace ui
