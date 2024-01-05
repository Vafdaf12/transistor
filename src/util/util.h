#pragma once

#include "SFML/Graphics/RenderTarget.hpp"
namespace util {
sf::Vector2f projectToWorldSpace(const sf::RenderTarget&, sf::Vector2i pos, const sf::View* view = nullptr);
sf::Vector2i projectToScreenSpace(const sf::RenderTarget&, sf::Vector2f pos, const sf::View* view = nullptr);

template <typename T>
inline T* value_ptr(T& v) {
    return &v;
}

// C++ doesn't have a built-in max function, so here's my own
template <typename T>
inline T max(T a, T b) {
    return a > b ? a : b;
}
} // namespace util
