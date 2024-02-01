#pragma once

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"

class CircuitView {
public:
    virtual ~CircuitView() = default;

    // Checks if a point is within the circuit's bounds
    virtual bool collide(sf::Vector2f) const = 0;

    // Retrieves a an axis-aligned rectangle containing the circuit
    virtual sf::FloatRect getBoundingBox() const = 0;

    // Draws the view to the specified render window
    virtual void draw(sf::RenderWindow&) const = 0;

    // Sets the position of the view, including any child views
    virtual void setPosition(sf::Vector2f) = 0;

    // Sets the size of the view (if 0, an automatic size is assumed)
    virtual void setSize(sf::Vector2f) {}

    // Retrieves the minimal size of the view, or {0, 0} if it does not have one
    virtual sf::Vector2f getMinimumSize() const { return {0, 0}; }

    // Clones the circuit view 
    virtual CircuitView* clone() const = 0;

    virtual sf::Vector2f getPosition() const = 0;

};