#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/View.hpp"
#include "Tool.h"

class PanTool : public Tool {
public:
    PanTool(sf::View& v, const sf::RenderWindow& w);

    void update() override;
    inline bool isActive() const override { return _isPanning; };

private:
    sf::View& _view;
    const sf::RenderWindow& _window;

    sf::Vector2i _previousPosition;
    bool _isPanning = false;
};