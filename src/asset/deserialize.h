#pragma once

#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Texture.hpp"

#include "asset/AssetLoader.h"
#include "circuit/BinaryGate.h"
#include "circuit/CompositeCircuit.h"
#include "circuit/NotGate.h"

#include "json.hpp"
using json = nlohmann::json;

namespace serde {
BinaryGate* createBinaryGate(const json& j, BinaryGate::Func func, const sf::Texture& assets);
NotGate* createNot(const json& j, const sf::Texture& texture);
CompositeCircuit* createComposite(const json& j, const sf::Font& font, const AssetLoader<Circuit, std::string>& loader); 
} // namespace serde
