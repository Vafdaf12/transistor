#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/ContextSettings.hpp"
#include "SFML/Window/Event.hpp"
#include "app/CircuitEditor.h"
#include "asset/AssetSystem.h"
#include "asset/ResourceManager.h"
#include "circuit/BinaryGate.h"

#include <iostream>

int main(int, char**) {
    // --- RESOURCES ---
    Assets assets;
    assets.textures.load("gate_not", "assets/sprites/gate_not.png");

    ResourceManager<BinaryGate::Func, sf::Texture> gateTextures;
    gateTextures.load(BinaryGate::Or, "assets/sprites/gate_or.png");
    gateTextures.load(BinaryGate::Xor, "assets/sprites/gate_xor.png");
    gateTextures.load(BinaryGate::And, "assets/sprites/gate_and.png");
    gateTextures.load(BinaryGate::Nand, "assets/sprites/gate_nand.png");

    /*
    CircuitRegistry registry;
    registry.add("or_gate", [&](const json& j) { return serde::createBinaryGate<BinaryGate::Or>(j, gateTextures); });
    registry.add("xor_gate", [&](const json& j) { return serde::createBinaryGate<BinaryGate::Xor>(j, gateTextures); });
    registry.add("and_gate", [&](const json& j) { return serde::createBinaryGate<BinaryGate::And>(j, gateTextures); });
    registry.add("nand_gate", [&](const json& j) { return serde::createBinaryGate<BinaryGate::Nand>(j, gateTextures); });
    registry.add("not_gate", [&](const json& j) { return serde::createNot(j, assets); });
    */


    // --- WINDOW SETUP ---
    sf::Clock clock;
    sf::ContextSettings settings;
    settings.antialiasingLevel = 5;
    sf::RenderWindow window({1280, 720}, "Transistor", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    // --- GAME WORLD ---
    sf::View view = window.getDefaultView();
    view.setCenter(0, 0);
    CircuitEditor editor(window.getDefaultView(), view);
    editor.addInput("in0");
    editor.addInput("in1");
    editor.addInput("in2");
    editor.addOutput("out0");
    editor.addOutput("out1");
    editor.addCircuit(new BinaryGate("pin", gateTextures, BinaryGate::Nand, {200, 0}));
    editor.addCircuit(new BinaryGate("pin2", gateTextures, BinaryGate::Xor, {200, 100}));

    // --- EVENT LOOP ---
    float time = clock.restart().asMilliseconds();
    std::cout << "Startup time: " << time << "ms" << std::endl;
    while (window.isOpen()) {
        // --- EVENT HANDLING ---
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            editor.onEvent(window, event);
        }

        // --- REALTIME UPDATES ---
        float dt = clock.restart().asSeconds();
        editor.update(window, dt);

        // --- RENDERING ---
        window.clear();
        editor.draw(window);
        window.display();
    }

    /*

    GameWorld world;
    world.loadFromFile("assets/world.json", registry);
    CommandLoader loader;


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

    world.saveToFile("assets/world.json");
    return 0;
    */
}
