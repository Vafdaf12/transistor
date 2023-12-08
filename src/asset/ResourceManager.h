#pragma once

#include <unordered_map>
template <typename Key, typename Res>
class ResourceManager {
public:
    ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    template <typename... Args>
    bool load(const Key& key, Args&&... args);

    const Res& get(const Key& key) const;

private:
    std::unordered_map<Key, Res> _resources;
};

#include "ResourceManager.inl"
