#pragma once

#include "SFML/Graphics/RectangleShape.hpp"

#include "ui/Widget.h"

#include <memory>
#include <vector>

namespace ui {
class HBox : public ui::Widget {
public:
    HBox() = default;

    bool onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float dt) override;
    void draw(sf::RenderWindow&) const override;

    bool addWidget(Widget* child) override;
    bool removeWidget(Widget* child) override;
    Widget* getWidget(size_t i) override;

    // Calculates the total bounding box of the widget
    sf::FloatRect getBoundingBox() const override { return m_shape.getGlobalBounds(); }
    sf::Vector2f getPosition() const override { return m_shape.getPosition(); }
    void setPosition(sf::Vector2f p) override;

    inline void setBackground(const sf::Color& color) { m_shape.setFillColor(color); }
    void setPadding(float padding);
    void setSeperation(float seperation);

    void layout();

private:
    std::vector<std::unique_ptr<ui::Widget>> m_widgets;
    float m_padding;
    float m_seperation;

    sf::RectangleShape m_shape;
};
} // namespace ui
