#include "SpriteView.h"

SpriteView::SpriteView(const sf::Texture& texture, sf::Vector2f pos) : m_sprite(texture) {
    m_sprite.setScale(0.7f, 0.7f);
    setPosition(pos);
}
SpriteView::SpriteView(const SpriteView& other) : m_sprite(other.m_sprite) {}

bool SpriteView::collide(sf::Vector2f p) const { return m_sprite.getGlobalBounds().contains(p); }

SpriteView* SpriteView::clone() const { return new SpriteView(*this); }

void SpriteView::draw(sf::RenderWindow& w) const { w.draw(m_sprite); }
