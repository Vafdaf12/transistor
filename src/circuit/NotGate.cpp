#include "NotGate.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

NotGate::NotGate(const std::string& id, const Assets& assets, sf::Vector2f pos)
    : Circuit(id), m_input("in", Pin::Input), m_output("out", Pin::Output),
      m_assets(assets) {
    m_sprite.setTexture(assets.textures.get("gate_not"));

    m_input.setParent(this);
    m_output.setParent(this);

    m_sprite.scale(0.7, 0.7);
    setPosition(pos);
}
bool NotGate::collide(sf::Vector2f v) const { return m_sprite.getGlobalBounds().contains(v); }

sf::Vector2f NotGate::getPosition() const {
    return m_sprite.getPosition();
}
void NotGate::setPosition(sf::Vector2f pos) {
    m_sprite.setPosition(pos);
    sf::Vector2f size = m_sprite.getGlobalBounds().getSize();

    m_output.setPosition(pos + sf::Vector2f(size.x, size.y / 2.f));
    m_input.setPosition(pos + sf::Vector2f(0, size.y / 2.f));
}

sf::FloatRect NotGate::getBoundingBox() const { return m_sprite.getGlobalBounds(); }

NotGate* NotGate::clone(const std::string& newId) const {
    NotGate* c = new NotGate(newId, m_assets, m_sprite.getPosition());
    return c;
}

void NotGate::update(const sf::RenderWindow& w, float dt) {
    m_output.setValue(!m_input.getValue());
    m_input.update(w, dt);
    m_output.update(w, dt);
}

void NotGate::draw(sf::RenderWindow& window) const {
    window.draw(m_sprite);
    m_input.draw(window);
    m_output.draw(window);
}

void NotGate::toJson(nlohmann::json& j) const {
    j["id"] = getId();
    j["type"] = "not_gate";
    j["position"] = {
        {"x", m_sprite.getPosition().x },
        {"y", m_sprite.getPosition().y },
    };
}
