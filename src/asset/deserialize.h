#pragma once

#include "circuit/Circuit.h"
#include "circuit/NandCircuit.h"
#include "circuit/BinaryGate.h"
#include "circuit/NotGate.h"

#include "json.hpp"
using json = nlohmann::json;

namespace serde {
    NandCircuit* createNand(const json& j, const Assets& assets); 
    BinaryGate* createAnd(const json& j, const Assets& assets);
    BinaryGate* createXor(const json& j, const Assets& assets);
    BinaryGate* createOr(const json& j, const Assets& assets);
    NotGate* createNot(const json& j, const Assets& assets);
}

