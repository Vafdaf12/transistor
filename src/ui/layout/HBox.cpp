#include "HBox.h"
#include "SFML/Graphics/Rect.hpp"
#include "util/util.h"

namespace ui {
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

bool HBox::addWidget(Widget* child) {
    bool found = std::any_of(m_widgets.begin(), m_widgets.end(), [child](const auto& w) {
        return w.get() == child;
    });
    if (found) {
        return false;
    }

    m_widgets.emplace_back(child);
    setParent(child, this);
    invalidateRect();
    return true;
}

bool HBox::removeWidget(Widget* child) {
    auto it = std::find_if(m_widgets.begin(), m_widgets.end(), [child](const auto& w) {
        return w.get() == child;
    });
    if (it == m_widgets.end()) {
        return false;
    }

    (void)it->release();
    setParent(child, this);
    invalidateRect();
    return true;
}

Widget* HBox::getWidget(size_t i) { return i >= m_widgets.size() ? nullptr : m_widgets[i].get(); }

void HBox::setPosition(sf::Vector2f p) {
    if (m_shape.getPosition() == p) {
        return;
    }
    m_shape.setPosition(p);
    invalidateRect();
}

void HBox::setPadding(float padding) {
    if (padding == m_padding) {
        return;
    }
    m_padding = padding;
    invalidateRect();
}

void HBox::setSeperation(float seperation) {
    if (seperation == m_seperation) {
        return;
    }
    m_seperation = seperation;
    invalidateRect();
}

void HBox::invalidateRect() {
    sf::Vector2f size(0, 0);
    sf::Vector2f pos = m_shape.getPosition() + sf::Vector2f(m_padding, m_padding);
    for (auto& child : m_widgets) {
        sf::FloatRect rect = child->getBoundingBox();
        setParent(child.get(), nullptr);
        child->setPosition(pos);

        pos.x += rect.width + m_seperation;

        size.x += rect.width + m_seperation;
        size.y = util::max(size.y, rect.height);
        setParent(child.get(), this);
    }
    size.x = size.x - m_seperation + 2 * m_padding;
    size.y = size.y + 2 * m_padding;

    if (m_shape.getSize() != size) {
        m_shape.setSize(size);
        Widget::invalidateRect();
    }
}
} // namespace ui
