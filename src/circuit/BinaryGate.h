#pragma once

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

#include "pin/Pin.h"
#include "circuit/Circuit.h"

class BinaryGate : public Circuit {
public:
    using Func = int(*)(int, int);

    BinaryGate(const sf::Texture& tex, Func fn,  sf::Vector2f pos = {0, 0});

    bool collide(sf::Vector2f) const override;
    Pin* collidePin(sf::Vector2f) override;
    std::vector<sf::Transformable*> getTransforms() override;
    sf::FloatRect getBoundingBox() const override;

    BinaryGate* clone() override;

    void draw(sf::RenderTarget&, sf::RenderStates) const override;
    void update(Pin* pin) override;


    static int And(int, int);
    static int Or(int, int);
    static int Nand(int, int);
    static int Nor(int, int);
    static int Xor(int, int);

private:
    static constexpr float PADDING = 20;

    sf::Sprite _sprite;
    
    Pin _in1, _in2;
    Pin _out;

    Func _process;
};