#pragma once

#include "core/Entity.h"
#include "json.hpp"
#include "pin/Pin.h"

#include <string>

class Circuit : public core::Entity {
public:
    Circuit(const std::string& id) : _id(id) {}
    virtual ~Circuit() {}

    inline const std::string& getId() const { return _id; }
    inline void setId(const std::string& id) { _id = id; }

    /**
     * @brief Checks if a point is within the circuit's bounds
     * 
     * @return true The point is within the circuit's bounds
     * @return false  The point is outside the circuit's bounds
     */
    virtual bool collide(sf::Vector2f) const = 0;

    /**
     * @brief Retrieves the pin (if any) containing the passed in point
     * 
     * @return Pin* 
     */
    virtual Pin* collidePin(sf::Vector2f) = 0;


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

    virtual Pin* queryPin(const std::string& id) = 0;

    virtual void setView(const sf::View& view) = 0;

    void onEvent(const sf::RenderWindow&, const sf::Event&) override {}

    virtual void toJson(nlohmann::json& j) const = 0;
private:
    std::string _id;
};
