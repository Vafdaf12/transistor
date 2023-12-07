#include "PanTool.h"

PanTool::PanTool(sf::View& v, const sf::RenderWindow& w) : _view(v), _window(w) {
    _target.bind(sf::Event::MouseButtonPressed, [this](const sf::Event& event) {
        if(event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        _previousPosition = {event.mouseButton.x, event.mouseButton.y};
        _isPanning = true;
    });
    _target.bind(sf::Event::MouseButtonReleased, [this](const sf::Event& event) {
        if(event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        _isPanning = false;
    });
}
void PanTool::update() {
    if(!isActive()) {
        return;
    }
    sf::Vector2i pos = sf::Mouse::getPosition(_window);

    sf::Vector2f prevWorldPos = _window.mapPixelToCoords(_previousPosition);
    sf::Vector2f worldPos = _window.mapPixelToCoords(pos);

    _view.move(prevWorldPos - worldPos);

    _previousPosition = pos;
}