#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <vector>

#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"
#include "asset/AssetSystem.h"
#include "circuit/Circuit.h"
#include "pin/Wire.h"

class GameWorld : public sf::Drawable {
public:
    enum PinTag {
        CIRCUIT = 1 << 0,
        SINGLE = 1 << 1,
    };
    bool loadFromFile(const std::string& path, const Assets& assets);
    bool saveToFile(const std::string& path);

    void addPin(Pin* p);

    void addCircuit(Circuit* c);

    void removeCircuit(Circuit* c);

    void connectPins(Pin* p1, Pin* p2);

    Pin* collidePin(sf::Vector2f pos, uint8_t filter = CIRCUIT | SINGLE);

    Circuit* collideCircuit(sf::Vector2f pos);
    std::vector<Circuit*> collideCircuit(sf::FloatRect rect);

    void draw(sf::RenderTarget&, sf::RenderStates) const override;

    Pin* queryPin(const std::string& path);
    Circuit* queryCircuit(const std::string& path);

private:
    std::list<Wire> _wires;
    std::vector<std::unique_ptr<Pin>> _pins;
    std::vector<std::unique_ptr<Circuit>> _circuits;
};