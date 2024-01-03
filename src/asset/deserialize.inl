#pragma once

#include "circuit/NotGate.h"
#include "deserialize.h"

namespace serde {
BinaryGate* createBinaryGate(const json& j, BinaryGate::Func func, const sf::Texture& texture) {
    std::string id = j["id"].get<std::string>();
    float x = j["position"]["x"].get<float>();
    float y = j["position"]["y"].get<float>();

    return new BinaryGate(id, texture, func, {x, y});
}

NotGate* createNot(const json& j, const sf::Texture& texture) {
    std::string id = j["id"].get<std::string>();
    float x = j["position"]["x"].get<float>();
    float y = j["position"]["y"].get<float>();

    return new NotGate(id, texture, {x, y});
}
} // namespace serde
