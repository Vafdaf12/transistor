#include "ImageView.h"
namespace ui {
bool ImageView::onEvent(const sf::RenderWindow&, const sf::Event&) {
    return _state & Hover;
}
void ImageView::update(const sf::RenderWindow& w, float) {
    sf::Vector2f pos = w.mapPixelToCoords(sf::Mouse::getPosition(w));
    if(_sprite.getGlobalBounds().contains(pos)) {
        _state |= Hover;
    } else {
        _state &= !Hover;
    }
}
}
