#pragma once
#include "Circuit.h"

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "asset/AssetSystem.h"

class NandCircuit : public Circuit {
public:
    NandCircuit(const std::string& id, const Assets& assets, sf::Vector2f pos = {0, 0});
    ~NandCircuit();

    bool collide(sf::Vector2f) const override;
    Pin* collidePin(sf::Vector2f) override;

    virtual sf::Vector2f getPosition() const override;
    virtual void setPosition(sf::Vector2f) override;

    sf::FloatRect getBoundingBox() const override;

    NandCircuit* clone(const std::string& id) const override;

    void draw(sf::RenderWindow& window) const override;
    void update(const sf::RenderWindow&, float dt) override;

    Pin* queryPin(const std::string& id) override;
private:
    static constexpr float PADDING = 20;

    const Assets& _assets;
    
    sf::RectangleShape _shape;
    sf::Text _text;

    Pin _in1, _in2;
    Pin _out;

    Pin::PinFlag _flags[2] = {Pin::None};
};
