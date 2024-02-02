#pragma once

#include "app/CircuitEditor.h"
#include "asset/AssetLoader.h"
#include "asset/ResourceManager.h"
#include "circuit/Circuit.h"
#include "ui/Widget.h"

#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Window/Event.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>

class Application {
public:
    int exec(int argc, char** argv);

private:
    void init(int argc, char** argv);
    void finalise();

    void handleEvent(const sf::Event& event);
    void update(float dt);

    void render(sf::RenderWindow& window) const;
    bool shouldQuit() const;

    void loadAssets();
    void createInterface();

    void loadEditor(const std::string& path);
    void saveEditor();

    sf::RenderWindow m_window;
    std::shared_ptr<spdlog::sinks::stderr_color_sink_st> m_logger;

    sf::Font m_font;
    ResourceManager<std::string, sf::Texture> m_textures;
    AssetLoader<Circuit, std::string> m_circuitLoader;

    std::unique_ptr<CircuitEditor> m_editor = nullptr;
    std::string m_filePath;
    std::unique_ptr<ui::Widget> m_gui;
};
