#include "deserialize.h"
#include "asset/AssetLoader.h"
#include "circuit/NotGate.h"
#include "circuit/view/SpriteView.h"
#include "circuit/view/TextView.h"
#include "spdlog/spdlog.h"

#include <fstream>

namespace serde {
BinaryGate* createBinaryGate(const json& j, BinaryGate::Func func, const sf::Texture& texture) {
    std::string id = j["id"].get<std::string>();
    float x = j["position"]["x"].get<float>();
    float y = j["position"]["y"].get<float>();

    return new BinaryGate(id, func, new SpriteView(texture, {x, y}));
}

NotGate* createNot(const json& j, const sf::Texture& texture) {
    std::string id = j["id"].get<std::string>();
    float x = j["position"]["x"].get<float>();
    float y = j["position"]["y"].get<float>();

    return new NotGate(id, new SpriteView(texture, {x, y}));
}
CompositeCircuit* createComposite(
    const json& j, const sf::Font& font, const AssetLoader<Circuit, std::string>& loader
) {
    auto logger = spdlog::get("file");

    std::string type = j["type"].get<std::string>();
    std::string path = "data/" + type + ".json";
    std::ifstream file(path);

    if (!file.is_open()) {
        logger->error("Failed load circuit from file \"{}\": Not found", path);
        return nullptr;
    }
    logger->info("Loading circuit from file \"{}\"", path);

    std::string id = j["id"].get<std::string>();
    float x = j["position"]["x"].get<float>();
    float y = j["position"]["y"].get<float>();
    auto circuit = std::make_unique<CompositeCircuit>(id, type, new TextView(font, type, {x, y}));

    json data = json::parse(file);

    circuit->setInputCount(data["inputs"].size());
    circuit->setOutputCount(data["outputs"].size());

    logger->debug("Loading child circuits for composite of type \"{}\"", type);
    for (auto elem : data["elements"]) {
        std::string type = elem["type"].get<std::string>();
        Circuit* inner = loader.read(type, elem);
        if (!inner) {
            logger->warn("Failed to load child circuit of type \"{}\"", type);
            continue;
        }
        circuit->addCircuit(inner);
    }

    logger->debug("Loading wires for composite of type \"{}\"", type);
    for (auto w : data["wires"]) {
        std::string froms, tos;
        w["from"].get_to(froms);
        w["to"].get_to(tos);
        if (!circuit->connectPins(froms, tos)) {
            logger->warn("Failed to load wire {} -> {}", froms, tos);
        }
    }

    return circuit.release();
}
} // namespace serde
