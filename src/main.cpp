#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"

#include <iostream>

int main(int, char**) {
    std::cout << "Hello, World!" << std::endl;
    sf::RenderWindow window({1280, 720}, "Transistor");

    sf::View view;
    view.setCenter({0, 0});
    view.setSize({1280, 720});
    window.setView(view);

    sf::RectangleShape shape;
    shape.setPosition({0, 0});
    shape.setSize({100, 100});
    shape.setFillColor(sf::Color::Yellow);

    sf::Vector2i mouse = sf::Mouse::getPosition();

    while (window.isOpen()) {
        for (sf::Event event; window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::Resized) {
                float x = event.size.width;
                float y = event.size.height;
                view.setSize({x, y});
            }
            if (event.type == sf::Event::MouseWheelScrolled) {
                float delta = event.mouseWheelScroll.delta;
                std::cout << delta << std::endl;
                view.zoom(1.0 - (delta * 0.1f));
            }
        }
        sf::Vector2i newPos = sf::Mouse::getPosition(window);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {

            sf::Vector2f delta = window.mapPixelToCoords(mouse) - window.mapPixelToCoords(newPos);
            view.move(delta);
        }
        mouse = newPos;

        window.setView(view);
        window.clear();
        window.draw(shape);
        window.display();
    }
    return 0;
}
