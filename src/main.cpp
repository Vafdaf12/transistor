#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
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
        sf::Vector2f pinPos = pos + sf::Vector2f(WIDTH-RADIUS, SEP+(2*RADIUS+SEP)*i);
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

enum ToolState {
    NONE = 0,
    PANNING,
    SELECTING,
    CONNECTING,
    DRAGGING,
};


int main(int, char**) {
    std::cout << "Hello, World!" << std::endl;
    sf::RenderWindow window({1280, 720}, "Transistor");

    std::set<std::pair<const sf::CircleShape*, const sf::CircleShape*>> edges;
    std::vector<Circuit*> circuits;

    sf::Vector2i mouse = sf::Mouse::getPosition();
    EventEmitter emitter;
    ToolState state = NONE;

    // --- GRAPHICS COLLECTIONS ---
    std::vector<sf::CircleShape*> pins;
    std::vector<sf::RectangleShape*> bodies;
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
    registerCircuit(circuit, bodies, pins);
    circuits.push_back(&circuit);

    Circuit circuit2;
    circuit2.body.setFillColor(sf::Color::Red);
    initCircuit(circuit2, 2, {-200, 400});
    registerCircuit(circuit2, bodies, pins);
    circuits.push_back(&circuit2);

    // --- VIEW UPDATES ---
    sf::View view;
    view.setCenter({0, 0});
    view.setSize({1280, 720});

    emitter.subscribe(sf::Event::Resized, [&](const sf::Event& event) {
        float x = event.size.width;
        float y = event.size.height;
        view.setSize({x, y});
    });
    emitter.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if(event.mouseButton.button != sf::Mouse::Left) return;
        if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) return;
        state = PANNING;
    });
    emitter.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        if(state != PANNING) return;
        state = NONE;
    });
    emitter.subscribe(sf::Event::MouseWheelScrolled, [&](const sf::Event& event) {
        float delta = event.mouseWheelScroll.delta;
        std::cout << delta << std::endl;
        view.zoom(1.0 - (delta * 0.1f));
    });

    // --- PIN CONNECTION ---
    sf::CircleShape* tempPin = nullptr;
    emitter.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if(state != NONE) return;
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        tempPin = collidePin(pins, worldPos);
        if (!tempPin) {
            return;
        }
        float r = tempPin->getRadius();
        connectVertices[0].position = tempPin->getPosition() + sf::Vector2f(r, r);
        state = CONNECTING;
    });
    emitter.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        if(state != CONNECTING) return;
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        sf::CircleShape* nextPin = collidePin(pins, worldPos);

        if (!nextPin || nextPin == tempPin) {
            tempPin = nullptr;
            state = NONE;
            return;
        }
        if (tempPin < nextPin) {
            edges.insert({tempPin, nextPin});
        } else {
            edges.insert({nextPin, tempPin});
        }
        tempPin = nullptr;
        state = NONE;
    });

    // --- CIRCUIT DRAGGING ---
    ComponentDragger dragger;
    std::vector<Circuit*> selected;

    emitter.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if(state != NONE) return;
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        std::vector<sf::Transformable*> components;
        if(selected.empty()) {
            // Find 
            for(Circuit* c : circuits) {
                if (c->body.getGlobalBounds().contains(worldPos)) {
                    std::vector<sf::Transformable*> children = circuitComponents(*c);
                    std::copy(children.begin(), children.end(), std::back_inserter(components));
                }
            }
        }
        else {
            for(Circuit* c : selected) {
                std::vector<sf::Transformable*> children = circuitComponents(*c);
                std::copy(children.begin(), children.end(), std::back_inserter(components));
            }
        }
        if(components.empty()) {
            return;
        }

        dragger.beginDrag(components, worldPos);
        state = DRAGGING;
    });
    emitter.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        if(state != DRAGGING) return;
        dragger.endDrag();

        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
        for(Circuit* c : selected) {
            if (c->body.getGlobalBounds().contains(worldPos)) {
                return;
            }
        }
        selected.clear();
        state = NONE;
    });
    // --- CIRCUIT SELECTION ---
    sf::RectangleShape selector;
    selector.setSize({100, 100});
    selector.setFillColor({66, 135, 245, 100});

    emitter.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        std::cout << "Selecting" << state << std::endl;
        if(state != NONE) return;
        if (event.mouseButton.button != sf::Mouse::Left) {
            return;
        }

        selected.clear();

        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        selector.setPosition(worldPos);
        selector.setSize({0, 0});
        state = SELECTING;
        std::cout << "Selecting" << std::endl;
    });
    emitter.subscribe(sf::Event::MouseMoved, [&](const sf::Event& event) {
        if(state != SELECTING) return;
        selected.clear();
        std::copy_if(circuits.begin(), circuits.end(), std::back_inserter(selected), [&](Circuit* c) {
            sf::Vector2f tl = c->body.getPosition();
            sf::Vector2f br = tl + c->body.getSize();
            sf::FloatRect selectRect = selector.getGlobalBounds();
            if(!selectRect.contains(tl)) return false;
            if(!selectRect.contains(br)) return false;
            return true;
        });

    });

    emitter.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        if(state != SELECTING) return;
        state = NONE;
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

        switch(state) {

        case PANNING: {
            sf::Vector2f delta = window.mapPixelToCoords(mouse) - newWorldPos;
            view.move(delta);
            break;

        }
        case SELECTING: {
            sf::Vector2f pos = newWorldPos - selector.getPosition();
            selector.setSize(pos);
            break;

        }
        case CONNECTING: {
            connectVertices[1] = window.mapPixelToCoords(mouse);

        }
        default: break;
        }


        dragger.update(newWorldPos);
        mouse = newPos;

        window.setView(view);
        window.clear();

        for(const auto& c : bodies) {
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

        if (state == CONNECTING) {
            window.draw(connectVertices, 2, sf::Lines);
        }
        if (state == SELECTING) {
            window.draw(selector);
        }
        for(const Circuit* c : selected) {
            sf::RectangleShape outline = c->body;
            outline.setFillColor(sf::Color::Transparent);
            outline.setOutlineColor({66, 135, 245, 150});
            outline.setOutlineThickness(5);
            window.draw(outline);
        }
        window.display();
    }
    return 0;
}
