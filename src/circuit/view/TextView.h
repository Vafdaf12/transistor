#pragma once

#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "circuit/view/CircuitView.h"

class TextView : public CircuitView {
public:
    TextView(const sf::Font& font, const std::string& text, sf::Vector2f pos = {0, 0});
    TextView(const TextView&);

    bool collide(sf::Vector2f) const override;
    void draw(sf::RenderWindow&) const override;

    void setPosition(sf::Vector2f) override;
    void setSize(sf::Vector2f) override;
    sf::Vector2f getMinimumSize() const override;

    TextView* clone() const override;

    sf::FloatRect getBoundingBox() const override { return m_shape.getGlobalBounds(); }
    sf::Vector2f getPosition() const override { return m_shape.getPosition(); }
private:
    void layout();

    sf::RectangleShape m_shape;
    sf::Text m_text;
};
