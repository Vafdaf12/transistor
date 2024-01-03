#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"

#include <string>

#include "Pin.h"
#include "circuit/Circuit.h"

class NotGate : public Circuit {
public:
    NotGate(const std::string& id, const sf::Texture& assets, sf::Vector2f pos = {0, 0});
    NotGate(const NotGate& other);

    virtual sf::Vector2f getPosition() const override;
    virtual void setPosition(sf::Vector2f) override;

    sf::FloatRect getBoundingBox() const override;

    void draw(sf::RenderWindow& window) const override;
    void update(const sf::RenderWindow&, float dt) override;

    bool collide(sf::Vector2f) const override;

    NotGate* clone(const std::string& id) const override;

    void toJson(nlohmann::json& j) const override;
    inline std::vector<Pin*> getAllPins() override { return {&m_input, &m_output}; }

private:
    Pin m_input, m_output;

    sf::Sprite m_sprite;
};
