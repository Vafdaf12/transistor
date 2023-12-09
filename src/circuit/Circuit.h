#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "pin/Pin.h"

#include <string>

class Circuit {
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


    /**
     * @brief Retrieves the transformable components of the circuit.
     * This is used to move the circuit around, e.g. drag and drop
     * 
     * @return std::vector<sf::Transformable*> 
     */
    virtual std::vector<sf::Transformable*> getTransforms() = 0;

    virtual sf::FloatRect getBoundingBox() const = 0;

    /**
    * @brief Clones the circuit with a new identifier
    * 
    * @param id New circuit identifier
    * @return Circuit*
    */
    virtual Circuit* clone(const std::string& id) = 0;

    virtual Pin* queryPin(const std::string& id) = 0;

    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual void update(const sf::RenderWindow&) {}

private:
    std::string _id;
};