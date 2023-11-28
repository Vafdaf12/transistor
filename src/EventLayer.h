#pragma once

#include <functional>
#include <list>


template<typename Key, typename Event>
class EventLayer {
public:
    using Handler = std::function<bool(const Event&)>;

    bool handle(const Key& key, const Event& event) {
        auto it = _handlers.find(key);
        if(it == _handlers.end()) return false;

        bool result = false;
        for(auto& f : it->second) {
            if(f(event)) {
                result = true;
            }
        }
        return result;
    }
    void subscribe(const Key& key, Handler&& h) {
        _handlers[key].push_back(h);
    }

private:
    std::unordered_map<Key, std::list<Handler>> _handlers;
};