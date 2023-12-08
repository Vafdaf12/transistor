#pragma once

#include "ResourceManager.h"
#include <utility>

template <typename Key, typename Res>
template <typename... Args>
bool ResourceManager<Key, Res>::load(const Key& key, Args&&... args) {
    Res resource;
    if (!resource.loadFromFile(std::forward<Args>(args)...)) {
        return false;
    }
    _resources.emplace(key, std::move(resource));
    return true;
}

template <typename Key, typename Res>
const Res& ResourceManager<Key, Res>::get(const Key& key) const {
    return _resources.at(key);
}