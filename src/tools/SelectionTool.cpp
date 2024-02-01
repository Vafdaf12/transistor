#include "SelectionTool.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "spdlog/spdlog.h"

SelectionTool::SelectionTool(CircuitEditor& editor, DragBoard& board)
    : m_editor(editor), m_board(board) {
    m_selector.setFillColor({66, 135, 245, 100});
}

void SelectionTool::onEvent(const sf::RenderWindow& window, const sf::Event& event) {

    // Start a new selection
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        m_board.clearSelection();

        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, m_editor.getWorldView());

        m_selector.setPosition(worldPos);
        m_selector.setSize({0, 0});
        m_active = true;
        spdlog::debug("set to true");
    }

    if (m_active) {
        if (event.type == sf::Event::MouseMoved) {
            m_board.setSelection(m_editor.collideCircuit(m_selector.getGlobalBounds()));
        } else if (event.type == sf::Event::MouseButtonReleased) {
            spdlog::debug("set to false");
            m_active = false;
        }
    } else {
        Tool::onEvent(window, event);
    }
}

void SelectionTool::update(const sf::RenderWindow& window, float dt) {
    if (m_active) {
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(pos, m_editor.getWorldView());
        sf::Vector2f size = worldPos - m_selector.getPosition();
        m_selector.setSize(size);
    } else {
        Tool::update(window, dt);
    }
}

void SelectionTool::draw(sf::RenderWindow& window) const {
    if (m_active) {
        window.draw(m_selector);
            spdlog::debug("draw");
    }
    for (const Circuit* c : m_board.getSelection()) {
        sf::RectangleShape outline;
        sf::FloatRect rect = c->getBoundingBox();
        outline.setSize(rect.getSize());
        outline.setPosition(rect.getPosition());
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineColor({66, 135, 245, 150});
        outline.setOutlineThickness(5);
        window.draw(outline);
    }
    Tool::draw(window);
}
