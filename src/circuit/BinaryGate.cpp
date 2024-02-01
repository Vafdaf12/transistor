#include "BinaryGate.h"

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"

BinaryGate::BinaryGate(const std::string& id, Func fn, CircuitView* view)
    : Circuit(id, view), m_in1("in0", Pin::Input), m_in2("in1", Pin::Input),
      m_out("out0", Pin::Output, {0, 0}, 0), m_process(fn) {
    m_in1.setParent(this);
    m_in2.setParent(this);
    m_out.setParent(this);
    layout();
}
BinaryGate::BinaryGate(const BinaryGate& other)
    : BinaryGate(other.getId(), other.m_process, other.m_view->clone()) {}


void BinaryGate::setPosition(sf::Vector2f pos) {
    m_view->setPosition(pos);
    layout();
}

BinaryGate* BinaryGate::clone(const std::string& newId) const {
    BinaryGate* c = new BinaryGate(*this);
    c->setId(newId);
    return c;
}

void BinaryGate::update(const sf::RenderWindow& w, float dt) {
    m_out.setValue(m_process(m_in1.getValue(), m_in2.getValue()));

    m_out.update(w, dt);
    m_in1.update(w, dt);
    m_in2.update(w, dt);
}

void BinaryGate::draw(sf::RenderWindow& window) const {
    m_view->draw(window);
    m_in1.draw(window);
    m_in2.draw(window);
    m_out.draw(window);
}

int BinaryGate::And(int x, int y) { return x && y; }
int BinaryGate::Or(int x, int y) { return x || y; }
int BinaryGate::Nand(int x, int y) { return !And(x, y); }
int BinaryGate::Nor(int x, int y) { return !Or(x, y); }
int BinaryGate::Xor(int x, int y) { return x ^ y; }

void BinaryGate::layout() {
    sf::FloatRect rect = m_view->getBoundingBox();
    
    sf::Vector2f pos = rect.getPosition();
    sf::Vector2f size = rect.getSize();

    m_out.setPosition(pos + sf::Vector2f(size.x, size.y / 2.f));

    size.y /= 4;
    m_in1.setPosition(pos + sf::Vector2f(0, size.y * 1.f));
    m_in2.setPosition(pos + sf::Vector2f(0, size.y * 3.f));
}

void BinaryGate::toJson(nlohmann::json& j) const {
    const static std::unordered_map<Func, std::string> mapping = {
        {And, "and_gate"},
        {Or, "or_gate"},
        {Nand, "nand_gate"},
        {Nor, "nor_gate"},
        {Xor, "xor_gate"},
    };

    j["id"] = getId();
    j["type"] = mapping.at(m_process);
    j["position"] = {
        {"x", m_view->getPosition().x},
        {"y", m_view->getPosition().y},
    };
}
