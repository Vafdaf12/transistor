#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/ContextSettings.hpp"
#include "SFML/Window/Event.hpp"
#include "app/CircuitEditor.h"
#include "asset/AssetLoader.h"
#include "asset/ResourceManager.h"
#include "asset/deserialize.h"
#include "circuit/BinaryGate.h"
#include "circuit/CompositeCircuit.h"
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
        if (!editor.addInput(id.get<std::string>())) {
            std::cout << "[WARN/CircuitEditor] Failed to load input: " << id << std::endl;
        }
    }

    std::cout << "[INFO/CircuitEditor] Loading outputs" << std::endl;
    for (auto id : data["outputs"]) {
        if (!editor.addOutput(id.get<std::string>())) {
            std::cout << "[WARN/CircuitEditor] Failed to load input: " << id << std::endl;
        }
    }

    std::cout << "[INFO/CircuitEditor] Loading circuits" << std::endl;
    for (auto elem : data["elements"]) {
        std::string type = elem["type"].get<std::string>();
        Circuit* circuit = circuits.read(type, elem);
        if (!circuit) {
            std::cout << "[WARN/CircuitEditor] Failed to load circuit: " << type << std::endl;
            continue;
        }
        editor.addCircuit(circuit);
    }

    std::cout << "[INFO/CircuitEditor] Loading wires" << std::endl;
    for (auto w : data["wires"]) {
        std::string froms, tos;
        w["from"].get_to(froms);
        w["to"].get_to(tos);
        Pin* from = editor.queryPin(froms);
        Pin* to = editor.queryPin(tos);

        if (!from || !to) {
            std::cout << "[WARN/CircuitEditor] Failed to load wire: " << froms << " -> " << tos
                      << std::endl;
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

int main(int argc, char** argv) {
    // --- RESOURCES ---

    std::cout << "[INFO] Loading Assets" << std::endl;

    sf::Font font;
    font.loadFromFile("data/fonts/CutiveMono-Regular.ttf");

    ResourceManager<std::string, sf::Texture> assets;
    assets.load("gate_not", "data/sprites/gate_not.png");
    assets.load("gate_or", "data/sprites/gate_or.png");
    assets.load("gate_xor", "data/sprites/gate_xor.png");
    assets.load("gate_and", "data/sprites/gate_and.png");
    assets.load("gate_nand", "data/sprites/gate_nand.png");

    AssetLoader<Circuit, std::string> loader;
    using namespace std::placeholders;
    loader.addType(
        "or_gate", std::bind(serde::createBinaryGate, _1, BinaryGate::Or, assets.get("gate_or"))
    );
    loader.addType(
        "xor_gate", std::bind(serde::createBinaryGate, _1, BinaryGate::Xor, assets.get("gate_xor"))
    );
    loader.addType(
        "and_gate", std::bind(serde::createBinaryGate, _1, BinaryGate::And, assets.get("gate_and"))
    );
    loader.addType(
        "nand_gate",
        std::bind(serde::createBinaryGate, _1, BinaryGate::Nand, assets.get("gate_nand"))
    );
    loader.addType("not_gate", std::bind(serde::createNot, _1, assets.get("gate_not")));
    loader.setDefault(std::bind(serde::createComposite, _2, font, loader));

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
    std::string filePath = argc > 1 ? argv[1] : "data/world.json";
    openEditor(editor, filePath, loader);

    // --- GUI ---
    sf::View gui = window.getDefaultView();

    std::vector<std::unique_ptr<ui::Widget>> widgets;

    std::cout << "[INFO] Loading Sprites" << std::endl;
    ui::ImageView* imageView = new ui::ImageView(assets.get("gate_xor"));
    imageView->getSprite().setScale(0.5f, 0.5f);
    imageView->getSprite().setColor(sf::Color::Cyan);
    widgets.emplace_back(new ui::CircuitButton(
        editor, new BinaryGate("xor", assets.get("gate_xor"), BinaryGate::Xor), imageView
    ));

    imageView = new ui::ImageView(assets.get("gate_and"));
    imageView->getSprite().setScale(0.5f, 0.5f);
    imageView->getSprite().setColor(sf::Color::Cyan);
    widgets.emplace_back(new ui::CircuitButton(
        editor, new BinaryGate("and", assets.get("gate_and"), BinaryGate::And), imageView
    ));

    imageView = new ui::ImageView(assets.get("gate_or"));
    imageView->getSprite().setScale(0.5f, 0.5f);
    imageView->getSprite().setColor(sf::Color::Cyan);
    widgets.emplace_back(new ui::CircuitButton(
        editor, new BinaryGate("or", assets.get("gate_or"), BinaryGate::Or), imageView
    ));

    imageView = new ui::ImageView(assets.get("gate_not"));
    imageView->getSprite().setScale(0.5f, 0.5f);
    imageView->getSprite().setColor(sf::Color::Cyan);
    widgets.emplace_back(
        new ui::CircuitButton(editor, new NotGate("not", assets.get("gate_not")), imageView)
    );

    CompositeCircuit* tmp = new CompositeCircuit("bingus", "bing", font);
    tmp->setLabel("Cool");
    tmp->setInputCount(2);
    tmp->setOutputCount(2);

    assert(tmp->addCircuit(new BinaryGate("or1", assets.get("gate_or"), BinaryGate::Or)));
    assert(tmp->addCircuit(new NotGate("not1", assets.get("gate_not"))));

    assert(tmp->addCircuit(new BinaryGate("or2", assets.get("gate_or"), BinaryGate::Or)));
    assert(tmp->addCircuit(new NotGate("not2", assets.get("gate_not"))));

    // Connect input/outputs pins
    assert(tmp->connectPins("in0", "or1/in0"));
    assert(tmp->connectPins("in1", "or2/in1"));
    assert(tmp->connectPins("not1/out0", "out0"));
    assert(tmp->connectPins("not2/out0", "out1"));

    // Connect NOR pins
    assert(tmp->connectPins("or1/out0", "not1/in0"));
    assert(tmp->connectPins("or2/out0", "not2/in0"));

    // Connect feedback pins
    assert(tmp->connectPins("not1/out0", "or2/in0"));
    assert(tmp->connectPins("not2/out0", "or1/in1"));

    imageView = new ui::ImageView(assets.get("gate_not"));
    imageView->getSprite().setScale(0.5f, 0.5f);
    imageView->getSprite().setColor(sf::Color::Red);
    widgets.emplace_back(new ui::CircuitButton(editor, tmp, imageView));

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
    saveEditor(editor, filePath);
}
