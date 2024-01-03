#include "BinaryGate.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"

BinaryGate::BinaryGate(const std::string& id, const sf::Texture& texture, Func fn, sf::Vector2f pos)
    : Circuit(id), m_in1("in1", Pin::Input), m_in2("in2", Pin::Input),
      m_out("out", Pin::Output, {0, 0}, 0), m_process(fn), m_sprite(texture) {
    m_in1.setParent(this);
    m_in2.setParent(this);
    m_out.setParent(this);

    m_sprite.scale(0.7, 0.7);

    setPosition(pos);
}
BinaryGate::BinaryGate(const BinaryGate& other)
    : BinaryGate(
          other.getId(), *other.m_sprite.getTexture(), other.m_process, other.getPosition()
      ) {}

bool BinaryGate::collide(sf::Vector2f v) const { return m_sprite.getGlobalBounds().contains(v); }

sf::Vector2f BinaryGate::getPosition() const { return m_sprite.getPosition(); }
void BinaryGate::setPosition(sf::Vector2f pos) {
    m_sprite.setPosition(pos);
    sf::Vector2f size = m_sprite.getGlobalBounds().getSize();

    m_out.setPosition(pos + sf::Vector2f(size.x, size.y / 2.f));

    size.y /= 4;
    m_in1.setPosition(pos + sf::Vector2f(0, size.y * 1.f));
    m_in2.setPosition(pos + sf::Vector2f(0, size.y * 3.f));
}

sf::FloatRect BinaryGate::getBoundingBox() const { return m_sprite.getGlobalBounds(); }

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
    window.draw(m_sprite);
    m_in1.draw(window);
    m_in2.draw(window);
    m_out.draw(window);
}

int BinaryGate::And(int x, int y) { return x && y; }
int BinaryGate::Or(int x, int y) { return x || y; }
int BinaryGate::Nand(int x, int y) { return !And(x, y); }
int BinaryGate::Nor(int x, int y) { return !Or(x, y); }
int BinaryGate::Xor(int x, int y) { return x ^ y; }

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
        {"x", m_sprite.getPosition().x},
        {"y", m_sprite.getPosition().y},
    };
}
