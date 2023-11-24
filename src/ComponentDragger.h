#pragma once

#include "SFML/Graphics/Transformable.hpp"

#include <vector>

class ComponentDragger {
public:
    void beginDrag(const std::vector<sf::Transformable*>& objects, sf::Vector2f pos);
    void endDrag();

    void update(sf::Vector2f newPos);
private:
    std::vector<std::pair<sf::Transformable*, sf::Vector2f>> _components;
};