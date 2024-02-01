#pragma once

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "CircuitView.h"

class SpriteView : public CircuitView {
public:
    SpriteView(const sf::Texture& texture, sf::Vector2f pos = {0, 0});
    SpriteView(const SpriteView&);

    bool collide(sf::Vector2f) const override;
    SpriteView* clone() const override;
    void draw(sf::RenderWindow&) const override;

    void setPosition(sf::Vector2f p) override { m_sprite.setPosition(p); }
    sf::Vector2f getMinimumSize() const override { return m_sprite.getGlobalBounds().getSize(); }

    sf::FloatRect getBoundingBox() const override { return m_sprite.getGlobalBounds(); }
    sf::Vector2f getPosition() const override { return m_sprite.getPosition(); }
private:
    sf::Sprite m_sprite;
};
