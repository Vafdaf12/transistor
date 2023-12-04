#pragma once

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Transformable.hpp"

#include "../pin/Pin.h"
#include "Circuit.h"
#include <vector>

class PassthroughCircuit : public Circuit {
public:
    PassthroughCircuit(size_t size, sf::Vector2f pos = {0, 0});

    bool collide(sf::Vector2f) const override;
    Pin* collidePin(sf::Vector2f) override;

    std::vector<sf::Transformable*> getTransforms() override;
    sf::FloatRect getBoundingBox() const override;

    void draw(sf::RenderTarget&, sf::RenderStates) const override;
    void update(Pin* pin) override;

    void setColor(sf::Color color);

    PassthroughCircuit* clone() override;
private:
    static constexpr float PADDING = 10;
    static constexpr float WIDTH = 150;

    sf::RectangleShape _shape;

    std::vector<Pin> _inputs;
    std::vector<Pin> _outputs;
};