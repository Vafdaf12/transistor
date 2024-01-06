#pragma once

#include <unordered_map>

#include "json.hpp"
using json = nlohmann::json;

template <typename Asset, typename Key>
class AssetLoader {
public:
    using Marshaller = std::function<Asset*(const json&)>;
    using DefaultMarshaller = std::function<Asset*(const Key&, const json&)>;

    Asset* read(const Key& k, const json& j) const {
        auto it = _marshallers.find(k);
        if (it != _marshallers.end()) {
            return it->second(j);
        }
        if (m_default) {
            return m_default(k, j);
        }
        return nullptr;
    }

    void addType(const Key& key, Marshaller&& c) { _marshallers[key] = c; }
    void setDefault(DefaultMarshaller&& m) { m_default = m; }

private:
    std::unordered_map<Key, Marshaller> _marshallers;
    DefaultMarshaller m_default;
};
