#include "NavigationTool.h"

NavigationTool::NavigationTool(sf::View& v) : _view(v) {}

void NavigationTool::onEvent(const sf::RenderWindow& window, const sf::Event& e) {
    if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::LAlt) {
        _state = Panning;
    }
    if (e.type == sf::Event::KeyReleased && e.key.code == sf::Keyboard::LAlt) {
        if (_state == Panning) {
            _state = None;
        }
    }
    if(e.type == sf::Event::MouseWheelScrolled) {
        float delta = e.mouseWheelScroll.delta;
        _view.zoom(1.0 - (delta * 0.1f));
    }
    if(e.type == sf::Event::Resized) {
        float zoom = _view.getSize().y / e.size.height;
        float x = e.size.width;
        float y = e.size.height;
        _view.setSize({x, y});
        _view.zoom(zoom);
    }
}

void NavigationTool::update(const sf::RenderWindow& window, float) {

    sf::Vector2i pos = sf::Mouse::getPosition(window);

    if(_state == Panning && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2f prevWorldPos = window.mapPixelToCoords(_prevMousePosition, _view);
        sf::Vector2f worldPos = window.mapPixelToCoords(pos, _view);

        _view.move(prevWorldPos - worldPos);
    }
    _prevMousePosition = pos;
}
