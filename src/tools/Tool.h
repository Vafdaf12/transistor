#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

class Tool {
public:
    virtual ~Tool() = default;

    virtual void update(const sf::RenderWindow& w, float dt) {
        if (m_next) {
            m_next->update(w, dt);
        }
    }
    virtual void onEvent(const sf::RenderWindow& w, const sf::Event& e) {
        if (m_next) {
            m_next->onEvent(w, e);
        }
    }
    virtual void draw(sf::RenderWindow& w) const {
        if (m_next) {
            m_next->draw(w);
        }
    }

    void append(Tool* tool) {
        if (m_next) {
            m_next->append(tool);
        } else {
            m_next = tool;
        }
    }

protected:
    Tool* m_next = nullptr;
};
