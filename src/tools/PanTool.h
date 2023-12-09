#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/View.hpp"
#include "Tool.h"

class PanTool : public Tool {
public:
    PanTool(sf::View& v);

    inline bool isActive() const override { return _active; };

    void onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&) override;

private:
    sf::View& _view;

    sf::Vector2i _previousPosition;
    bool _active = false;
    bool _isPanning = false;
};