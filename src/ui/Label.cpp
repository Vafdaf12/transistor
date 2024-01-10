#include "Label.h"
namespace ui {

Label::Label(const std::string& text, const sf::Font& font, sf::Vector2f pos) : m_text(text, font) {
    m_background.setPosition(pos);
    layout();
}
bool Label::onEvent(const sf::RenderWindow&, const sf::Event&) { return m_state & Hover; }
void Label::update(const sf::RenderWindow& w, float) {
    sf::Vector2f pos = w.mapPixelToCoords(sf::Mouse::getPosition(w));
    if (m_background.getGlobalBounds().contains(pos)) {
        m_state |= Hover;
    } else {
        m_state &= !Hover;
    }
}
void Label::draw(sf::RenderWindow& window) const {
    if(m_background.getFillColor().a != 0) {
        window.draw(m_background);
    }
    window.draw(m_text);
}
void Label::setPosition(sf::Vector2f p) {
    m_background.setPosition(p);
    layout();
}
void Label::setPadding(float padding) {
    if(m_padding != padding) {
        m_padding = padding;
        layout();
    }
}
void Label::setFont(const sf::Font& font) {
    m_text.setFont(font);
    layout();
}
void Label::setText(const std::string& text) {
    m_text.setString(text);
    layout();
}
void Label::layout() {
    sf::Vector2f size(2*m_padding, 2*m_padding);
    size.x += m_text.getGlobalBounds().width;
    size.y += m_text.getCharacterSize();
    m_background.setSize(size);

    size = {m_padding, m_padding};
    m_text.setPosition(m_background.getPosition() + size); 
}
} // namespace ui
