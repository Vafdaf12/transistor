#pragma once

#include <list>
#include <memory>
#include <string>

#include "SFML/Graphics/VertexArray.hpp"
#include "SFML/Graphics/View.hpp"
#include "json.hpp"

#include "circuit/Pin.h"
#include "circuit/Wire.h"
#include "spdlog/logger.h"
#include "tools/Tool.h"
#include "util/DragBoard.h"

class CircuitEditor {
public:
    CircuitEditor(const sf::View& screen);

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

    bool addInput(const std::string& id);
    bool removeInput(Pin* pin);

    bool addOutput(const std::string& id);
    bool removeOutput(Pin* pin);

    bool addCircuit(Circuit* c);
    bool removeCircuit(const Circuit* c);

    bool addWire(Pin* from, Pin* to);
    bool removeWire(Pin* from, Pin* to = nullptr);
    Wire* getWire(Pin* from, Pin* to = nullptr);

    Pin* collidePin(const sf::RenderWindow&, sf::Vector2i pos, bool worldOnly = false);
    Circuit* collideCircuit(sf::Vector2f pos);
    std::vector<Circuit*> collideCircuit(sf::FloatRect rect);

    void onEvent(const sf::RenderWindow&, const sf::Event&);
    void update(const sf::RenderWindow&, float dt);
    void draw(sf::RenderWindow&) const;

    inline const sf::View& getWorldView() { return m_cameraView; }

    void toJson(nlohmann::json&) const;

private:
    void removeConnectedWires(const Pin* pin);
    void layoutPins();

    std::string getInputId(const std::string& base) const;
    std::string getOutputId(const std::string& base) const;

    sf::View _screenSpace;
    sf::View m_cameraView;

    sf::VertexArray _grid;

    // NOTE: Lists are used here to preserve the consistency of flag pointers
    // between wires and pins, since a vector does it's whole "resize-and-copy"
    // maneuver, inherently invalidating the pointers
    std::list<Wire> _wires;
    std::list<Pin> _inputs;
    std::list<Pin> _outputs;

    std::vector<std::unique_ptr<Circuit>> _circuits;

    std::unique_ptr<Tool> m_tools;
    DragBoard _board;

    std::shared_ptr<spdlog::logger> m_logger;
};
