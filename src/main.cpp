#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/ContextSettings.hpp"
#include "SFML/Window/Event.hpp"
#include "app/CircuitEditor.h"
#include "asset/AssetLoader.h"
#include "asset/ResourceManager.h"
#include "asset/deserialize.h"
#include "circuit/BinaryGate.h"
#include "circuit/NotGate.h"
#include "ui/CircuitButton.h"
#include "ui/ImageView.h"
#include "ui/Widget.h"

#include <fstream>
#include <iostream>
#include <string>

bool openEditor(
    CircuitEditor& editor,
    const std::string& path,
    const AssetLoader<Circuit, std::string>& circuits
) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    json data = json::parse(file);

    std::cout << "[INFO/CircuitEditor] Loading inputs" << std::endl;
    for (auto id : data["inputs"]) {
        if(!editor.addInput(id.get<std::string>())) {
            std::cout << "[WARN/CircuitEditor] Failed to load input: "  << id << std::endl;
        }
    }

    std::cout << "[INFO/CircuitEditor] Loading outputs" << std::endl;
    for (auto id : data["outputs"]) {
        if(!editor.addOutput(id.get<std::string>())) {
            std::cout << "[WARN/CircuitEditor] Failed to load input: "  << id << std::endl;
        }
    }

    std::cout << "[INFO/CircuitEditor] Loading circuits" << std::endl;
    for (auto elem : data["elements"]) {
        std::string type = elem["type"].get<std::string>();
        Circuit* circuit = circuits.read(type, elem);
        editor.addCircuit(circuit);
    }

    std::cout << "[INFO/CircuitEditor] Loading wires" << std::endl;
    for (auto w : data["wires"]) {
        std::string froms, tos;
        w["from"].get_to(froms);
        w["to"].get_to(tos);
        Pin* from = editor.queryPin(froms);
        Pin* to = editor.queryPin(tos);

        if(!from || !to) {
            std::cout << "[WARN/CircuitEditor] Failed to load wire: "  << froms << " -> " << tos << std::endl;
            continue;
        }
        editor.addWire(from, to);
    }
    std::cout << "[INFO/CircuitEditor] Done." << std::endl;

    return true;
}

bool saveEditor(const CircuitEditor& editor, const std::string& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    json data;
    editor.toJson(data);
    file << std::setw(4) << data;
    return true;
}

int main(int, char**) {
    // --- RESOURCES ---
    
    std::cout << "[INFO] Loading Assets" << std::endl;
    ResourceManager<std::string, sf::Texture> assets;
    assets.load("gate_not", "assets/sprites/gate_not.png");

    ResourceManager<BinaryGate::Func, sf::Texture> gateTextures;
    gateTextures.load(BinaryGate::Or, "assets/sprites/gate_or.png");
    gateTextures.load(BinaryGate::Xor, "assets/sprites/gate_xor.png");
    gateTextures.load(BinaryGate::And, "assets/sprites/gate_and.png");
    gateTextures.load(BinaryGate::Nand, "assets/sprites/gate_nand.png");

    AssetLoader<Circuit, std::string> context;
    context.addType("or_gate", [&](const json& j) {
        return serde::createBinaryGate<BinaryGate::Or>(j, gateTextures);
    });
    context.addType("xor_gate", [&](const json& j) {
        return serde::createBinaryGate<BinaryGate::Xor>(j, gateTextures);
    });
    context.addType("and_gate", [&](const json& j) {
        return serde::createBinaryGate<BinaryGate::And>(j, gateTextures);
    });
    context.addType("nand_gate", [&](const json& j) {
        return serde::createBinaryGate<BinaryGate::Nand>(j, gateTextures);
    });
    context.addType("not_gate", [&](const json& j) { return serde::createNot(j, assets.get("gate_not")); });

    // --- WINDOW SETUP ---
    std::cout << "[INFO] Setting up window" << std::endl;
    sf::Clock clock;
    sf::ContextSettings settings;
    settings.antialiasingLevel = 5;
    sf::RenderWindow window({1280, 720}, "Transistor", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    // --- GAME WORLD ---
    CircuitEditor editor(window.getDefaultView());

    std::cout << "[INFO] Loading editor" << std::endl;
    openEditor(editor, "assets/world.json", context);

    // --- GUI ---
    sf::View gui = window.getDefaultView();

    std::vector<std::unique_ptr<ui::Widget>> widgets;

    std::cout << "[INFO] Loading Sprites" << std::endl;
    ui::ImageView* imageView = new ui::ImageView(gateTextures.get(BinaryGate::Xor));
    imageView->getSprite().setScale(0.5f, 0.5f);
    imageView->getSprite().setColor(sf::Color::Cyan);
    widgets.emplace_back(new ui::CircuitButton(
        editor, new BinaryGate("xor", gateTextures.get(BinaryGate::Xor), BinaryGate::Xor), imageView
    ));

    imageView = new ui::ImageView(gateTextures.get(BinaryGate::And));
    imageView->getSprite().setScale(0.5f, 0.5f);
    imageView->getSprite().setColor(sf::Color::Cyan);
    widgets.emplace_back(new ui::CircuitButton(
        editor, new BinaryGate("and", gateTextures.get(BinaryGate::And), BinaryGate::And), imageView
    ));

    imageView = new ui::ImageView(gateTextures.get(BinaryGate::Or));
    imageView->getSprite().setScale(0.5f, 0.5f);
    imageView->getSprite().setColor(sf::Color::Cyan);
    widgets.emplace_back(
        new ui::CircuitButton(editor, new BinaryGate("or", gateTextures.get(BinaryGate::Or), BinaryGate::Or), imageView)
    );

    imageView = new ui::ImageView(assets.get("gate_not"));
    imageView->getSprite().setScale(0.5f, 0.5f);
    imageView->getSprite().setColor(sf::Color::Cyan);
    widgets.emplace_back(new ui::CircuitButton(editor, new NotGate("not", assets.get("gate_not")), imageView));

    sf::Vector2f offset(10, 10);
    for (auto& w : widgets) {
        w->setPosition(offset);
        offset += sf::Vector2f(w->getBoundingBox().getSize().x + 10, 0);
    }

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
            if (event.type == sf::Event::Resized) {
                gui = window.getDefaultView();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            window.setView(gui);
            bool handled = false;
            for (auto& w : widgets) {
                if (w->onEvent(window, event)) {
                    handled = true;
                }
            }
            if (handled) {
                continue;
            }
            editor.onEvent(window, event);
        }

        // --- REALTIME UPDATES ---
        float dt = clock.restart().asSeconds();
        editor.update(window, dt);
        window.setView(gui);
        for (auto& w : widgets) {
            w->update(window, dt);
        }

        // --- RENDERING ---
        window.clear(sf::Color(0x181818ff));
        editor.draw(window);

        window.setView(gui);
        for (auto& w : widgets) {
            w->draw(window);
        }
        window.display();
    }
    saveEditor(editor, "assets/world.json");
}
