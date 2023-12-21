#pragma once

#include <string>

#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/Graphics/View.hpp"
#include "core/Entity.h"
#include "pin/Pin.h"
#include "pin/Wire.h"

class CircuitEditor : public core::Entity {
public:
    CircuitEditor(const sf::View& screen, const sf::View& world);

    /**
    * @brief Computes an available circuit ID
    * 
    * @param base The base id to work from when computing the ID
    * @return std::string An available ID
    */
    std::string getCircuitId(const std::string& base) const;

    /**
    * @brief Attempts to find a pin in the editor, searching both circuits
    * and input pins
    * 
    * @param path The path to the pin
    * @return Pin* The found pin (or null if not found)
    */
    Pin* queryPin(const std::string& path);

    /**
    * @brief Attempts to find a circuit in the editor
    * 
    * @param id The id of the circuit
    * @return Circuit* The found circuit (or null if not found)
    */
    Circuit* queryCircuit(const std::string& id);

    // TODO:
    // - Finish implementing this
    // - Figure out how to handle viewports and resizing
    // - Integrate relevant tools (connecting, panning)
    // - Refactor tools to not be dependant on view

    bool addInput(const std::string& id);
    bool removeInput(Pin* pin);

    bool addOutput(const std::string& id);
    bool removeOutput(Pin* pin);

    bool addCircuit(Circuit* c);
    bool removeCircuit(Circuit* c);

    bool addWire(Pin* from, Pin* to);
    bool removeWire(Pin* from, Pin* to = nullptr);

    Pin* collidePin(const sf::RenderWindow&, sf::Vector2i pos, bool worldOnly = true);

    void onEvent(const sf::RenderWindow&, const sf::Event&) override;
    void update(const sf::RenderWindow&, float dt) override;
    void draw(sf::RenderWindow&) const override;

    inline sf::View& getWorldView() {return _worldSpace; }

private:
    void layoutPins();
    void updateWires();

    sf::View _screenSpace;
    sf::View _worldSpace;

    sf::VertexArray _grid;


    std::vector<Wire> _wires;
    std::vector<Pin> _inputs;
    std::vector<Pin> _outputs;
    std::vector<std::unique_ptr<Circuit>> _circuits;
};
