#pragma once

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

#include "pin/Pin.h"
#include "SFML/Graphics/Transformable.hpp"
#include <vector>

class Circuit : public sf::Drawable {
public:
    Circuit(size_t numInputs, size_t numOutputs, sf::Vector2f pos = {0, 0});

    bool collide(sf::Vector2f) const;
    const Pin* collidePin(sf::Vector2f) const;

    std::vector<sf::Transformable*> getTransforms();
    sf::FloatRect getBoundingBox() const;
    void draw(sf::RenderTarget&, sf::RenderStates) const override;

    void setColor(sf::Color color);

private:
    static constexpr float PADDING = 10;
    static constexpr float WIDTH = 150;

    sf::RectangleShape _shape;

    std::vector<Pin> _inputs;
    std::vector<Pin> _outputs;
};