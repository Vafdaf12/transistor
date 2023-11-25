#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"

#include <iostream>
#include <set>
#include <utility>
#include <vector>

#include "ComponentDragger.h"
#include "EventEmitter.h"

sf::CircleShape createPin(sf::Vector2f pos = {0, 0}) {
    sf::CircleShape pin;
    pin.setPosition(pos);
    pin.setFillColor(sf::Color::Black);
    pin.setOutlineColor(sf::Color::White);
    pin.setOutlineThickness(1);
    pin.setRadius(10);
    return pin;
}

sf::CircleShape* collidePin(std::vector<sf::CircleShape*>& pins, sf::Vector2f pos) {
    for (int i = 0; i < pins.size(); i++) {
        if (pins[i]->getGlobalBounds().contains(pos)) {
            return pins[i];
        }
    }
    return nullptr;
}

struct Circuit {
    sf::RectangleShape body;
    std::vector<sf::CircleShape> pins;
};

void initCircuit(Circuit& c, size_t n, sf::Vector2f pos) {
    constexpr float RADIUS = 10.0f;
    constexpr float SEP = 2*RADIUS;
    constexpr float WIDTH = 150;

    c.body.setPosition(pos);
    c.body.setSize({WIDTH, (2*RADIUS+SEP)*n+SEP});

    c.pins.clear();
    for(size_t i = 0; i < n; i++) {
        sf::Vector2f pinPos = pos + sf::Vector2f(WIDTH-RADIUS, pos.y+SEP+(2*RADIUS+SEP)*i);
        sf::CircleShape pin;
        pin.setPosition(pinPos);
        pin.setFillColor(sf::Color::Black);
        pin.setOutlineColor(sf::Color::White);
        pin.setOutlineThickness(1);
        pin.setRadius(RADIUS);
        c.pins.emplace_back(std::move(pin));
    }
}

void registerCircuit(Circuit& c, std::vector<sf::RectangleShape*>& bodies,std::vector<sf::CircleShape*>& pins) {
    bodies.push_back(&c.body);
    for(auto& p : c.pins) {
        pins.push_back(&p);
    }
}

std::vector<sf::Transformable*> circuitComponents(Circuit& c) {
    std::vector<sf::Transformable*> components;
    for(auto& p : c.pins) {
        components.push_back(&p);
    }
    components.push_back(&c.body);
    return components;
}


int main(int, char**) {
    std::cout << "Hello, World!" << std::endl;
    sf::RenderWindow window({1280, 720}, "Transistor");

    std::set<std::pair<const sf::CircleShape*, const sf::CircleShape*>> edges;

    sf::Vector2i mouse = sf::Mouse::getPosition();
    EventEmitter emitter;

    // --- GRAPHICS COLLECTIONS ---
    std::vector<sf::CircleShape*> pins;
    std::vector<sf::RectangleShape*> circuits;
    sf::Vertex edgeVertices[2] = {sf::Vertex({0, 0}, sf::Color::White)};
    sf::Vertex connectVertices[2] = {sf::Vertex({0, 0}, sf::Color::White)};
    
    // --- GRAPHICS ELEMENTS ---
    sf::CircleShape p1 = createPin({0, 0});
    pins.push_back(&p1);

    sf::CircleShape p2 = createPin({100, 50});
    pins.push_back(&p2);

    sf::CircleShape p3 = createPin({150, 50});
    pins.push_back(&p3);


    Circuit circuit;
    circuit.body.setFillColor(sf::Color::Green);
    initCircuit(circuit, 3, {-200, 0});
    registerCircuit(circuit, circuits, pins);

    // --- VIEW UPDATES ---
    sf::View view;
    view.setCenter({0, 0});
    view.setSize({1280, 720});
    bool isPanning = false;

    emitter.subscribe(sf::Event::Resized, [&](const sf::Event& event) {
        float x = event.size.width;
        float y = event.size.height;
        view.setSize({x, y});
    });
    emitter.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if(event.mouseButton.button != sf::Mouse::Left) return;
        if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) return;

        isPanning = true;
    });
    emitter.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        isPanning = false;
    });
    emitter.subscribe(sf::Event::MouseWheelScrolled, [&](const sf::Event& event) {
        float delta = event.mouseWheelScroll.delta;
        std::cout << delta << std::endl;
        view.zoom(1.0 - (delta * 0.1f));
    });

    // --- PIN CONNECTION ---
    sf::CircleShape* tempPin = nullptr;
    emitter.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        tempPin = collidePin(pins, worldPos);
        if (tempPin) {
            float r = tempPin->getRadius();
            connectVertices[0].position = tempPin->getPosition() + sf::Vector2f(r, r);
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
        if (tempPin < nextPin) {
            edges.insert({tempPin, nextPin});
        } else {
            edges.insert({nextPin, tempPin});
        }
        tempPin = nullptr;
    });

    // --- CIRCUIT DRAGGING ---
    ComponentDragger dragger;

    emitter.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        if (!circuit.body.getGlobalBounds().contains(worldPos)) {
            return;
        }

        dragger.beginDrag(circuitComponents(circuit), worldPos);
    });
    emitter.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        dragger.endDrag();
    });

    // --- CIRCUIT SELECTION ---
    sf::RectangleShape selector;
    selector.setSize({100, 100});
    selector.setFillColor({66, 135, 245, 100});
    bool isSelecting = false;

    emitter.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if(dragger.isDragging()) return;
        if(event.mouseButton.button != sf::Mouse::Left) return;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) return;

        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        isSelecting = true;
        selector.setPosition(worldPos);
        selector.setSize({0, 0});

    });

    emitter.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        isSelecting = false;
    });

    // --- EVENT LOOP ---
    while (window.isOpen()) {
        for (sf::Event event; window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            emitter.post(event);
        }
        sf::Vector2i newPos = sf::Mouse::getPosition(window);
        sf::Vector2f newWorldPos = window.mapPixelToCoords(newPos);

        if(isPanning) {
            sf::Vector2f delta = window.mapPixelToCoords(mouse) - newWorldPos;
            view.move(delta);
        }
        if(isSelecting) {
            sf::Vector2f pos = newWorldPos - selector.getPosition();
            selector.setSize(pos);
        }

        dragger.update(newWorldPos);
        mouse = newPos;
        if (tempPin) {
            connectVertices[1] = window.mapPixelToCoords(mouse);
        }

        window.setView(view);
        window.clear();

        for(const auto& c : circuits) {
            window.draw(*c);
        }
        for (const auto& pin : pins) {
            window.draw(*pin);
        }
        for (const auto& [s1, s2] : edges) {
            float r1 = s1->getRadius();
            float r2 = s2->getRadius();

            edgeVertices[0].position = s1->getPosition() + sf::Vector2f(r1, r1);
            edgeVertices[1].position = s2->getPosition() + sf::Vector2f(r2, r2);

            window.draw(edgeVertices, 2, sf::Lines);
        }

        if (tempPin) {
            window.draw(connectVertices, 2, sf::Lines);
        }
        if (isSelecting) {
            window.draw(selector);
        }
        window.display();
    }
    return 0;
}
