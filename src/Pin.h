#pragma once

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/System/Vector2.hpp"

class Pin : public sf::Drawable {
public:
    enum PinType {
        Input,
        Output
    };

    Pin(PinType type, sf::Vector2f pos, int state = 0);

    void setState(int s);
    int getState() const;

    sf::Vector2f getCenter() const;
    void setCenter(sf::Vector2f pos);

    bool canConnect(const Pin& other) const;
    bool collide(sf::Vector2f) const;

    sf::Transformable& getTransform();

    void draw(sf::RenderTarget&, sf::RenderStates) const override;

private:
    sf::CircleShape _shape;
    PinType _type;
    int _state = 0;
};