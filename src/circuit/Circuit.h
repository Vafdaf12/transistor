#pragma once

#include "Pin.h"
#include "core/Entity.h"
#include "json.hpp"

#include <string>

class Circuit : public core::Entity<std::string> {
public:
    using Entity::Entity;

    /**
     * @brief Checks if a point is within the circuit's bounds
     *
     * @return true The point is within the circuit's bounds
     * @return false  The point is outside the circuit's bounds
     */
    virtual bool collide(sf::Vector2f) const = 0;

    /**
     * @return std::vector<Pin*> all pins in the circuit (excluding any sub-circuits)
     */
    virtual std::vector<Pin*> getAllPins() = 0;
    std::vector<const Pin*> getAllPins() const { return getAllPins(); }

    virtual sf::FloatRect getBoundingBox() const = 0;
    virtual sf::Vector2f getPosition() const = 0;
    virtual void setPosition(sf::Vector2f) = 0;

    /**
     * @brief Clones the circuit with a new identifier
     *
     * @param id New circuit identifier
     * @return Circuit*
     */
    virtual Circuit* clone(const std::string& id) const = 0;

    void onEvent(const sf::RenderWindow&, const sf::Event&) override {}

    virtual void toJson(nlohmann::json& j) const = 0;
};
