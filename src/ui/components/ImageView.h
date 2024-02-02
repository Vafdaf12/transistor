#pragma once

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Vector2.hpp"
#include "ui/Widget.h"

namespace ui {
class ImageView : public Widget {
public:
    ImageView() = default;
    ImageView(const sf::Texture& tex) : _sprite(tex) {}
    ImageView(const ImageView&) = default;

    bool onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float dt) override;

    inline void draw(sf::RenderWindow& w) const override { w.draw(_sprite); }

    inline sf::FloatRect getBoundingBox() const override { return _sprite.getGlobalBounds(); }
    inline void setPosition(sf::Vector2f p) override { _sprite.setPosition(p); }
    inline sf::Vector2f getPosition() const override { return _sprite.getPosition(); }

    inline void setTint(const sf::Color& color) { _sprite.setColor(color); }
    inline void setScale(const sf::Vector2f scale) { _sprite.setScale(scale); }
    inline void setTexture(const sf::Texture& texture) { _sprite.setTexture(texture); }

private:
    uint32_t _state = 0;
    sf::Sprite _sprite;
};
} // namespace ui
