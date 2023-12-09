#pragma once

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/System/Vector2.hpp"
#include <set>
#include <string>

class Circuit;

class Pin {
public:
    enum PinType { Input, Output };

    Pin(const std::string& id, PinType type, sf::Vector2f pos = {0, 0}, int state = 0);

    void onEvent(const sf::RenderWindow&, const sf::Event& event);
    void update(const sf::RenderWindow&);
    void draw(sf::RenderWindow&) const;


    bool canConnect(const Pin& other) const;
    bool collide(sf::Vector2f) const;

    sf::Transformable& getTransform();

    bool connect(bool* obs);
    bool disconnect(bool* obs);

    std::string getFullPath() const;

    inline void setParent(Circuit* c) { _parent = c; }

    inline void setEditable(bool val) { _editable = val; }
    inline void setType(PinType val) { _type = val; }
    inline void setId(const std::string& id) { _id = id; }
    void setValue(bool val);
    void setCenter(sf::Vector2f pos);

    inline bool getValue() const { return _value; }
    inline PinType getType() const { return _type; }
    inline const std::string& getId() const { return _id; }
    sf::Vector2f getCenter() const;

    static constexpr float RADIUS = 10.f;

private:
    void changed();

    std::string _id;

    bool _value = 0;
    bool _editable = false;
    PinType _type;

    std::set<bool*> _dirtyFlags;

    Circuit* _parent = nullptr;

    sf::CircleShape _graphic;
};
