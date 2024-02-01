#include "NotGate.h"

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"

NotGate::NotGate(const std::string& id, CircuitView* view)
    : Circuit(id, view), m_input("in0", Pin::Input), m_output("out0", Pin::Output) {

    m_input.setParent(this);
    m_output.setParent(this);
    layout();
}
NotGate::NotGate(const NotGate& other) : NotGate(other.getId(), other.m_view->clone()) {}

void NotGate::setPosition(sf::Vector2f pos) {
    m_view->setPosition(pos);
    layout();
}

NotGate* NotGate::clone(const std::string& newId) const {
    NotGate* c = new NotGate(*this);
    c->setId(newId);
    return c;
}

void NotGate::update(const sf::RenderWindow& w, float dt) {
    m_output.setValue(!m_input.getValue());
    m_input.update(w, dt);
    m_output.update(w, dt);
}

void NotGate::draw(sf::RenderWindow& window) const {
    m_view->draw(window);
    m_input.draw(window);
    m_output.draw(window);
}

void NotGate::toJson(nlohmann::json& j) const {
    j["id"] = getId();
    j["type"] = "not_gate";
    j["position"] = {
        {"x", m_view->getPosition().x},
        {"y", m_view->getPosition().y},
    };
}

void NotGate::layout() {
    sf::FloatRect rect = m_view->getBoundingBox();

    m_output.setPosition(rect.getPosition() + sf::Vector2f(rect.width, rect.height / 2.f));
    m_input.setPosition(rect.getPosition() + sf::Vector2f(0, rect.height / 2.f));
}
