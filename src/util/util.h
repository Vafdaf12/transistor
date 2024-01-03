#pragma once

#include "SFML/Graphics/RenderTarget.hpp"
namespace util {
    sf::Vector2f projectToWorldSpace(const sf::RenderTarget&, sf::Vector2i pos, const sf::View* view = nullptr);
    sf::Vector2i projectToScreenSpace(const sf::RenderTarget&, sf::Vector2f pos, const sf::View* view = nullptr);
}
