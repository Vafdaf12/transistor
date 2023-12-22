#pragma once


#include <unordered_map>

#include "json.hpp"
using json = nlohmann::json;

template<typename Asset, typename Key>
class AssetContext {
public:
    using Marshaller = std::function<Asset*(const json&)>;

    Asset* read(const Key& k, const json& j) const {
        auto it = _marshallers.find(k);
        if(it == _marshallers.end()) {
            return nullptr;
        }
        return it->second(j);
    }


    void addType(const Key& key, Marshaller&& c) {
        _marshallers[key] = c;
    }

private:
    std::unordered_map<Key, Marshaller> _marshallers;
};
