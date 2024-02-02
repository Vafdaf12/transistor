#include "InputLabel.h"
#include "SFML/System/Vector2.hpp"

namespace ui {

InputLabel::InputLabel(const std::string& text, const sf::Font& font, sf::Vector2f pos) : m_text(text, font) {
    m_background.setFillColor(sf::Color::Transparent);
    m_background.setPosition(pos);
    invalidateRect();
}
bool InputLabel::onEvent(const sf::RenderWindow&, const sf::Event& e) {
    if(m_state & Hover && e.type == sf::Event::TextEntered) {
        // Backspace
        if(e.text.unicode == 8) {
            std::string value = m_text.getString();
            value = value.substr(0, value.length()-1);
            setText(value);
        }
        else if(e.text.unicode < 255) {
            std::string value = m_text.getString();
            value.push_back(e.text.unicode);
            setText(value);
        }
    }
    return m_state & Hover;
}
void InputLabel::update(const sf::RenderWindow& w, float) {
    sf::Vector2f pos = w.mapPixelToCoords(sf::Mouse::getPosition(w));
    if (m_background.getGlobalBounds().contains(pos)) {
        m_state |= Hover;
    } else {
        m_state &= !Hover;
    }
}
void InputLabel::draw(sf::RenderWindow& window) const {
    if(m_background.getFillColor().a != 0) {
        window.draw(m_background);
    }
    window.draw(m_text);
}
void InputLabel::setPosition(sf::Vector2f p) {
    m_background.setPosition(p);
    invalidateRect();
}
void InputLabel::setPadding(float padding) {
    if(m_padding != padding) {
        m_padding = padding;
        invalidateRect();
    }
}
void InputLabel::setFont(const sf::Font& font) {
    m_text.setFont(font);
    invalidateRect();
}
void InputLabel::setText(const std::string& text) {
    if(m_text.getString() == text) {
        return;
    }
    m_text.setString(text);
    invalidateRect();
}
void InputLabel::invalidateRect() {

    sf::Vector2f size(2*m_padding, 2*m_padding);
    size.x += m_text.getGlobalBounds().width;
    size.y += m_text.getCharacterSize();

    sf::Vector2f pos = m_background.getPosition();

    m_background.setSize(size);
    m_text.setPosition(pos + sf::Vector2f(m_padding, m_padding)); 
    Widget::invalidateRect();
}
} // namespace ui
