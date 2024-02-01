#pragma once

#include "SFML/Graphics/RenderWindow.hpp"

#include <string>

#include "Pin.h"
#include "circuit/Circuit.h"

class NotGate : public Circuit {
public:
    NotGate(const std::string& id, CircuitView* view);
    NotGate(const NotGate& other);

    virtual void setPosition(sf::Vector2f) override;

    void draw(sf::RenderWindow& window) const override;
    void update(const sf::RenderWindow&, float dt) override;

    NotGate* clone(const std::string& id) const override;

    void toJson(nlohmann::json& j) const override;
    inline std::vector<Pin*> getAllPins() override { return {&m_input, &m_output}; }

private:
    void layout();
    Pin m_input, m_output;
};
