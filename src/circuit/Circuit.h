#pragma once

#include "Pin.h"
#include "core/Entity.h"
#include "view/CircuitView.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "json.hpp"
#include <string>

class Circuit : public core::Entity<std::string> {
public:
    Circuit(const std::string& id, CircuitView* view) : Entity(id), m_view(view) {}

    inline bool collide(sf::Vector2f p) const { return m_view->collide(p); }
    inline sf::FloatRect getBoundingBox() const { return m_view->getBoundingBox(); }
    inline sf::Vector2f getPosition() const { return m_view->getPosition(); }

    virtual void setPosition(sf::Vector2f p) { m_view->setPosition(p); }
    virtual void draw(sf::RenderWindow& window) const override { m_view->draw(window); }

    /**
     * @return std::vector<Pin*> all pins in the circuit (excluding any sub-circuits)
     */
    virtual std::vector<Pin*> getAllPins() = 0;

    /**
     * @brief Clones the circuit with a new identifier
     *
     * @param id New circuit identifier
     * @return Circuit*
     */
    virtual Circuit* clone(const std::string& id) const = 0;

    void onEvent(const sf::RenderWindow&, const sf::Event&) override {}

    virtual void toJson(nlohmann::json& j) const = 0;

protected:
    std::unique_ptr<CircuitView> m_view;
};
