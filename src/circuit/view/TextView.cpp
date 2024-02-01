#include "TextView.h"
#include "SFML/System/Vector2.hpp"
TextView::TextView(const sf::Font& font, const std::string& text, sf::Vector2f pos) {
    m_text.setString(text);
    m_text.setFont(font);

    m_text.setFillColor(sf::Color::Red);
    m_shape.setFillColor(sf::Color::White);
    m_shape.setSize(getMinimumSize());
    setPosition(pos);
}
TextView::TextView(const TextView& other) : m_shape(other.m_shape), m_text(other.m_text) {}

bool TextView::collide(sf::Vector2f p) const { return m_shape.getGlobalBounds().contains(p); }

void TextView::draw(sf::RenderWindow& w) const {
    w.draw(m_shape);
    w.draw(m_text);
}

void TextView::setPosition(sf::Vector2f p) {
    m_shape.setPosition(p);
    layout();
}

void TextView::setSize(sf::Vector2f p) {
    sf::Vector2f pos = getMinimumSize();
    p = {std::max(p.x, pos.x), std::max(p.y, pos.y)};
    m_shape.setSize(p);
    layout();
}

sf::Vector2f TextView::getMinimumSize() const {
    return {m_text.getGlobalBounds().width, static_cast<float>(m_text.getCharacterSize())};
}

TextView* TextView::clone() const { return new TextView(*this); }

void TextView::layout() {
    sf::Vector2f pos = m_shape.getSize() - getMinimumSize(); 
    m_text.setPosition(m_shape.getPosition() + pos / 2.0f);
}
