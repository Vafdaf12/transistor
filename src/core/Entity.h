#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

namespace core {
template<typename Id>
class Entity {
public:
    inline Entity(const Id& id) : _id(id) {}
    virtual ~Entity() {}

    inline const Id& getId() const { return _id; }
    inline void setId(const Id& id) { _id = id; }

    virtual void onEvent(const sf::RenderWindow&, const sf::Event&) = 0;
    virtual void update(const sf::RenderWindow&, float dt) = 0;
    virtual void draw(sf::RenderWindow&) const = 0;

private:
    Id _id;
};
} // namespace core
