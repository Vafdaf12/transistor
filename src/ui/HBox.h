#pragma once

#include "SFML/Graphics/RectangleShape.hpp"

#include "ui/Widget.h"

#include <memory>
#include <vector>

namespace ui {
class HBox : public ui::Widget {
public:
    HBox() = default;

    bool addWidget(ui::Widget* widget);
    bool removeWidget(ui::Widget* widget);

    bool onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float dt) override;
    void draw(sf::RenderWindow&) const override;

    void move(sf::Vector2f) override;
    void setPosition(sf::Vector2f) override;

    inline void setBackground(const sf::Color& color) { m_shape.setFillColor(color); }
    inline void setPadding(float padding) {
        m_padding = padding;
        layout();
    }
    inline void setSeperation(float seperation) {
        m_seperation = seperation;
        layout();
    }

    inline sf::Vector2f getPosition() const override { return m_shape.getPosition(); }
    inline sf::FloatRect getBoundingBox() const override { return m_shape.getGlobalBounds(); }

    void layout();

private:
    std::vector<std::unique_ptr<ui::Widget>> m_widgets;
    float m_padding;
    float m_seperation;

    sf::RectangleShape m_shape;
};
} // namespace ui
