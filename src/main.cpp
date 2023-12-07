#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Font.hpp"
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
#include "SFML/Window/Clipboard.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include "ComponentDragger.h"
#include "EventLayer.h"
#include "circuit/Circuit.h"
#include "circuit/NandCircuit.h"
#include "circuit/PassthroughCircuit.h"
#include "circuit/BinaryGate.h"
#include "game/GameWorld.h"
#include "io/CommandLoader.h"
#include "pin/Pin.h"


#include "SFML/Window/WindowBase.hpp"

#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "tools/PanTool.h"
#include "tools/Tool.h"

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


enum ToolState {
    NONE = 0,
    SELECTING,
    CONNECTING,
    DRAGGING,
};

int main(int, char**) {
    std::cout << sf::Clipboard::getString().toAnsiString() << std::endl;
    sf::RenderWindow window({1280, 720}, "Transistor");
    window.setKeyRepeatEnabled(false);

    sf::Font font;
    font.loadFromFile("assets/fonts/CutiveMono-Regular.ttf");

    sf::Texture xorTexture;
    sf::Texture orTexture;
    sf::Texture andTexture;
    orTexture.loadFromFile("assets/sprites/gate_or.png");
    xorTexture.loadFromFile("assets/sprites/gate_xor.png");
    andTexture.loadFromFile("assets/sprites/gate_and.png");

    GameWorld world;
    CommandLoader loader;

    size_t inputCount = 0;
    size_t outputCount = 0;
    loader.registerCommand("pin", [&](const std::string& params) {
        std::stringstream stream(params);
        char t;
        float x, y;
        stream >> t >> x >> y;
        switch (t) {
            case 'I': {
                world.addPin(new Pin("in" + std::to_string(++inputCount), Pin::Input, {x, y}));
                break;
            }
            case 'O': {
                world.addPin(new Pin("out" + std::to_string(++outputCount), Pin::Output, {x, y}));
                break;
            }
            default: break;
        }
    });

    int circuitCount = 0;
    loader.registerCommand("circuit", [&](const std::string& params) {
        std::stringstream stream(params);
        char t;
        float x, y;
        stream >> t >> x >> y;
        std::string id = std::to_string(++circuitCount); 
        switch(t) {
            case 'P': {
                int n, r, g, b;
                stream >> n >> r >> g >> b;
                PassthroughCircuit* c;
                c = new PassthroughCircuit("pt" + id,n, {x, y});
                c->setColor(sf::Color(r, g, b, 255));
                world.addCircuit(c);

                break;
            }
            case 'N': {
                world.addCircuit(new NandCircuit("nand" + id, font, {x, y}));
                break;
            }
            case 'X': {
                world.addCircuit(new BinaryGate("xor" + id, xorTexture, BinaryGate::Xor, {x, y}));
                break;
            }
            case 'O': {
                world.addCircuit(new BinaryGate("or" + id, orTexture, BinaryGate::Or, {x, y}));
                break;
            }
            case 'A': {
                world.addCircuit(new BinaryGate("and" + id, andTexture, BinaryGate::And, {x, y}));
                break;
            }
            default: break;
        }
    });
    loader.loadFile("assets/world.txt");
    SfLayer guiLayer;
    SfLayer worldLayer;

    sf::Vector2i mouse = sf::Mouse::getPosition();
    Tool* currentTool = nullptr;
    ToolState state = NONE;

    Circuit* dragBoard = nullptr;
    NandCircuit* proto = new NandCircuit("",font);

    // --- GRAPHICS COLLECTIONS ---
    sf::Vertex connectVertices[2] = {sf::Vertex({0, 0}, sf::Color::White)};

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
        dragBoard = proto;
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
    worldLayer.subscribe(sf::Event::KeyPressed, [&](const sf::Event& event) {
        if(event.key.code != sf::Keyboard::LAlt) {
            return false;
        }
        currentTool = new PanTool(view, window);
        std::cout << "Tool assigned" << std::endl;
        return true;
    });
    worldLayer.subscribe(sf::Event::KeyReleased, [&](const sf::Event& event) {
        if(event.key.code != sf::Keyboard::LAlt) {
            return false;
        }
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
    Pin* tempPin = nullptr;
    worldLayer.subscribe(sf::Event::MouseButtonPressed, [&](const sf::Event& event) {
        if (state != NONE)
            return false;
        if (event.mouseButton.button != sf::Mouse::Left) {
            return false;
        }
        sf::Vector2i mousePos = {event.mouseButton.x, event.mouseButton.y};
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

        tempPin = world.collidePin(worldPos);
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

        Pin* nextPin = world.collidePin(worldPos);

        if (!nextPin || nextPin == tempPin || !tempPin->canConnect(*nextPin)) {
            tempPin = nullptr;
            state = NONE;
            return true;
        }

        world.connectPins(tempPin, nextPin);

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

        clickedPin = world.collidePin(worldPos, GameWorld::SINGLE);
        if (clickedPin && clickedPin->type != Pin::Output) {
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
            Circuit* hovered = world.collideCircuit(worldPos);
            if (hovered) {
                std::vector<sf::Transformable*> children = hovered->getTransforms();
                std::copy(children.begin(), children.end(), std::back_inserter(components));
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
        selected = world.collideCircuit(selector.getGlobalBounds());
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
        Circuit* c = dragBoard->clone("proto" + std::to_string(++circuitCount));

        sf::Vector2f pos = c->getBoundingBox().getPosition() + c->getBoundingBox().getSize() / 2.f;

        dragger.beginDrag(c->getTransforms(), pos);
        world.addCircuit(c);
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
        for (auto s : selected) {
            world.removeCircuit(s);
        }

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
            if(currentTool) {
                currentTool->getEventTarget()->post(e);
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
        window.draw(world);

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
