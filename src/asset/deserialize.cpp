#include "deserialize.h"

namespace serde {
    NandCircuit* createNand(const json& j, const Assets& assets) {
        std::string id = j["id"].get<std::string>();
        float x = j["position"]["x"].get<float>();
        float y = j["position"]["y"].get<float>();

        return new NandCircuit(id, assets, {x, y});
    }

    BinaryGate* createAnd(const json& j, const Assets& assets) {
        std::string id = j["id"].get<std::string>();
        float x = j["position"]["x"].get<float>();
        float y = j["position"]["y"].get<float>();
        return new BinaryGate(id, assets, BinaryGate::And, {x, y});
    }
    BinaryGate* createXor(const json& j, const Assets& assets) {
        std::string id = j["id"].get<std::string>();
        float x = j["position"]["x"].get<float>();
        float y = j["position"]["y"].get<float>();
        return new BinaryGate(id, assets, BinaryGate::Xor, {x, y});
    }
    BinaryGate* createOr(const json& j, const Assets& assets) {
        std::string id = j["id"].get<std::string>();
        float x = j["position"]["x"].get<float>();
        float y = j["position"]["y"].get<float>();
        return new BinaryGate(id, assets, BinaryGate::Or, {x, y});
    }
    NotGate* createNot(const json& j, const Assets& assets) {
        std::string id = j["id"].get<std::string>();
        float x = j["position"]["x"].get<float>();
        float y = j["position"]["y"].get<float>();
        return new NotGate(id, assets, {x, y});
    }
}

