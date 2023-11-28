#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/Graphics/Vertex.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <set>
#include <utility>
#include <vector>

#include "ComponentDragger.h"
#include "EventLayer.h"

sf::CircleShape createPin(sf::Vector2f pos = {0, 0}) {
    sf::CircleShape pin;
    pin.setPosition(pos);
    pin.setFillColor(sf::Color::Black);
    pin.setOutlineColor(sf::Color::White);
    pin.setOutlineThickness(1);
    pin.setRadius(10);
    return pin;
}

using SfLayer = EventLayer<sf::Event::EventType, sf::Event>;

struct Pin {
    enum PinType { INPUT, OUTPUT, ANY };
    sf::CircleShape body;
    PinType type = ANY;
};

struct Circuit {
    sf::RectangleShape body;
    sf::Text text;

    std::vector<Pin> inputs;
    std::vector<Pin> outputs;
};

struct Button {
    sf::FloatRect bounds;
    std::function<void(const sf::Event&)> onMouseDown;
    std::function<void(const sf::Event&)> onMouseMove;
    std::function<void(const sf::Event&)> onMouseUp;
};

void buttonRegister(const Button& b, SfLayer& emitter, const sf::RenderTarget& target) {
    emitter.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        sf::Vector2i pos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = target.mapPixelToCoords(pos);

        if (!b.bounds.contains(worldPos)) {
            return false;
        }
        if (b.onMouseDown) {
            b.onMouseDown(event);
        }
        return true;
    });
    emitter.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        sf::Vector2i pos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = target.mapPixelToCoords(pos);

        if (!b.bounds.contains(worldPos)) {
            return false;
        }
        if (b.onMouseUp) {
            b.onMouseUp(event);
        }
        return true;
    });
    emitter.subscribe(sf::Event::MouseMoved, [&](const sf::Event& event) {
        sf::Vector2i pos = {event.mouseMove.x, event.mouseMove.y};
        sf::Vector2f worldPos = target.mapPixelToCoords(pos);

        if (!b.bounds.contains(worldPos)) {
            return false;
        }
        if (b.onMouseMove) {
            b.onMouseMove(event);
        }
        return true;
    });
}

constexpr float RADIUS = 10.0f;
constexpr float SEP = 2 * RADIUS;
constexpr float WIDTH = 150;

Pin* collidePin(const std::vector<Pin*>& pins, sf::Vector2f pos) {
    for (int i = 0; i < pins.size(); i++) {
        if (pins[i]->body.getGlobalBounds().contains(pos)) {
            return pins[i];
        }
    }
    return nullptr;
}

bool isCompatible(const Pin& p1, const Pin& p2) {
    if (p1.type == Pin::ANY || p2.type == Pin::ANY)
        return true;
    return p1.type != p2.type;
}

void initPin(Pin& p, sf::Vector2f pos, Pin::PinType type = Pin::ANY) {
    p.type = type;
    p.body.setPosition(pos);
    p.body.setFillColor(sf::Color::Black);
    switch (type) {
    case Pin::INPUT: p.body.setOutlineColor(sf::Color::Blue); break;
    case Pin::OUTPUT: p.body.setOutlineColor(sf::Color::Magenta); break;
    case Pin::ANY: p.body.setOutlineColor(sf::Color::White); break;
    }
    p.body.setOutlineThickness(1);
    p.body.setRadius(RADIUS);
}

void initCircuit(Circuit& c, sf::Vector2f pos, size_t inputs, size_t outputs) {
    size_t maxPins = std::max(inputs, outputs);

    c.body.setPosition(pos);
    c.body.setSize({WIDTH, (2 * RADIUS + SEP) * maxPins + SEP});

    c.inputs.clear();
    float inputStart = (c.body.getSize().y - (2 * RADIUS + SEP) * inputs) / 2.f + RADIUS;
    for (size_t i = 0; i < inputs; i++) {
        sf::Vector2f pinPos = pos + sf::Vector2f(-RADIUS, inputStart + (2 * RADIUS + SEP) * i);
        Pin pin;
        initPin(pin, pinPos, Pin::INPUT);
        c.inputs.emplace_back(std::move(pin));
    }

    c.outputs.clear();
    float outputStart = (c.body.getSize().y - (2 * RADIUS + SEP) * outputs) / 2.f + RADIUS;
    for (size_t i = 0; i < outputs; i++) {
        sf::Vector2f pinPos =
            pos + sf::Vector2f(WIDTH - RADIUS, outputStart + (2 * RADIUS + SEP) * i);
        Pin pin;
        initPin(pin, pinPos, Pin::OUTPUT);
        c.outputs.emplace_back(std::move(pin));
    }
}

void registerCircuit(
    Circuit& c, std::vector<sf::RectangleShape*>& bodies, std::vector<Pin*>& pins
) {
    bodies.push_back(&c.body);
    for (auto& p : c.inputs) {
        pins.push_back(&p);
    }
    for (auto& p : c.outputs) {
        pins.push_back(&p);
    }
}

std::vector<sf::Transformable*> circuitComponents(Circuit& c) {
    std::vector<sf::Transformable*> components;
    for (auto& p : c.inputs) {
        components.push_back(&p.body);
    }
    for (auto& p : c.outputs) {
        components.push_back(&p.body);
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

    std::set<std::pair<const Pin*, const Pin*>> edges;
    std::vector<Circuit*> circuits;

    SfLayer guiLayer;
    SfLayer worldLayer;

    sf::Vector2i mouse = sf::Mouse::getPosition();
    ToolState state = NONE;

    Circuit* dragBoard = nullptr;
    Circuit prototype;
    initCircuit(prototype, {0, 0}, 2, 1);
    prototype.body.setFillColor(sf::Color::Cyan);

    // --- GRAPHICS COLLECTIONS ---
    std::vector<Pin*> pins;
    std::vector<sf::RectangleShape*> bodies;
    sf::Vertex edgeVertices[2] = {sf::Vertex({0, 0}, sf::Color::White)};
    sf::Vertex connectVertices[2] = {sf::Vertex({0, 0}, sf::Color::White)};

    // --- GRAPHICS ELEMENTS ---
    Pin p1;
    initPin(p1, {0, 0}, Pin::INPUT);
    pins.push_back(&p1);

    Pin p2;
    initPin(p2, {100, 50}, Pin::OUTPUT);
    pins.push_back(&p2);

    Pin p3;
    initPin(p3, {150, 50});
    pins.push_back(&p3);

    Circuit circuit;
    circuit.body.setFillColor(sf::Color::Green);
    initCircuit(circuit, {-200, 0}, 2, 3);
    registerCircuit(circuit, bodies, pins);
    circuits.push_back(&circuit);

    Circuit circuit2;
    circuit2.body.setFillColor(sf::Color::Red);
    initCircuit(circuit2, {-200, 400}, 2, 1);
    registerCircuit(circuit2, bodies, pins);
    circuits.push_back(&circuit2);

    // --- GUI UPDATES ---
    sf::RectangleShape buttonShape;

    buttonShape.setSize({200, 75});
    buttonShape.setPosition({10, 10});
    buttonShape.setFillColor(sf::Color::White);

    Button button = {buttonShape.getGlobalBounds()};
    button.onMouseDown = [&](const sf::Event& e) {
        if (e.mouseButton.button != sf::Mouse::Left)
            return;
        std::cout << "Pressed" << std::endl;
        dragBoard = &prototype;
    };
    button.onMouseUp = [&](const sf::Event& e) {
        if (e.mouseButton.button != sf::Mouse::Left)
            return;
        std::cout << "Released" << std::endl;
        dragBoard = nullptr;
    };

    buttonRegister(button, guiLayer, window);

    // --- VIEW UPDATES ---
    sf::View view;
    view.setCenter({0, 0});
    view.setSize({1280, 720});

    worldLayer.subscribe(sf::Event::Resized, [&](const sf::Event& event) {
        float x = event.size.width;
        float y = event.size.height;
        view.setSize({x, y});
        return false;
    });
    worldLayer.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if (event.mouseButton.button != sf::Mouse::Left)
            return false;
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
            return false;
        state = PANNING;
        return true;
    });
    worldLayer.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        if (state != PANNING)
            return false;
        state = NONE;
        return true;
    });
    worldLayer.subscribe(sf::Event::MouseWheelScrolled, [&](const sf::Event& event) {
        float delta = event.mouseWheelScroll.delta;
        std::cout << delta << std::endl;
        view.zoom(1.0 - (delta * 0.1f));
        return false;
    });

    // --- PIN CONNECTION ---
    Pin* tempPin = nullptr;
    worldLayer.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if (state != NONE)
            return false;
        if (event.mouseButton.button != sf::Mouse::Left) {
            return false;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        tempPin = collidePin(pins, worldPos);
        if (!tempPin) {
            return false;
        }
        float r = tempPin->body.getRadius();
        connectVertices[0].position = tempPin->body.getPosition() + sf::Vector2f(r, r);
        state = CONNECTING;
        return true;
    });
    worldLayer.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        if (state != CONNECTING)
            return false;
        if (event.mouseButton.button != sf::Mouse::Left) {
            return false;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        Pin* nextPin = collidePin(pins, worldPos);

        if (!nextPin || nextPin == tempPin) {
            tempPin = nullptr;
            state = NONE;
            return true;
        }
        if (!isCompatible(*tempPin, *nextPin)) {
            tempPin = nullptr;
            state = NONE;
            return true;
        }

        if (tempPin < nextPin) {
            edges.insert({tempPin, nextPin});
        } else {
            edges.insert({nextPin, tempPin});
        }
        tempPin = nullptr;
        state = NONE;
        return true;
    });

    // --- CIRCUIT DRAGGING ---
    ComponentDragger dragger;
    std::vector<Circuit*> selected;

    worldLayer.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if (state != NONE)
            return false;
        if (event.mouseButton.button != sf::Mouse::Left) {
            return false;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        std::vector<sf::Transformable*> components;
        if (selected.empty()) {
            // Find
            for (Circuit* c : circuits) {
                if (c->body.getGlobalBounds().contains(worldPos)) {
                    std::vector<sf::Transformable*> children = circuitComponents(*c);
                    std::copy(children.begin(), children.end(), std::back_inserter(components));
                }
            }
        } else {
            for (Circuit* c : selected) {
                std::vector<sf::Transformable*> children = circuitComponents(*c);
                std::copy(children.begin(), children.end(), std::back_inserter(components));
            }
        }
        if (components.empty()) {
            return false;
        }

        dragger.beginDrag(components, worldPos);
        state = DRAGGING;
        return true;
    });
    worldLayer.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        if (state != DRAGGING)
            return false;
        dragger.endDrag();

        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
        for (Circuit* c : selected) {
            if (c->body.getGlobalBounds().contains(worldPos)) {
                return false;
            }
        }
        selected.clear();
        state = NONE;
        return true;
    });
    // --- CIRCUIT SELECTION ---
    sf::RectangleShape selector;
    selector.setSize({100, 100});
    selector.setFillColor({66, 135, 245, 100});

    worldLayer.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if (state != NONE)
            return false;
        if (event.mouseButton.button != sf::Mouse::Left) {
            return false;
        }

        selected.clear();

        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        selector.setPosition(worldPos);
        selector.setSize({0, 0});
        state = SELECTING;
        return true;
    });
    worldLayer.subscribe(sf::Event::MouseMoved, [&](const sf::Event& event) {
        if (state != SELECTING)
            return false;
        selected.clear();
        std::copy_if(
            circuits.begin(),
            circuits.end(),
            std::back_inserter(selected),
            [&](Circuit* c) {
                sf::Vector2f tl = c->body.getPosition();
                sf::Vector2f br = tl + c->body.getSize();
                sf::FloatRect selectRect = selector.getGlobalBounds();
                if (!selectRect.contains(tl))
                    return false;
                if (!selectRect.contains(br))
                    return false;
                return true;
            }
        );
        return true;
    });

    worldLayer.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        if (state != SELECTING)
            return false;
        state = NONE;
        return true;
    });

    // --- DRAG DROP ---
    worldLayer.subscribe(sf::Event::MouseMoved, [&](const sf::Event& event) {
        if (!dragBoard)
            return false;
        Circuit* c = new Circuit(*dragBoard);

        registerCircuit(*c, bodies, pins);
        dragger.beginDrag(circuitComponents(*c), c->body.getPosition() + c->body.getSize() / 2.f);
        circuits.push_back(c);
        state = DRAGGING;
        dragBoard = nullptr;
        return true;
    });

    // --- CIRCUIT DELETION ---
    worldLayer.subscribe(sf::Event::KeyReleased, [&](const sf::Event& event) {
        if (event.key.code != sf::Keyboard::Delete)
            return false;
        if (selected.empty()) {
            std::cout << "Nothing to delete" << std::endl;
            return false;
        }

        // Remove bodies
        std::erase_if(bodies, [&](const sf::RectangleShape* r) {
            for (auto s : selected) {
                if (r == &s->body)
                    return true;
            }
            return false;
        });

        // Remove pins
        std::erase_if(pins, [&](const Pin* pin) {
            for (auto s : selected) {
                for (const auto& p : s->inputs) {
                    if (&p == pin)
                        return true;
                }
                for (const auto& p : s->outputs) {
                    if (&p == pin)
                        return true;
                }
            }
            return false;
        });

        // Remove Circuit
        std::erase_if(circuits, [&](const Circuit* c) {
            for (auto s : selected) {
                if (c == s)
                    return true;
            }
            return false;
        });

        selected.clear();
        return true;
    });

    // --- EVENT LOOP ---
    while (window.isOpen()) {
        std::list<sf::Event> events;
        for (sf::Event e; window.pollEvent(e);)
            events.push_back(e);
        for (sf::Event e : events) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }
        }

        // GUI Events
        for (sf::Event e : events) {
            window.setView(window.getDefaultView());
            if (guiLayer.handle(e.type, e))
                continue;

            window.setView(view);
            if (worldLayer.handle(e.type, e))
                continue;
        }

        window.setView(view);

        sf::Vector2i newPos = sf::Mouse::getPosition(window);
        sf::Vector2f newWorldPos = window.mapPixelToCoords(newPos);

        switch (state) {

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

        window.clear();

        // --- WORLD VIEW ---
        window.setView(view);
        for (const auto& c : bodies) {
            window.draw(*c);
        }
        for (const auto& pin : pins) {
            window.draw(pin->body);
        }
        for (const auto& [s1, s2] : edges) {
            float r1 = s1->body.getRadius();
            float r2 = s2->body.getRadius();

            edgeVertices[0].position = s1->body.getPosition() + sf::Vector2f(r1, r1);
            edgeVertices[1].position = s2->body.getPosition() + sf::Vector2f(r2, r2);

            window.draw(edgeVertices, 2, sf::Lines);
        }

        if (state == CONNECTING) {
            window.draw(connectVertices, 2, sf::Lines);
        }
        if (state == SELECTING) {
            window.draw(selector);
        }
        for (const Circuit* c : selected) {
            sf::RectangleShape outline = c->body;
            outline.setFillColor(sf::Color::Transparent);
            outline.setOutlineColor({66, 135, 245, 150});
            outline.setOutlineThickness(5);
            window.draw(outline);
        }

        // --- GUI VIEW ---
        window.setView(window.getDefaultView());
        window.draw(buttonShape);
        window.display();
    }
    return 0;
}
