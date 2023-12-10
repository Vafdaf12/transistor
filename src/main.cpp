#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Clipboard.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/WindowBase.hpp"

#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "EventLayer.h"
#include "asset/AssetSystem.h"
#include "asset/CommandLoader.h"
#include "circuit/Circuit.h"
#include "circuit/NandCircuit.h"
#include "game/GameWorld.h"
#include "pin/Pin.h"
#include "tools/CircuitDragger.h"
#include "tools/NavigationTool.h"
#include "tools/PinConnector.h"
#include "tools/SelectionTool.h"
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
    DRAGGING,
};

int main(int, char**) {
    std::cout << sf::Clipboard::getString().toAnsiString() << std::endl;
    sf::RenderWindow window({1280, 720}, "Transistor");
    window.setKeyRepeatEnabled(false);

    Assets assets;
    assets.fonts.load("default", "assets/fonts/CutiveMono-Regular.ttf");
    assets.textures.load("gate_or", "assets/sprites/gate_or.png");
    assets.textures.load("gate_xor", "assets/sprites/gate_xor.png");
    assets.textures.load("gate_and", "assets/sprites/gate_and.png");

    GameWorld world;
    world.loadFromFile("assets/world.json", assets);
    CommandLoader loader;

    int circuitCount = 0;
    SfLayer guiLayer;
    SfLayer worldLayer;

    sf::View view;
    view.setCenter({0, 0});
    view.setSize({1280, 720});

    sf::Vector2i mouse = sf::Mouse::getPosition();

    ToolState state = NONE;

    std::list<Tool*> tools;
    tools.push_back(new NavigationTool(view));
    tools.push_back(new PinConnector(world));

    CircuitDragger* dragger = new CircuitDragger(world);
    tools.push_back(dragger);

    SelectionTool* selector = new SelectionTool(world);
    tools.push_back(selector);

    Circuit* dragBoard = nullptr;
    NandCircuit* proto = new NandCircuit("", assets);

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

    // --- CIRCUIT SELECTION ---

    std::vector<Circuit*> selected;
    auto onSelect = [&](const std::vector<Circuit*>& circuits) {
        dragger->setSelection(circuits);
        selected = circuits;
    };
    selector->setOnSelect(onSelect);

    // --- DRAG DROP ---
    worldLayer.subscribe(sf::Event::MouseMoved, [&](const sf::Event& event) {
        if (!dragBoard)
            return false;
        Circuit* c = dragBoard->clone("proto" + std::to_string(++circuitCount));

        sf::Vector2f pos = c->getBoundingBox().getPosition() + c->getBoundingBox().getSize() / 2.f;

        dragger->setSelection({c});
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

        dragger->setSelection({});
        selector->clearSelection();
        selected.clear();
        return true;
    });

    Tool* activeTool = nullptr;
    // --- EVENT LOOP ---
    while (window.isOpen()) {
        std::list<sf::Event> events;

        for (sf::Event e; window.pollEvent(e);)
            events.push_back(e);
        for (sf::Event e : events) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }
            window.setView(view);
            world.onEvent(window, e);
            for(Tool* tool : tools) {
                tool->onEvent(window, e);
                if(tool->isActive()) {
                    activeTool = tool;
                    break;
                }
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
        world.update(window);

        sf::Vector2i newPos = sf::Mouse::getPosition(window);
        sf::Vector2f newWorldPos = window.mapPixelToCoords(newPos);
        if (activeTool) {
            activeTool->update(window);
        }

        mouse = newPos;

        window.clear();

        // --- WORLD VIEW ---
        window.setView(view);
        world.draw(window);

        for(const Tool* tool : tools) {
            tool->draw(window);
        }
        // --- GUI VIEW ---
        window.setView(window.getDefaultView());
        window.draw(buttonShape);
        window.display();
    }
    world.saveToFile("assets/world.json");
    return 0;
}
