#include "CompositeCircuit.h"

#include "SFML/Graphics/Rect.hpp"

#include <algorithm>
#include <iterator>
#include <ranges>
#include <vector>

#include "spdlog/spdlog.h"
#include "util/util.h"

CompositeCircuit::CompositeCircuit(
    const std::string& id, const std::string& type, CircuitView* view
)
    : Circuit(id, view), m_type(type) {

    m_logger = spdlog::get("editor");
    assert(m_logger);
    layout();
}

CompositeCircuit::CompositeCircuit(const CompositeCircuit& other) : Circuit(other.getId(), other.m_view->clone()) {
    m_type = other.m_type;

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

            Pin* source = nullptr;
            if(w.getFrom()->getParent() == this) {
                source = queryPin(w.getFrom()->getFullPath());
            } else {
                source = queryPin(w.getFrom()->getId());
            }

            Pin* target = nullptr;
            if(w.getTo()->getParent() == this) {
                target = queryPin(w.getTo()->getFullPath());
            } else {
                target = queryPin(w.getTo()->getId());
            }

            return Wire(source, target);
        }
    );
}

std::vector<Pin*> CompositeCircuit::getAllPins() {
    std::vector<Pin*> pins;
    pins.reserve(m_inputs.size() + m_outputs.size());

    auto inserter = std::back_inserter(pins);
    std::transform(m_inputs.begin(), m_inputs.end(), inserter, util::value_ptr<Pin>);
    std::transform(m_outputs.begin(), m_outputs.end(), inserter, util::value_ptr<Pin>);
    return pins;
}

void CompositeCircuit::setPosition(sf::Vector2f pos) {
    sf::Vector2f diff = pos - m_view->getPosition();
    m_view->setPosition(pos);
    for (Pin& pin : m_inputs) {
        pin.setPosition(pin.getPosition() + diff);
    }
    for (Pin& pin : m_outputs) {
        pin.setPosition(pin.getPosition() + diff);
    }
}

Circuit* CompositeCircuit::clone(const std::string& id) const {
    Circuit* c = new CompositeCircuit(*this);
    c->setId(id);
    return c;
}

void CompositeCircuit::toJson(nlohmann::json& j) const {
    j["id"] = getId();
    j["type"] = m_type;
    sf::Vector2f pos = getPosition();
    j["position"]["x"] = pos.x;
    j["position"]["y"] = pos.y;
}

void CompositeCircuit::update(const sf::RenderWindow& w, float dt) {
    for (auto& p : m_wires) {
        p.update(w, dt);
    }
    for (auto& p : m_inputs) {
        p.update(w, dt);
    }
    for (auto& p : m_outputs) {
        p.update(w, dt);
    }
    for (auto& c : m_circuits) {
        c->update(w, dt);
    }

    /*
    size_t count = std::min(m_inputs.size(), m_outputs.size());
    auto in = m_inputs.begin();
    auto out = m_outputs.begin();

    for (size_t i = 0; i < count; i++, in++, out++) {
        out->setValue(in->getValue());

    }
    */
}
void CompositeCircuit::draw(sf::RenderWindow& w) const {
    m_view->draw(w);
    for (const Pin& p : m_inputs) {
        p.draw(w);
    }
    for (const Pin& p : m_outputs) {
        p.draw(w);
    }
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

void CompositeCircuit::setInputCount(size_t count) {
    if (count > m_inputs.size()) {
        count -= m_inputs.size();
        for (size_t i = 0; i < count; i++) {
            m_inputs.emplace_back("in" + std::to_string(m_inputs.size()), Pin::Input);
            m_inputs.back().setParent(this);
            m_rootPins.insert(&m_inputs.back());
        }
        layout();
    } else if (count < m_inputs.size()) {
        auto view =
            m_inputs | std::views::drop(count) | std::views::transform(util::value_ptr<Pin>);
        std::set<const Pin*> pins;
        std::copy(view.begin(), view.end(), std::inserter(pins, pins.begin()));
        std::erase_if(m_wires, [&pins](const Wire& w) { return pins.contains(w.getFrom()); });

        for (size_t i = 0; i < m_inputs.size() - count; i++) {
            m_rootPins.erase(&m_inputs.back());
            m_inputs.pop_back();
        }
        layout();
    }
}
void CompositeCircuit::setOutputCount(size_t count) {
    if (count > m_outputs.size()) {
        count -= m_outputs.size();
        for (size_t i = 0; i < count; i++) {
            m_outputs.emplace_back("out" + std::to_string(m_outputs.size()), Pin::Output);
            m_outputs.back().setParent(this);
            m_rootPins.insert(&m_outputs.back());
        }
        layout();
    } else if (count < m_outputs.size()) {
        auto view =
            m_outputs | std::views::drop(count) | std::views::transform(util::value_ptr<Pin>);
        std::set<const Pin*> pins;
        std::copy(view.begin(), view.end(), std::inserter(pins, pins.begin()));
        std::erase_if(m_wires, [&pins](const Wire& w) { return pins.contains(w.getTo()); });

        for (size_t i = 0; i < m_outputs.size() - count; i++) {
            m_rootPins.erase(&m_outputs.back());
            m_outputs.pop_back();
        }
        layout();
    }
}

bool CompositeCircuit::connectPins(const std::string& id1, const std::string& id2) {
    Pin* p1 = queryPin(id1);
    Pin* p2 = queryPin(id2);
    if (!p1) {
        m_logger->warn("Failed to connect pins {} -> {}: {} not found", id1, id2, id1);
        return false;
    }
    if (!p2) {
        m_logger->warn("Failed to connect pins {} -> {}: {} not found", id1, id2, id2);
        return false;
    }
    m_wires.emplace_back(p1, p2);
    m_logger->debug("Connected pins {} -> {}", id1, id2);
    return true;
}
bool CompositeCircuit::isInteriorPin(const Pin& pin) const {
    return !m_rootPins.contains(&pin);
}
bool CompositeCircuit::addCircuit(Circuit* circuit) {
    if (!circuit)
        return false;
    if (queryCircuit(circuit->getId())) {
        return false;
    }
    m_circuits.emplace_back(circuit);
    return true;
}
void CompositeCircuit::layout() {
    size_t count = util::max(m_inputs.size(), m_outputs.size());

    sf::Vector2f minSize = m_view->getMinimumSize();
    sf::Vector2f size = minSize;
    size.x += 2 * (PADDING+Pin::RADIUS);
    size.y = count * (2 * Pin::RADIUS + PADDING) + PADDING;
    size.y = util::max<float>(minSize.y + 2 * PADDING, size.y);

    m_view->setSize(size);

    // Layout input pins
    float pinStart = m_inputs.size() * (2 * Pin::RADIUS + PADDING) - PADDING;
    pinStart = Pin::RADIUS + m_view->getPosition().y + (size.y - pinStart) / 2;
    for (Pin& pin : m_inputs) {
        pin.setPosition({m_view->getPosition().x, pinStart});
        pinStart += 2 * Pin::RADIUS + PADDING;
    }

    // Layout output pins
    pinStart = m_outputs.size() * (2 * Pin::RADIUS + PADDING) - PADDING;
    pinStart = Pin::RADIUS + m_view->getPosition().y + (size.y - pinStart) / 2;
    for (Pin& pin : m_outputs) {
        pin.setPosition({m_view->getPosition().x + m_view->getBoundingBox().width, pinStart});
        pinStart += 2 * Pin::RADIUS + PADDING;
    }
}
