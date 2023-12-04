#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
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
#include <vector>

#include "Circuit.h"
#include "ComponentDragger.h"
#include "EventLayer.h"
#include "pin/Pin.h"
#include "pin/Wire.h"

#include "SFML/Window/WindowBase.hpp"

using SfLayer = EventLayer<sf::Event::EventType, sf::Event>;

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

class Tool : public sf::Drawable {
public:
    virtual void update() = 0;

protected:
    virtual void draw(sf::RenderTarget&, sf::RenderStates) const override {}
};

class PanTool : public Tool {
public:
    PanTool(sf::View& v, const sf::RenderWindow& w) : _view(v), _window(w) {
        _previousPosition = sf::Mouse::getPosition(_window);
    }
    void update() override {
        sf::Vector2i pos = sf::Mouse::getPosition(_window);

        sf::Vector2f prevWorldPos = _window.mapPixelToCoords(_previousPosition);
        sf::Vector2f worldPos = _window.mapPixelToCoords(pos);

        _view.move(prevWorldPos - worldPos);

        _previousPosition = pos;
    }

private:
    sf::View& _view;
    const sf::RenderWindow& _window;

    sf::Vector2i _previousPosition;
};

enum ToolState {
    NONE = 0,
    SELECTING,
    CONNECTING,
    DRAGGING,
};
const Pin* collidePin(const std::vector<Circuit*>& circuits, sf::Vector2f pos) {
    for (Circuit* c : circuits) {
        if (const Pin* p = c->collidePin(pos)) {
            return p;
        }
    }
    return nullptr;
}
Pin* collidePin(const std::vector<Pin*>& circuits, sf::Vector2f pos) {
    for (Pin* p : circuits) {
        if (p->collide(pos)) {
            return p;
        }
    }
    return nullptr;
}

int main(int, char**) {
    std::cout << "Hello, World!" << std::endl;
    sf::RenderWindow window({1280, 720}, "Transistor");

    std::vector<Wire> wires;
    std::vector<Circuit*> circuits;

    SfLayer guiLayer;
    SfLayer worldLayer;

    sf::Vector2i mouse = sf::Mouse::getPosition();
    Tool* currentTool = nullptr;
    ToolState state = NONE;

    Circuit* dragBoard = nullptr;
    Circuit prototype(2, 1, {0, 0});
    prototype.setColor(sf::Color::Cyan);

    // --- GRAPHICS COLLECTIONS ---
    std::vector<Pin*> pins;
    sf::Vertex connectVertices[2] = {sf::Vertex({0, 0}, sf::Color::White)};

    // --- GRAPHICS ELEMENTS ---
    Pin p1(Pin::Input, {0, 0});
    pins.push_back(&p1);

    Pin p2(Pin::Output, {100, 50});
    pins.push_back(&p2);

    Pin p3(Pin::Output, {150, 50});
    pins.push_back(&p3);

    Circuit circuit(2, 3, {-200, 0});
    circuit.setColor(sf::Color::Green);
    circuits.push_back(&circuit);

    Circuit circuit2(2, 1, {-200, 400});
    circuit2.setColor(sf::Color::Red);
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
        currentTool = new PanTool(view, window);
        return true;
    });
    worldLayer.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        if (!currentTool)
            return false;
        delete currentTool;
        currentTool = nullptr;
        return true;
    });
    worldLayer.subscribe(sf::Event::MouseWheelScrolled, [&](const sf::Event& event) {
        float delta = event.mouseWheelScroll.delta;
        std::cout << delta << std::endl;
        view.zoom(1.0 - (delta * 0.1f));
        return false;
    });

    // --- PIN CONNECTION ---
    const Pin* tempPin = nullptr;
    worldLayer.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if (state != NONE)
            return false;
        if (event.mouseButton.button != sf::Mouse::Left) {
            return false;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        tempPin = collidePin(circuits, worldPos);
        if (!tempPin) {
            tempPin = collidePin(pins, worldPos);
        }
        if (!tempPin) {
            return false;
        }
        connectVertices[0].position = tempPin->getCenter();
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

        const Pin* nextPin = collidePin(circuits, worldPos);
        if (!nextPin) {
            nextPin = collidePin(pins, worldPos);
        }

        if (!nextPin || nextPin == tempPin || !tempPin->canConnect(*nextPin)) {
            tempPin = nullptr;
            state = NONE;
            return true;
        }


        wires.emplace_back(tempPin, nextPin);

        tempPin = nullptr;
        state = NONE;
        return true;
    });

    // --- PIN TOGGLING ---
    Pin* clickedPin = nullptr;
    worldLayer.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if (event.mouseButton.button != sf::Mouse::Left) {
            return false;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        clickedPin = collidePin(pins, worldPos);
        if(clickedPin && clickedPin->type != Pin::Output) {
            clickedPin = nullptr;
        }
        return false;
    });
    worldLayer.subscribe(sf::Event::MouseMoved, [&](const sf::Event& event) {
        if (!clickedPin) {
            return false;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
        if (clickedPin->collide(worldPos)) {
            return false;
        }
        clickedPin = nullptr;
        return true;
    });
    worldLayer.subscribe(sf::Event::MouseButtonReleased, [&](const sf::Event& event) {
        if (event.mouseButton.button != sf::Mouse::Left) {
            return false;
        }
        if (!clickedPin) {
            return false;
        }
        clickedPin->setState(!clickedPin->getState());
        clickedPin = nullptr;

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
                if (c->collide(worldPos)) {
                    std::vector<sf::Transformable*> children = c->getTransforms();
                    std::copy(children.begin(), children.end(), std::back_inserter(components));
                }
            }
        } else {
            for (Circuit* c : selected) {
                std::vector<sf::Transformable*> children = c->getTransforms();
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
            if (c->collide(worldPos)) {
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
                sf::Vector2f tl = c->getBoundingBox().getPosition();
                sf::Vector2f br = tl + c->getBoundingBox().getSize();
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

        sf::Vector2f pos = c->getBoundingBox().getPosition() + c->getBoundingBox().getSize() / 2.f;

        dragger.beginDrag(c->getTransforms(), pos);
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
        if (currentTool) {
            currentTool->update();
        }

        switch (state) {

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
        for (const auto& c : circuits) {
            window.draw(*c);
        }
        for (const auto& pin : pins) {
            window.draw(*pin);
        }
        for (const auto& wire : wires) {

            window.draw(wire);
        }

        if (state == CONNECTING) {
            window.draw(connectVertices, 2, sf::Lines);
        }
        if (state == SELECTING) {
            window.draw(selector);
        }
        for (const Circuit* c : selected) {
            sf::RectangleShape outline;
            sf::FloatRect rect = c->getBoundingBox();
            outline.setSize(rect.getSize());
            outline.setPosition(rect.getPosition());
            outline.setFillColor(sf::Color::Transparent);
            outline.setOutlineColor({66, 135, 245, 150});
            outline.setOutlineThickness(5);
            window.draw(outline);
        }
        if (currentTool) {
            window.draw(*currentTool);
        }

        // --- GUI VIEW ---
        window.setView(window.getDefaultView());
        window.draw(buttonShape);
        window.display();
    }
    return 0;
}
