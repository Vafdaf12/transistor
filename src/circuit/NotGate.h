#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"

#include <string>

#include "Pin.h"
#include "asset/AssetSystem.h"
#include "circuit/Circuit.h"

class NotGate : public Circuit {
public:
    NotGate(const std::string& id, const Assets& assets, sf::Vector2f pos = {0, 0});
    ~NotGate();

    virtual sf::Vector2f getPosition() const override;
    virtual void setPosition(sf::Vector2f) override;

    sf::FloatRect getBoundingBox() const override;

    void draw(sf::RenderWindow& window) const override;
    void update(const sf::RenderWindow&, float dt) override;

    bool collide(sf::Vector2f) const override;

    NotGate* clone(const std::string& id) const override;

    void toJson(nlohmann::json& j) const override;
    inline std::vector<Pin*> getAllPins() override { return {&_input, &_output}; }

private:
    const Assets& _assets;

    sf::Sprite _sprite;

    Pin _input, _output;
    Pin::PinFlag _flag = Pin::Dirty;
};
