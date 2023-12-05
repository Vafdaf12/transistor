#pragma once

#include "SFML/Graphics/Drawable.hpp"
#include "pin/Pin.h"

class Circuit : public sf::Drawable, public PinObserver {
public:
    virtual ~Circuit() {}

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

    virtual Circuit* clone() = 0;
};