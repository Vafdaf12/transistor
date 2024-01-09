#pragma once

#include "SFML/Graphics/RectangleShape.hpp"

#include "ui/Widget.h"

#include <vector>
#include <memory>

namespace ui {
class HBox : ui::Widget {
public:
    HBox() = default;

    bool addWidget(ui::Widget* widget);
    bool removeWidget(ui::Widget* widget);

    bool onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float dt) override;
    void draw(sf::RenderWindow&) const override;

    void setPosition(sf::Vector2f) override;

    inline void setBackground(const sf::Color& color) { m_shape.setFillColor(color); }
    inline void setPadding(float padding) { padding = m_padding; }
    inline void setSeperation(float seperation) { seperation = m_seperation; }

    inline sf::Vector2f getPosition() const override { return m_shape.getPosition(); }
    inline sf::FloatRect getBoundingBox() const override { return m_shape.getGlobalBounds(); }

private:
    void layout();

    std::vector<std::unique_ptr<ui::Widget>> m_widgets;
    float m_padding;
    float m_seperation;

    sf::RectangleShape m_shape;
};
} // namespace ui
