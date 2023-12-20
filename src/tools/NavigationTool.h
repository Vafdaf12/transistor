#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/View.hpp"
#include "Tool.h"

class NavigationTool : public Tool {
public:
    NavigationTool(sf::View& v);

    inline bool isActive() const override { return _state != None; };

    void onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float) override;

private:
    enum State { None, Panning };

    sf::View& _view;

    sf::Vector2i _prevMousePosition;
    State _state = None;
};
