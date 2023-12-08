#pragma once

#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Texture.hpp"

#include "ResourceManager.h"

#include <string>

struct Assets {
    ResourceManager<std::string, sf::Texture> textures;
    ResourceManager<std::string, sf::Font> fonts;
};