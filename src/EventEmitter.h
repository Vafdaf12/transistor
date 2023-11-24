#pragma once

#include <functional>
#include <unordered_map>
#include <list>
#include "SFML/Window/Event.hpp"

class EventEmitter {
public:
    using Handler = std::function<void(const sf::Event&)>;
    using EventKey = sf::Event::EventType;

    void post(const sf::Event& event) {
        auto it = _handlers.find(event.type);
        if(it == _handlers.end()) return;
        for(auto& f : it->second) {
            f(event);
        }
    }
    void subscribe(EventKey key, Handler&& h) {
        _handlers[key].push_back(h);
    }

private:
    std::unordered_map<EventKey, std::list<Handler>> _handlers;
};