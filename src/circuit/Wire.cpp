#include "Wire.h"
#include "SFML/Graphics/PrimitiveType.hpp"
#include "util/util.h"

Wire::Wire(const Pin* source, Pin* target) : m_source(source), m_target(target) {
    m_target->setValue(m_source->getValue());
}
void Wire::draw(sf::RenderWindow& target) const {
    static sf::Vertex edge[2] = {sf::Vertex({0, 0}, sf::Color::White)};

    edge[0].position = util::projectToWorldSpace(target, m_source->getScreenSpacePosition(target));
    edge[1].position = util::projectToWorldSpace(target, m_target->getScreenSpacePosition(target));

    if (m_source->getValue()) {
        edge[0].color = sf::Color(Pin::COLOR_ACTIVE);
        edge[1].color = sf::Color(Pin::COLOR_ACTIVE);
    } else {
        edge[0].color = sf::Color(Pin::COLOR_INACTIVE);
        edge[1].color = sf::Color(Pin::COLOR_INACTIVE);
    }

    target.draw(edge, 2, sf::Lines);
}

void Wire::update(const sf::RenderWindow&, float) {
    m_target->setValue(m_source->getValue());
}

bool Wire::isEndpoint(const Pin* pin) const { return m_source == pin || m_target == pin; }
bool Wire::operator==(const Wire& rhs) const { return m_source == rhs.m_source && m_target == rhs.m_target; }
