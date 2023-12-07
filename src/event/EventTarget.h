#pragma once

#include "SFML/Window/Event.hpp"
#include <functional>
#include <unordered_map>

class EventTarget {
public:
    using Handler = std::function<void(const sf::Event&)>;

    void bind(const sf::Event::EventType& key, Handler&& fn) {
        _handlers[key] = fn;
    }
    bool unbind(const sf::Event::EventType& key) {
        return _handlers.erase(key) > 0;
    }

    bool post(const sf::Event& event) const {
        for (auto& [key, fn] : _handlers) {
            if(key == event.type) {
                fn(event);
                return true;
            }
        }
        return false;
    }

private:
    std::unordered_map<sf::Event::EventType, Handler> _handlers;
};