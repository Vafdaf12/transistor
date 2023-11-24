#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"

#include <iostream>
#include <vector>

#include "EventEmitter.h"
#include "ComponentDragger.h"

sf::CircleShape createPin(sf::Vector2f pos = {0, 0}) {
    sf::CircleShape pin;
    pin.setPosition(pos);
    pin.setFillColor(sf::Color::Black);
    pin.setOutlineColor(sf::Color::White);
    pin.setOutlineThickness(1);
    pin.setRadius(10);
    return pin;
}

sf::CircleShape* collidePin(std::vector<sf::CircleShape>& pins, sf::Vector2f pos) {
    for (int i = 0; i < pins.size(); i++) {
        if (pins[i].getGlobalBounds().contains(pos)) {
            return &pins[i];
        }
    }
    return nullptr;
}

int main(int, char**) {
    std::cout << "Hello, World!" << std::endl;
    sf::RenderWindow window({1280, 720}, "Transistor");
    window.setFramerateLimit(70);

    sf::View view;
    view.setCenter({0, 0});
    view.setSize({1280, 720});
    window.setView(view);

    std::vector<sf::CircleShape> pins;
    std::vector<std::pair<sf::CircleShape*, sf::CircleShape*>> edges;

    pins.emplace_back(createPin({0, 0}));
    pins.emplace_back(createPin({100, 50}));
    pins.emplace_back(createPin({150, 50}));

    sf::Vector2i mouse = sf::Mouse::getPosition();

    sf::Vertex vertex[2];
    vertex[0].color = sf::Color::White;
    vertex[1].color = sf::Color::White;

    sf::Vertex edgeVertex[2];
    edgeVertex[0].color = sf::Color::White;
    edgeVertex[1].color = sf::Color::White;

    sf::CircleShape* tempPin = nullptr;

    EventEmitter emitter;
    emitter.subscribe(sf::Event::Resized, [&](const sf::Event& event) {
        float x = event.size.width;
        float y = event.size.height;
        view.setSize({x, y});
    });
    emitter.subscribe(sf::Event::MouseWheelScrolled, [&](const sf::Event& event) {
        float delta = event.mouseWheelScroll.delta;
        std::cout << delta << std::endl;
        view.zoom(1.0 - (delta * 0.1f));
    });
    emitter.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        tempPin = collidePin(pins, worldPos);
        if (tempPin) {
            float r = tempPin->getRadius();
            vertex[0].position = tempPin->getPosition() + sf::Vector2f(r, r);
        }
    });
    emitter.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
        if (!tempPin) {
            return;
        }

        sf::CircleShape* nextPin = collidePin(pins, worldPos);

        if (!nextPin || nextPin == tempPin) {
            tempPin = nullptr;
            return;
        }
        edges.emplace_back(tempPin, nextPin);
        tempPin = nullptr;
    });

    // --- CIRCUIT DRAGGING ---
    sf::RectangleShape circuit;
    int cpin1 = pins.size();
    int cpin2 = pins.size()+1;

    pins.emplace_back(createPin({-60, 15}));
    pins.emplace_back(createPin({-60, 65}));

    circuit.setFillColor(sf::Color::Green);
    circuit.setSize({150, 100});
    circuit.setPosition({-200, 0});

    ComponentDragger dragger;


    emitter.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        if(!circuit.getGlobalBounds().contains(worldPos)) {
            return;
        }

        dragger.beginDrag({&circuit, &pins[cpin1], &pins[cpin2]}, worldPos);
    });
    emitter.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        dragger.endDrag();
    });

    while (window.isOpen()) {
        for (sf::Event event; window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            emitter.post(event);
        }
        sf::Vector2i newPos = sf::Mouse::getPosition(window);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {

            sf::Vector2f delta = window.mapPixelToCoords(mouse) - window.mapPixelToCoords(newPos);
            view.move(delta);
        }
        {
            sf::Vector2f worldPos = window.mapPixelToCoords(newPos);
            dragger.update(worldPos);
        }
        mouse = newPos;
        if (tempPin) {
            vertex[1] = window.mapPixelToCoords(mouse);
        }

        window.setView(view);
        window.clear();

        window.draw(circuit);
        for (const auto& pin : pins) {
            window.draw(pin);
        }
        for (const auto& [s1, s2] : edges) {
            float r1 = s1->getRadius();
            float r2 = s2->getRadius();

            edgeVertex[0].position = s1->getPosition() + sf::Vector2f(r1, r1);
            edgeVertex[1].position = s2->getPosition() + sf::Vector2f(r2, r2);

            window.draw(edgeVertex, 2, sf::Lines);
        }

        if (tempPin) {
            window.draw(vertex, 2, sf::Lines);
        }
        window.display();
    }
    return 0;
}
