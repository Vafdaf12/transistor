#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <vector>

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/System/Vector2.hpp"
#include "asset/CircuitRegistry.h"
#include "circuit/Circuit.h"
#include "core/Entity.h"
#include "pin/Wire.h"

class GameWorld : public core::Entity {
public:
    enum PinTag {
        CIRCUIT = 1 << 0,
        SINGLE = 1 << 1,
    };
    bool loadFromFile(const std::string& path, const CircuitRegistry<std::string>& registry);
    bool saveToFile(const std::string& path);

    void addPin(Pin* p);

    void addCircuit(Circuit* c);

    void removeCircuit(Circuit* c);
    void removeWire(const Wire* w);
    const Wire* getConnectedWire(const Pin* pin);

    void connectPins(Pin* p1, Pin* p2);

    Pin* collidePin(sf::Vector2f pos, uint8_t filter = CIRCUIT | SINGLE);
    Pin* collidePin(const sf::RenderWindow& window, sf::Vector2i pos, uint8_t filter = CIRCUIT | SINGLE);

    bool isPinConnected(const Pin* pin) const;

    Circuit* collideCircuit(sf::Vector2f pos);
    std::vector<Circuit*> collideCircuit(sf::FloatRect rect);

    void onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float dt) override;
    void draw(sf::RenderWindow&) const override;

    Pin* queryPin(const std::string& path);
    Circuit* queryCircuit(const std::string& path);

    inline const sf::View& getScreenView() { return _guiView; }

    std::string assignCircuitId(const std::string& id) const;
private:
    void layoutPins(const sf::RenderWindow&);

    sf::View _guiView;

    std::list<Wire> _wires;
    std::vector<std::unique_ptr<Pin>> _inputs;
    std::vector<std::unique_ptr<Pin>> _outputs;
    std::vector<std::unique_ptr<Circuit>> _circuits;
};
