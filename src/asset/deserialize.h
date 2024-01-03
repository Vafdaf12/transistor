#pragma once

#include "circuit/BinaryGate.h"
#include "circuit/NotGate.h"

#include "json.hpp"
using json = nlohmann::json;

namespace serde {
BinaryGate* createBinaryGate(const json& j, BinaryGate::Func func, const sf::Texture& assets);
NotGate* createNot(const json& j, const sf::Texture& texture);
} // namespace serde

#include "deserialize.inl"
