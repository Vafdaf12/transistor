#include "CompositeCircuit.h"
#include "SFML/Graphics/Rect.hpp"
#include <algorithm>
#include <iterator>

CompositeCircuit::CompositeCircuit(const std::string& id, const sf::Font& font, sf::Vector2f pos)
    : Circuit(id), m_label(id, font) {

    m_label.setFillColor(sf::Color::Red);
    m_shape.setFillColor(sf::Color::White);
    setLabel(id);
    setPosition(pos);
}

CompositeCircuit::CompositeCircuit(const CompositeCircuit& other) : Circuit(other.getId()) {
    m_label = other.m_label;
    m_shape = other.m_shape;

    m_inputs = other.m_inputs;
    m_outputs = other.m_outputs;

    std::transform(
        other.m_circuits.begin(),
        other.m_circuits.end(),
        std::back_inserter(m_circuits),
        [](const auto& c) { return std::unique_ptr<Circuit>(c->clone(c->getId())); }
    );

    std::transform(
        other.m_wires.begin(),
        other.m_wires.end(),
        std::back_inserter(m_wires),
        [this](const Wire& w) {
            return Wire(queryPin(w.getFrom()->getFullPath()), queryPin(w.getTo()->getFullPath()));
        }
    );
}
bool CompositeCircuit::collide(sf::Vector2f p) const {
    return m_shape.getGlobalBounds().contains(p);
}

template <typename T>
T* value_ptr(T& v) {
    return &v;
}

std::vector<Pin*> CompositeCircuit::getAllPins() {
    std::vector<Pin*> pins;
    pins.reserve(m_inputs.size() + m_outputs.size());

    auto inserter = std::back_inserter(pins);
    std::transform(m_inputs.begin(), m_inputs.end(), inserter, value_ptr<Pin>);
    std::transform(m_outputs.begin(), m_outputs.end(), inserter, value_ptr<Pin>);
    return pins;
}

sf::FloatRect CompositeCircuit::getBoundingBox() const { return m_shape.getGlobalBounds(); }
sf::Vector2f CompositeCircuit::getPosition() const { return m_shape.getPosition(); }
void CompositeCircuit::setPosition(sf::Vector2f pos) {
    m_shape.setPosition(pos);
    m_label.setPosition(pos + sf::Vector2f(PADDING, PADDING));
}

Circuit* CompositeCircuit::clone(const std::string& id) const {
    Circuit* c = new CompositeCircuit(*this);
    c->setId(id);
    return c;
}

void CompositeCircuit::toJson(nlohmann::json& j) const { j["type"] = "composite"; }

void CompositeCircuit::update(const sf::RenderWindow&, float dt) {
    size_t count = std::min(m_inputs.size(), m_outputs.size());
    auto in = m_inputs.begin();
    auto out = m_outputs.begin();

    for (size_t i = 0; i < count; i++, in++, out++) {
        out->setValue(in->getValue());
    }
}
void CompositeCircuit::draw(sf::RenderWindow& w) const {
    w.draw(m_shape);
    w.draw(m_label);
}

Pin* CompositeCircuit::queryPin(const std::string& path) {
    size_t i = path.find('/');
    if (i == std::string::npos) {
        for (auto& p : m_inputs) {
            if (path == p.getId()) {
                return &p;
            }
        }
        for (auto& p : m_outputs) {
            if (path == p.getId()) {
                return &p;
            }
        }
        return nullptr;
    }

    std::string circuit = path.substr(0, i);
    std::string pin = path.substr(i + 1);

    Circuit* c = queryCircuit(circuit);
    if (!c) {
        return nullptr;
    }
    std::vector<Pin*> pins = c->getAllPins();
    auto it =
        std::find_if(pins.begin(), pins.end(), [&pin](const Pin* p) { return p->getId() == pin; });
    return it == pins.end() ? nullptr : *it;
}

Circuit* CompositeCircuit::queryCircuit(const std::string& path) {
    for (const auto& c : m_circuits) {
        if (c->getId() == path) {
            return c.get();
        }
    }
    return nullptr;
}
void CompositeCircuit::setLabel(const std::string& label) {
    m_label.setString(label);
    m_shape.setSize(
        {m_label.getGlobalBounds().width + 2 * PADDING, m_label.getCharacterSize() + 2 * PADDING}
    );
}
