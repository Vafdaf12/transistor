#pragma once

#include "circuit/Circuit.h"

#include <unordered_map>
#include <string>
#include <functional>

#include "json.hpp"
using json = nlohmann::json;

template<typename Key = std::string>
class CircuitRegistry {
public:
    using Creator = std::function<Circuit*(const json&)>;

    Circuit* create(const Key& k, const json& j) const {
        auto it = _creators.find(k);
        if(it == _creators.end()) {
            return nullptr;
        }
        return it->second(j);
    }

    void add(const Key& key, Creator&& c) {
        _creators[key] = c;
    }
private:
    std::unordered_map<Key, Creator> _creators;
};
