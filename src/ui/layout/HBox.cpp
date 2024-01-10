#include "HBox.h"
#include "SFML/Graphics/Rect.hpp"
#include "util/util.h"
#include <algorithm>
#include <vector>
namespace ui {

bool HBox::addWidget(ui::Widget* widget) {
    bool exists = std::any_of(m_widgets.begin(), m_widgets.end(), [widget](const auto& w) {
        return w.get() == widget;
    });
    if (!exists) {
        m_widgets.emplace_back(widget);
        layout();
    }
    return !exists;
}
bool HBox::removeWidget(ui::Widget* widget) {
    if (std::erase_if(m_widgets, [widget](const auto& w) { return w.get() == widget; }) > 0) {
        layout();
        return true;
    }
    return false;
}

bool HBox::onEvent(const sf::RenderWindow& win, const sf::Event& e) {
    bool handled = false;
    for (auto& w : m_widgets) {
        if (w->onEvent(win, e)) {
            handled = true;
        }
    }
    return handled;
}
void HBox::update(const sf::RenderWindow& win, float dt) {
    for (auto& w : m_widgets) {
        w->update(win, dt);
    }
}
void HBox::draw(sf::RenderWindow& win) const {
    win.draw(m_shape);
    for (auto& w : m_widgets) {
        w->draw(win);
    }
}

void HBox::move(sf::Vector2f delta) {
    m_shape.move(delta);
    for (auto& child : m_widgets) {
        child->move(delta);
    }
}
void HBox::setPosition(sf::Vector2f pos) {
    m_shape.setPosition(pos);
    layout();
}

void HBox::layout() {
    sf::Vector2f size(0, 0);
    sf::Vector2f pos = m_shape.getPosition() + sf::Vector2f(m_padding, m_padding);
    for (auto& child : m_widgets) {
        sf::FloatRect rect = child->getBoundingBox();
        child->setPosition(pos);

        pos.x += rect.width + m_seperation;

        size.x += rect.width + m_seperation;
        size.y = util::max(size.y, rect.height);
    }
    size.x = size.x - m_seperation + 2 * m_padding;
    size.y = size.y + 2 * m_padding;

    m_shape.setSize(size);
}
} // namespace ui
