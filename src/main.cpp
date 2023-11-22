#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/Window/Event.hpp"

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

    while(window.isOpen()) {
        for(sf::Event event; window.pollEvent(event);) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        window.draw(shape);
        window.display();
    }
    return 0;
}
