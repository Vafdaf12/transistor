#pragma once

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Transformable.hpp"

#include "pin/Pin.h"
#include "Circuit.h"
#include <vector>

class PassthroughCircuit : public Circuit {
public:
    PassthroughCircuit(const std::string& id, size_t size, sf::Vector2f pos = {0, 0});
    ~PassthroughCircuit();

    bool collide(sf::Vector2f) const override;
    Pin* collidePin(sf::Vector2f) override;

    std::vector<sf::Transformable*> getTransforms() override;
    sf::FloatRect getBoundingBox() const override;

    void draw(sf::RenderWindow& window) const override;
    void update(const sf::RenderWindow&) override;

    void setColor(sf::Color color);
    inline sf::Color getColor() const { return _shape.getFillColor(); }

    inline size_t getSize() const { return _inputs.size(); }

    PassthroughCircuit* clone(const std::string& newId) override;

    Pin* queryPin(const std::string& id) override;
private:
    static constexpr float PADDING = 10;
    static constexpr float WIDTH = 150;

    sf::RectangleShape _shape;

    std::vector<Pin> _inputs;
    std::vector<Pin> _outputs;
    bool* _flags;
};