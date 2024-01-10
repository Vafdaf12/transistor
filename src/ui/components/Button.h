#pragma once

#include "SFML/Graphics/RenderWindow.hpp"

#include "ui/WidgetDecorator.h"

#include <functional>

namespace ui {

class Button : public WidgetDecorator {
public:
    using Handler = std::function<void()>;

    Button(Widget* w);
    Button(Widget* w, Handler&& h);

    void update(const sf::RenderWindow&, float) override;
    bool onEvent(const sf::RenderWindow&, const sf::Event&) override;



    inline void setOnClick(Handler&& h) { m_onClick = h; }
    inline bool isActive() const { return m_state & (Hover | Dragging); }

private:
    State m_state = 0;
    Handler m_onClick;
};
} // namespace ui
