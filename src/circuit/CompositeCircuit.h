#pragma once

#include <list>

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"

#include "./Circuit.h"
#include "./Pin.h"
#include "./Wire.h"


class CompositeCircuit : public Circuit {
public:
    CompositeCircuit(const std::string& id, const sf::Font& font, sf::Vector2f pos = {0, 0});
    CompositeCircuit(const CompositeCircuit&);

    bool collide(sf::Vector2f) const override;

    std::vector<Pin*> getAllPins() override;

    sf::FloatRect getBoundingBox() const override;
    sf::Vector2f getPosition() const override;
    void setPosition(sf::Vector2f) override;

    Circuit* clone(const std::string& id) const override;

    void onEvent(const sf::RenderWindow&, const sf::Event&) override {}

    void toJson(nlohmann::json& j) const override;

    void update(const sf::RenderWindow&, float dt) override;
    void draw(sf::RenderWindow&) const override;

    void setLabel(const std::string& label);

private:
    Pin* queryPin(const std::string& path);
    Circuit* queryCircuit(const std::string& id);

    static constexpr float PADDING = 10.f;

    sf::RectangleShape m_shape;
    sf::Text m_label;

    std::list<Wire> m_wires;
    std::list<Pin> m_inputs;
    std::list<Pin> m_outputs;
    std::list<std::unique_ptr<Circuit>> m_circuits;
};
