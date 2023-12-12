#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"

#include <string>

#include "pin/Pin.h"
#include "circuit/Circuit.h"
#include "asset/AssetSystem.h"


class NotGate : public Circuit {
public:
    NotGate(const std::string& id, const Assets& assets,  sf::Vector2f pos = {0, 0});
    ~NotGate();

    std::vector<sf::Transformable*> getTransforms() override;
    sf::FloatRect getBoundingBox() const override;

    void draw(sf::RenderWindow& window) const override;
    void update(const sf::RenderWindow&) override;

    bool collide(sf::Vector2f) const override;
    Pin* collidePin(sf::Vector2f) override;
    Pin* queryPin(const std::string& id) override;

    NotGate* clone(const std::string& id) const override;

private:
    const Assets& _assets;

    sf::Sprite _sprite;

    Pin _input, _output;
    Pin::PinFlag _flag = Pin::Dirty;
};
