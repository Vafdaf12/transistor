#pragma once

#include "Widget.h"
#include <type_traits>

namespace ui {
class WidgetDecorator : public Widget {
public:
    WidgetDecorator(Widget* inner) : m_inner(inner) {}

    virtual bool addWidget(Widget* child) override { return m_inner->addWidget(child); }
    virtual bool removeWidget(Widget* child) override { return m_inner->removeWidget(child); }
    virtual Widget* getWidget(size_t i) override { return m_inner->getWidget(i); }

    virtual bool onEvent(const sf::RenderWindow& w, const sf::Event& e) override {
        return m_inner->onEvent(w, e);
    }

    virtual void update(const sf::RenderWindow& w, float dt) override { m_inner->update(w, dt); }
    virtual void draw(sf::RenderWindow& w) const override { m_inner->draw(w); }

    virtual sf::FloatRect getBoundingBox() const override { return m_inner->getBoundingBox(); }
    virtual sf::Vector2f getPosition() const override { return m_inner->getPosition(); }
    virtual void setPosition(sf::Vector2f p) override { return m_inner->setPosition(p); }

    // Retrieves the inner widget decorated by this widget
    template <typename T>
        requires std::is_base_of_v<T, Widget>
    T* get() {
        return static_cast<T*>(m_inner.get());
    }

    // Releases the inner widget from this widget
    template <typename T>
        requires std::is_base_of_v<T, Widget>
    T* releaseInner() {
        return static_cast<T*>(m_inner.release());
    }

protected:
    std::unique_ptr<Widget> m_inner;
};
} // namespace ui
