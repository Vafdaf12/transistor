#pragma once

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/System/Vector2.hpp"
#include <set>

class Pin;

class PinObserver {
public:
    virtual void update(Pin* pin) = 0;
};

class Pin : public sf::Drawable {
public:
    enum PinType {
        Input,
        Output
    };

    Pin(PinType type, sf::Vector2f pos={0, 0}, int state = 0);

    void setState(int s);
    int getState() const;

    sf::Vector2f getCenter() const;
    void setCenter(sf::Vector2f pos);


    bool canConnect(const Pin& other) const;
    bool collide(sf::Vector2f) const;

    sf::Transformable& getTransform();

    void draw(sf::RenderTarget&, sf::RenderStates) const override;

    static constexpr float RADIUS = 10.0f;

    const PinType type;

    bool connect(PinObserver* obs);
    bool disconnect(PinObserver* obs);

private:
    void notify();

    sf::CircleShape _shape;
    int _state = 0;

    std::set<PinObserver*> _observers;
};
