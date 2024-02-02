#include "Application.h"

#include "asset/deserialize.h"
#include "circuit/BinaryGate.h"
#include "circuit/view/SpriteView.h"
#include "ui/components/CircuitButton.h"
#include "ui/components/ImageView.h"
#include "ui/components/InputLabel.h"
#include "ui/layout/HBox.h"

#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include <cassert>
#include <exception>
#include <fstream>
#include <stdexcept>

void Application::init(int argc, char** argv) {
    // Logging categories
    m_logger = std::make_shared<spdlog::sinks::stderr_color_sink_st>();
    spdlog::register_logger(std::make_shared<spdlog::logger>("editor", m_logger));
    spdlog::register_logger(std::make_shared<spdlog::logger>("file", m_logger));
    spdlog::register_logger(std::make_shared<spdlog::logger>("ui", m_logger));

#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    spdlog::debug("Creating window");
    sf::ContextSettings settings;
    settings.antialiasingLevel = 5;
    m_window.create({1280, 720}, "Transistor", sf::Style::Default, settings);
    m_window.setVerticalSyncEnabled(true);
    m_screenSpace = m_window.getDefaultView();

    spdlog::debug("Loading assets");
    m_textures.load("gate_not", "data/sprites/gate_not.png");
    m_textures.load("gate_or", "data/sprites/gate_or.png");
    m_textures.load("gate_xor", "data/sprites/gate_xor.png");
    m_textures.load("gate_and", "data/sprites/gate_and.png");
    m_textures.load("gate_nand", "data/sprites/gate_nand.png");
    if (!m_font.loadFromFile("data/fonts/CutiveMono-Regular.ttf")) {
        throw std::runtime_error("Failed to load font");
    }

    spdlog::debug("Registering circuits");
    using namespace std::placeholders;
    m_circuitLoader.addType(
        "or_gate", std::bind(serde::createBinaryGate, _1, BinaryGate::Or, m_textures.get("gate_or"))
    );
    m_circuitLoader.addType(
        "xor_gate",
        std::bind(serde::createBinaryGate, _1, BinaryGate::Xor, m_textures.get("gate_xor"))
    );
    m_circuitLoader.addType(
        "and_gate",
        std::bind(serde::createBinaryGate, _1, BinaryGate::And, m_textures.get("gate_and"))
    );
    m_circuitLoader.addType(
        "nand_gate",
        std::bind(serde::createBinaryGate, _1, BinaryGate::Nand, m_textures.get("gate_nand"))
    );
    m_circuitLoader.addType(
        "not_gate", std::bind(serde::createNot, _1, m_textures.get("gate_not"))
    );
    m_circuitLoader.setDefault(
        std::bind(serde::createComposite, _2, m_font, std::cref(m_circuitLoader))
    );

    m_editor = std::make_unique<CircuitEditor>(m_screenSpace);
    loadEditor(argc > 1 ? argv[1] : "data/world.json");

    createInterface();
}

void Application::loadEditor(const std::string& path) {
    auto logger = spdlog::get("editor");

    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::invalid_argument("Could not open file: " + path);
    }

    json data = json::parse(file);
    logger->info("Loading editor from file: {}", path);

    logger->debug("Loading pins");
    for (auto id : data["inputs"]) {
        m_editor->addInput(id.get<std::string>());
    }
    for (auto id : data["outputs"]) {
        m_editor->addOutput(id.get<std::string>());
    }

    logger->debug("Loading circuits");
    for (auto elem : data["elements"]) {
        std::string type = elem["type"].get<std::string>();
        Circuit* circuit = m_circuitLoader.read(type, elem);
        if (!circuit) {
            logger->warn("Failed to load circuit of type \"{}\". Skipping.", type);
            continue;
        }
        m_editor->addCircuit(circuit);
    }

    logger->debug("Loading wires");
    for (auto w : data["wires"]) {
        std::string froms, tos;
        w["from"].get_to(froms);
        w["to"].get_to(tos);
        Pin* from = m_editor->queryPin(froms);
        Pin* to = m_editor->queryPin(tos);

        if (!from || !to) {
            logger->warn("Failed to load wire {} -> {}: pins not found. Skipping.", froms, tos);
            continue;
        }
        m_editor->addWire(from, to);
    }
    m_filePath = path;
    logger->info("Finished loading editor");
}

void Application::finalise() {
    std::ofstream file(m_filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + m_filePath);
    }

    json data;
    m_editor->toJson(data);
    file << std::setw(4) << data;
}

void Application::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        m_window.close();
    };
    if (event.type == sf::Event::Resized) {
        m_gui->setAnchor({event.size.width / 2.f, 0});
        m_screenSpace.setSize(sf::Vector2f(event.size.width, event.size.height));
        m_screenSpace.setCenter(sf::Vector2f(event.size.width, event.size.height) / 2.f);
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        m_window.close();
    }
    m_window.setView(m_screenSpace);
    bool handled = m_gui->onEvent(m_window, event);
    if (handled) {
        return;
    }

    m_editor->onEvent(m_window, event);
}

void Application::update(float dt) {
    m_editor->update(m_window, dt);
    m_window.setView(m_screenSpace);
    m_gui->update(m_window, dt);
}

void Application::render(sf::RenderWindow& window) const {
    assert(&m_window == &window);
    m_editor->draw(window);

    window.setView(m_screenSpace);
    m_gui->draw(window);
}

void Application::createInterface() {
    static const sf::Color BACKGROUND = sf::Color(0x404040ff);

    auto root = std::make_unique<ui::HBox>();
    root->setPadding(10);
    root->setSeperation(10);
    root->setBackground(BACKGROUND);

    ui::ImageView* proto = new ui::ImageView;
    proto->setScale({0.4f, 0.4f});
    proto->setTint(sf::Color::Cyan);

    ui::ImageView* imageView = new ui::ImageView(*proto);
    imageView->setTexture(m_textures.get("gate_xor"));
    assert(root->addWidget(new ui::CircuitButton(
        *m_editor,
        new BinaryGate("xor", BinaryGate::Xor, new SpriteView(m_textures.get("gate_xor"))),
        imageView
    )));

    imageView = new ui::ImageView(*proto);
    imageView->setTexture(m_textures.get("gate_and"));
    assert(root->addWidget(new ui::CircuitButton(
        *m_editor,
        new BinaryGate("and", BinaryGate::And, new SpriteView(m_textures.get("gate_and"))),
        imageView
    )));

    imageView = new ui::ImageView(*proto);
    imageView->setTexture(m_textures.get("gate_or"));
    assert(root->addWidget(new ui::CircuitButton(
        *m_editor,
        new BinaryGate("or", BinaryGate::Or, new SpriteView(m_textures.get("gate_or"))),
        imageView
    )));

    imageView = new ui::ImageView(*proto);
    imageView->setTexture(m_textures.get("gate_not"));
    assert(root->addWidget(new ui::CircuitButton(
        *m_editor, new NotGate("not", new SpriteView(m_textures.get("gate_not"))), imageView
    )));

    root->addWidget(new ui::InputLabel("hi", m_font));

    float x = (m_window.getSize().x - root->getBoundingBox().width) / 2;
    root->setPosition({x, 10});
    root->setAnchor({m_window.getSize().x / 2.f, 0}, false);

    m_gui = std::move(root);
}

int Application::exec(int argc, char** argv) {
    try {
        init(argc, argv);
        spdlog::info("Application initialized");

        sf::Clock clock;
        while (m_window.isOpen()) {
            for (sf::Event e; m_window.pollEvent(e);) {
                handleEvent(e);
            }
            float delta = clock.restart().asSeconds();
            update(delta);
            m_window.clear(sf::Color(0x181818ff));
            render(m_window);
            m_window.display();
        }
        finalise();
        return 0;
    } catch (const std::exception& e) {
        spdlog::error("An exception occured: {}", e.what());
        return 1;
    }
}
