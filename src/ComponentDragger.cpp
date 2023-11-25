#include "ComponentDragger.h"
#include "SFML/Graphics/Transformable.hpp"
#include <algorithm>
#include <iterator>
#include <utility>

void ComponentDragger::beginDrag(const std::vector<sf::Transformable*>& objects, sf::Vector2f pos) {
    _components.clear();
    std::transform(
        objects.begin(),
        objects.end(),
        std::back_inserter(_components),
        [&pos](sf::Transformable* t) {
            sf::Vector2f delta = t->getPosition() - pos;
            return std::make_pair(t, delta);
        }
    );
}
void ComponentDragger::endDrag() { _components.clear(); }
void ComponentDragger::update(sf::Vector2f newPos) {
    for(auto& [t, delta] : _components) {
        t->setPosition(newPos + delta);
    }
}

bool ComponentDragger::isDragging() const {
    return !_components.empty();
    
}
