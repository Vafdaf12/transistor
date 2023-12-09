#include "PanTool.h"
#include <malloc.h>

PanTool::PanTool(sf::View& v) : _view(v) {
}
void PanTool::onEvent(const sf::RenderWindow& window, const sf::Event& event) {
    if(event.type == sf::Event::KeyPressed) {
        if (event.key.code != sf::Keyboard::LAlt) {
            return;
        }
        _active = true;
    }
    if(event.type == sf::Event::KeyReleased) {
        if (event.key.code != sf::Keyboard::LAlt) {
            return;
        }
        _active = false;
    }
    if(event.type == sf::Event::MouseButtonPressed) {
        if(event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        _previousPosition = {event.mouseButton.x, event.mouseButton.y};
        _isPanning = true;
    }
    if(event.type == sf::Event::MouseButtonReleased) {
        if(event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        _isPanning = false;
    }
}

void PanTool::update(const sf::RenderWindow& window) {
    if(!_isPanning) {
        return;
    }
    sf::Vector2i pos = sf::Mouse::getPosition(window);

    sf::Vector2f prevWorldPos = window.mapPixelToCoords(_previousPosition);
    sf::Vector2f worldPos = window.mapPixelToCoords(pos);

    _view.move(prevWorldPos - worldPos);

    _previousPosition = pos;
    
}
