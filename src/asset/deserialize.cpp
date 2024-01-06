#include "deserialize.h"
#include "asset/AssetLoader.h"
#include "circuit/NotGate.h"

#include <fstream>
#include <iostream>

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
CompositeCircuit* createComposite(
    const json& j, const sf::Font& font, const AssetLoader<Circuit, std::string>& loader
) {
    std::string type = j["type"].get<std::string>();
    std::string path = "assets/" + type + ".json";
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cout << "[WARN/serde] Could not load composite circuit: " << type << std::endl;
        return nullptr;
    }

    std::string id = j["id"].get<std::string>();
    float x = j["position"]["x"].get<float>();
    float y = j["position"]["y"].get<float>();
    auto circuit = std::make_unique<CompositeCircuit>(id, type, font, sf::Vector2f(x, y));

    json data = json::parse(file);

    circuit->setInputCount(data["inputs"].size());
    circuit->setOutputCount(data["outputs"].size());

    std::cout << "[INFO/serde] Loading circuits" << std::endl;
    for (auto elem : data["elements"]) {
        std::string type = elem["type"].get<std::string>();
        Circuit* inner = loader.read(type, elem);
        if (!inner) {
            std::cout << "[WARN/serde] Failed to load circuit: " << type << std::endl;
            continue;
        }
        circuit->addCircuit(inner);
    }

    std::cout << "[INFO/serde] Loading wires" << std::endl;
    for (auto w : data["wires"]) {
        std::string froms, tos;
        w["from"].get_to(froms);
        w["to"].get_to(tos);
        if (!circuit->connectPins(froms, tos)) {
            std::cout << "[WARN/serde] Failed to load wire: " << froms << " -> " << tos
                      << std::endl;
        }
    }

    return circuit.release();
}
} // namespace serde
