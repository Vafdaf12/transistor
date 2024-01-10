#pragma once

#include "SFML/Graphics/Sprite.hpp"
#include "Widget.h"

namespace ui {
class ImageView : public Widget {
public:
    ImageView(const sf::Texture& tex) : _sprite(tex) {}

    bool onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float dt) override;

    inline void draw(sf::RenderWindow& w) const override { w.draw(_sprite); }

    inline sf::FloatRect getBoundingBox() const override { return _sprite.getGlobalBounds(); }
    inline void move(sf::Vector2f delta) override { _sprite.move(delta); }
    inline void setPosition(sf::Vector2f p) override { _sprite.setPosition(p); }
    inline sf::Vector2f getPosition() const override { return _sprite.getPosition(); }

    inline sf::Sprite& getSprite() { return _sprite; }

private:
    uint32_t _state = 0;
    sf::Sprite _sprite;
};
} // namespace ui
