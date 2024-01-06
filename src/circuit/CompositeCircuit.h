#pragma once

#include <list>

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"

#include "./Circuit.h"
#include "./Pin.h"
#include "./Wire.h"


class CompositeCircuit : public Circuit {
public:
    CompositeCircuit(const std::string& id, const std::string& type, const sf::Font& font, sf::Vector2f pos = {0, 0});
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

    // -------------------------------------------------- CUSTOM METHODS

    // Sets the label of the circuit, adjusting the background to fit the text
    void setLabel(const std::string& label);

    // Configures the number of input pins (IDs are auto-generated). Removes any connected wires if count decreases
    void setInputCount(size_t count);

    // Configures the number of output pins (IDs are auto-generated). Removes any connected wires if count decreases
    void setOutputCount(size_t count);

    // Attempts to connect the inner pins with the passed-in IDS
    bool connectPins(const std::string& source, const std::string& target);

    // Attempts to add a sub-circuit to the circuit
    bool addCircuit(Circuit* c);

private:
    // Re-computes pin layout and corresponding circuit size
    void layout();

    // Checks if the pin is not one of the inputs/outputs
    bool isInteriorPin(const Pin& pin) const;

    Pin* queryPin(const std::string& path);
    Circuit* queryCircuit(const std::string& id);

    static constexpr float PADDING = 10.f;

    sf::RectangleShape m_shape;
    sf::Text m_label;

    std::vector<Wire> m_wires;
    std::list<Pin> m_inputs;
    std::list<Pin> m_outputs;
    std::list<std::unique_ptr<Circuit>> m_circuits;

    // Required for serialization purposes
    // TODO: find a way to not have to specify this (memento?)
    std::string m_type;
};
