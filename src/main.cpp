#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Transform.hpp"
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
#include "asset/deserialize.h"
#include "asset/CommandLoader.h"
#include "circuit/Circuit.h"
#include "circuit/NandCircuit.h"
#include "circuit/BinaryGate.h"
#include "circuit/NotGate.h"
#include "game/GameWorld.h"
#include "pin/Pin.h"
#include "tools/CircuitDragger.h"
#include "tools/NavigationTool.h"
#include "tools/PinConnector.h"
#include "tools/SelectionTool.h"
#include "tools/Tool.h"

#include "CircuitButton.h"

using SfLayer = EventLayer<sf::Event::EventType, sf::Event>;

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
    assets.textures.load("gate_not", "assets/sprites/gate_not.png");

    CircuitRegistry registry;
    registry.add("or_gate", [&assets](const json& j) { return serde::createOr(j, assets); });
    registry.add("xor_gate", [&assets](const json& j) { return serde::createXor(j, assets); });
    registry.add("and_gate", [&assets](const json& j) { return serde::createAnd(j, assets); });
    registry.add("nand_gate", [&assets](const json& j) { return serde::createNand(j, assets); });
    registry.add("not_gate", [&assets](const json& j) { return serde::createNot(j, assets); });

    DragBoard board;

    sf::View view;
    sf::Transform x;
    view.setCenter({0, 0});
    view.setSize({1280, 720});

    GameWorld world;
    world.loadFromFile("assets/world.json", registry);
    CommandLoader loader;

    int circuitCount = 0;
    SfLayer guiLayer;
    SfLayer worldLayer;

    sf::Vector2i mouse = sf::Mouse::getPosition();

    ToolState state = NONE;

    std::list<Tool*> tools;
    tools.push_back(new NavigationTool(view));
    tools.push_back(new PinConnector(world));

    CircuitDragger* dragger = new CircuitDragger(world, board);
    tools.push_back(dragger);

    SelectionTool* selector = new SelectionTool(world, board);
    tools.push_back(selector);

    Circuit* dragBoard = nullptr;

    NandCircuit* proto = new NandCircuit("nand", assets);
    BinaryGate* proto2 = new BinaryGate("and", assets, BinaryGate::And);
    BinaryGate* proto3 = new BinaryGate("or", assets, BinaryGate::Or);
    BinaryGate* proto4 = new BinaryGate("xor", assets, BinaryGate::Xor);
    Circuit* proto5 = new NotGate("not", assets);

    // --- GUI UPDATES ---
    std::vector<CircuitButton> circuitButtons;

    circuitButtons.emplace_back(world, *proto, *dragger);
    circuitButtons.back().setView(&world.getScreenView());
    circuitButtons.back().getShape().setFillColor(sf::Color::White);
    circuitButtons.back().getShape().setPosition({10, 10});
    circuitButtons.back().getShape().setSize({200, 75});

    circuitButtons.emplace_back(world, *proto2, *dragger);
    circuitButtons.back().setView(&world.getScreenView());
    circuitButtons.back().getShape().setFillColor(sf::Color::Yellow);
    circuitButtons.back().getShape().setPosition({10, 100});
    circuitButtons.back().getShape().setSize({100, 50});

    circuitButtons.emplace_back(world, *proto3, *dragger);
    circuitButtons.back().setView(&world.getScreenView());
    circuitButtons.back().getShape().setFillColor(sf::Color::Blue);
    circuitButtons.back().getShape().setPosition({10, 160});
    circuitButtons.back().getShape().setSize({100, 50});
    
    circuitButtons.emplace_back(world, *proto4, *dragger);
    circuitButtons.back().setView(&world.getScreenView());
    circuitButtons.back().getShape().setFillColor(sf::Color::Magenta);
    circuitButtons.back().getShape().setPosition({10, 220});
    circuitButtons.back().getShape().setSize({100, 50});

    circuitButtons.emplace_back(world, *proto5, *dragger);
    circuitButtons.back().setView(&world.getScreenView());
    circuitButtons.back().getShape().setFillColor(sf::Color::Red);
    circuitButtons.back().getShape().setPosition({10, 290});
    circuitButtons.back().getShape().setSize({100, 50});

    // --- CIRCUIT DELETION ---
    worldLayer.subscribe(sf::Event::KeyReleased, [&](const sf::Event& event) {
        if (event.key.code != sf::Keyboard::Delete)
            return false;
        std::vector<Circuit*> selected = board.getSelection();
        if (selected.empty()) {
            std::cout << "Nothing to delete" << std::endl;
            return false;
        }
        for (auto s : selected) {
            world.removeCircuit(s);
        }

        board.clearSelection();
        return true;
    });

    // --- EVENT LOOP ---
    while (window.isOpen()) {
        for (sf::Event e; window.pollEvent(e);) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }
            window.setView(window.getDefaultView());
            if (guiLayer.handle(e.type, e))
                continue;

            window.setView(view);

            bool handled = false;
            for(auto& btn : circuitButtons) {
                btn.onEvent(window, e);
                handled = handled || btn.isActive();
            }
            if(handled) {
                continue;
            }

            worldLayer.handle(e.type, e);

            window.setView(view);
            world.onEvent(window, e);
            for(Tool* tool : tools) {
                tool->onEvent(window, e);
            }
        }

        window.setView(view);
        world.update(window);
        for(auto& btn : circuitButtons) {
            btn.update(window);
        }

        sf::Vector2i newPos = sf::Mouse::getPosition(window);
        sf::Vector2f newWorldPos = window.mapPixelToCoords(newPos);
        for(Tool* tool : tools) {
            tool->update(window);
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
        window.setView(world.getScreenView());
        for(const auto& btn : circuitButtons) {
            btn.draw(window);
        }
        window.display();
    }
    world.saveToFile("assets/world.json");
    return 0;
}
