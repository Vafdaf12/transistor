#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

#include <iostream>

int main(int, char**) {
    std::cout << "Hello, World!" << std::endl;
    sf::RenderWindow window({1280, 720}, "Transistor");

    while(window.isOpen()) {
        for(sf::Event event; window.pollEvent(event);) {
            if(event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        window.display();
    }
    return 0;
}
