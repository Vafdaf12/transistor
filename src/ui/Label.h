#pragma once


#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include "ui/Widget.h"
namespace ui {
class Label : public Widget {
public:
    Label(const std::string& text, const sf::Font& font, sf::Vector2f pos = {0, 0});

    bool onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float dt) override;
    void draw(sf::RenderWindow&) const override;

    inline sf::Vector2f getPosition() const override { return m_background.getPosition(); }
    inline sf::FloatRect getBoundingBox() const override { return m_background.getGlobalBounds(); }

    inline void setBackground(const sf::Color& color) { m_background.setFillColor(color); }
    inline void setForeground(const sf::Color& color) { m_text.setFillColor(color); }
    void setPosition(sf::Vector2f p) override;
    void setPadding(float padding);
    void setFont(const sf::Font& font);
    void setText(const std::string& text);

private:
    void layout();

    sf::Text m_text;
    sf::RectangleShape m_background;
    State m_state = 0;
    float m_padding = 0;
};
}
