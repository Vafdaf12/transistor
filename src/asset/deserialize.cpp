#include "deserialize.h"
#include "circuit/NotGate.h"

namespace serde {
    template<BinaryGate::Func F>
    BinaryGate* createBinaryGate(const json& j, const ResourceManager<BinaryGate::Func, sf::Texture>& assets) {
        std::string id = j["id"].get<std::string>();
        float x = j["position"]["x"].get<float>();
        float y = j["position"]["y"].get<float>();

        return new BinaryGate(id, assets, F, {x, y});
    }

    NotGate* createNot(const json& j, const Assets& assets) {
        std::string id = j["id"].get<std::string>();
        float x = j["position"]["x"].get<float>();
        float y = j["position"]["y"].get<float>();

        return new NotGate(id, assets, {x, y});
    }
}

