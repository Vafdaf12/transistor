#include "Circuit.h"
#include "SFML/Graphics/RenderTarget.hpp"

Circuit::Circuit(size_t numInputs, size_t numOutputs, sf::Vector2f pos) {
    size_t maxPins = std::max(numInputs, numOutputs);
    const float totalHeight = 2 * Pin::RADIUS + PADDING;

    _shape.setPosition(pos);
    _shape.setSize({WIDTH, totalHeight * maxPins + PADDING});

    _inputs.clear();
    float inputStart = (_shape.getSize().y - totalHeight * numInputs) / 2.f;
    for (size_t i = 0; i < numInputs; i++) {

        sf::Vector2f pinPos = pos + sf::Vector2f(0, inputStart + totalHeight * i);
        _inputs.emplace_back(Pin::Input, pinPos);
    }

    _outputs.clear();
    float outputStart = (_shape.getSize().y - (2 * Pin::RADIUS + PADDING) * numOutputs) / 2.f;
    for (size_t i = 0; i < numOutputs; i++) {
        sf::Vector2f pinPos = pos + sf::Vector2f(WIDTH, outputStart + totalHeight * i);
        _outputs.emplace_back(Pin::Output, pinPos);
    }
}

bool Circuit::collide(sf::Vector2f p) const {
    return _shape.getGlobalBounds().contains(p);
}

const Pin* Circuit::collidePin(sf::Vector2f v) const {
    for (auto& p : _inputs) {
        if (p.collide(v)) {
            return &p;
        }
    }
    for (auto& p : _outputs) {
        if (p.collide(v)) {
            return &p;
        }
    }
    return nullptr;
}

std::vector<sf::Transformable*> Circuit::getTransforms() {
    std::vector<sf::Transformable*> components;
    for (auto& p : _inputs) {
        components.push_back(&p.getTransform());
    }
    for (auto& p : _outputs) {
        components.push_back(&p.getTransform());
    }
    components.push_back(&_shape);
    return components;
}

sf::FloatRect Circuit::getBoundingBox() const {
    return _shape.getGlobalBounds();
}


void Circuit::draw(sf::RenderTarget& target, sf::RenderStates) const {
    target.draw(_shape);
    for(const auto& p : _inputs) {
        target.draw(p);
    }
    for(const auto& p : _outputs) {
        target.draw(p);
    }
}

void Circuit::setColor(sf::Color color) {
    _shape.setFillColor(color);
}
