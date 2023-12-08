#pragma once

#include "SFML/Graphics/RectangleShape.hpp"

#include "SFML/Graphics/RenderWindow.hpp"
#include "circuit/Circuit.h"
#include "game/GameWorld.h"
#include "tools/Tool.h"
#include <functional>

class SelectionTool : public Tool{
public:
    using Callback = std::function<void(const std::vector<Circuit*>&)>;

    SelectionTool(const sf::RenderWindow& window, GameWorld& world);

    void update() override;
    bool isActive() const override { return _active; }

    void setOnSelect(Callback&& cb) { _onSelect = cb; }
private:
    void draw(sf::RenderTarget&, sf::RenderStates) const override;

    bool _active = false;

    std::vector<Circuit*> _selection;
    Callback _onSelect;
    GameWorld& _world;

    sf::RectangleShape _selector;
    const sf::RenderWindow& _window;
};