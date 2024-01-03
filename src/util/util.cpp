#include "util.h"

namespace util {

sf::Vector2f
projectToWorldSpace(const sf::RenderTarget& t, sf::Vector2i pos, const sf::View* view) {
    return view ? t.mapPixelToCoords(pos, *view) : t.mapPixelToCoords(pos);
}

sf::Vector2i
projectToScreenSpace(const sf::RenderTarget& t, sf::Vector2f pos, const sf::View* view) {
    return view ? t.mapCoordsToPixel(pos, *view) : t.mapCoordsToPixel(pos);
}
} // namespace util
