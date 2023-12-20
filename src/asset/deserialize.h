#pragma once

#include "circuit/BinaryGate.h"
#include "circuit/NotGate.h"

#include "asset/ResourceManager.h"

#include "json.hpp"
using json = nlohmann::json;

namespace serde {
    template<BinaryGate::Func F>
    BinaryGate* createBinaryGate(const json& j, const ResourceManager<BinaryGate::Func, sf::Texture>& assets);

    NotGate* createNot(const json& j, const Assets& assets);
}

