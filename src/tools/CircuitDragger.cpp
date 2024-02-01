#include "CircuitDragger.h"
#include "circuit/Circuit.h"

#include <algorithm>
#include <iterator>

CircuitDragger::CircuitDragger(CircuitEditor& editor, DragBoard& board)
    : m_editor(editor), m_board(board) {}

bool CircuitDragger::isActive() const { return !m_circuits.empty(); }

void CircuitDragger::onEvent(const sf::RenderWindow& window, const sf::Event& e) {
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = {e.mouseButton.x, e.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, m_editor.getWorldView());

        // Find a colliding circuit in the selection
        std::vector<Circuit*> selection = m_board.getSelection();

        bool canDrag = std::any_of(
            selection.begin(),
            selection.end(),
            std::bind(&Circuit::collide, std::placeholders::_1, worldPos)
        );

        if (!canDrag) {
            if (Circuit* c = m_editor.collideCircuit(worldPos)) {
                selection = {c};
            } else {
                selection.clear();
            }
        }

        if(selection.size() == 1) {
            m_board.clearSelection();
        }

        if (!selection.empty()) {
            startDragging(window, selection);
        } else {
            m_circuits.clear();
        }
    }
    if (e.type == sf::Event::MouseButtonReleased && e.mouseButton.button == sf::Mouse::Left) {
        m_circuits.clear();
    }
    if (!isActive()) {
        Tool::onEvent(window, e);
    }
}

void CircuitDragger::update(const sf::RenderWindow& window, float dt) {
    sf::Vector2i pos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pos, m_editor.getWorldView());

    for (auto& [delta, c] : m_circuits) {
        c->setPosition(worldPos + delta);
    }
    if (!isActive()) {
        Tool::update(window, dt);
    }
}

void CircuitDragger::startDragging(
    const sf::RenderWindow& window, const std::vector<Circuit*> selection
) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, m_editor.getWorldView());

    m_circuits.clear();
    std::transform(
        selection.begin(),
        selection.end(),
        std::back_inserter(m_circuits),
        [&](Circuit* c) { return std::make_pair(c->getPosition() - worldPos, c); }
    );
}
