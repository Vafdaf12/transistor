#pragma once

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/System/Vector2.hpp"
#include <set>
#include <string>

class Pin;
class Circuit;

class PinObserver {
public:
    virtual void update(Pin* pin) = 0;
    virtual void onRemove(Pin* pin) {}
};

class Pin2 {
public:
    enum PinType { Input, Output };

    Pin2(const std::string& id, PinType type);



    sf::Vector2f getCenter() const;
    void setCenter(sf::Vector2f);

private:
    std::string _id;
    PinType _type;

    bool _value = 0;
    bool _editable = false;

    sf::CircleShape _graphic;
};

class Pin {
public:
    enum PinType { Input, Output };

    Pin(const std::string& id, PinType type, sf::Vector2f pos = {0, 0}, int state = 0);
    ~Pin();

    void onEvent(const sf::RenderWindow&, const sf::Event& event);
    void update(const sf::RenderWindow&);
    void draw(sf::RenderWindow&) const;


    bool canConnect(const Pin& other) const;
    bool collide(sf::Vector2f) const;

    sf::Transformable& getTransform();

    bool connect(PinObserver* obs);
    bool disconnect(PinObserver* obs);

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
    void notify();


    bool _value = 0;
    bool _editable = false;
    PinType _type;

    std::set<PinObserver*> _observers;
    std::string _id;

    Circuit* _parent = nullptr;

    sf::CircleShape _graphic;
};
