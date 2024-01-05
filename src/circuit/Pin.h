#pragma once

#include <set>
#include <string>

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/System/Vector2.hpp"

#include "core/Entity.h"

class Circuit;

class Pin : public core::Entity<std::string> {
public:
    static constexpr float RADIUS = 10.f;
    static constexpr int HOVER_MOD = 70;
    static constexpr uint32_t COLOR_ACTIVE = 0xff0000ff;
    static constexpr uint32_t COLOR_INACTIVE = 0x808080ff;
    static constexpr uint32_t COLOR_OUTLINE = 0xffffffff;

    enum PinType { Input, Output };

    Pin(const std::string& id, PinType type, sf::Vector2f pos = {0, 0}, int state = 0);

    void onEvent(const sf::RenderWindow&, const sf::Event& event) override;
    void update(const sf::RenderWindow&, float dt) override;
    void draw(sf::RenderWindow&) const override;

    bool canConnect(const Pin& other) const;
    bool collide(sf::Vector2f) const;
    bool collide(const sf::RenderTarget& target, sf::Vector2i) const;

    sf::Transformable& getTransform();

    std::string getFullPath() const;

    inline void setParent(Circuit* c) { _parent = c; }

    inline void setEditable(bool val) { _editable = val; }
    inline void setType(PinType val) { _type = val; }
    inline void setView(const sf::View* view) { _view = view; }
    void setValue(bool val);
    void setPosition(sf::Vector2f pos);

    inline bool getValue() const { return _value; }
    inline PinType getType() const { return _type; }
    sf::Vector2f getPosition() const;

    sf::Vector2i getScreenSpacePosition(const sf::RenderTarget&) const;

    // Checks if two pin types can be connected to each other
    static bool isCompatible(PinType a, PinType b);

private:
    // --- Data model attributes ---
    bool _value = 0;
    bool _editable = false;
    PinType _type;
    Circuit* _parent = nullptr;

    // --- SFML-related attributes ---
    sf::CircleShape _graphic;
    const sf::View* _view = nullptr;
};
