#include "PassthroughCircuit.h"
#include "SFML/Graphics/RenderTarget.hpp"

PassthroughCircuit::PassthroughCircuit(size_t size, sf::Vector2f pos) {
    const float pinHeight = 2 * Pin::RADIUS + PADDING;
    const float totalHeight = pinHeight * size + PADDING;

    _shape.setPosition(pos);
    _shape.setSize({WIDTH, totalHeight});

    float startY = Pin::RADIUS + PADDING;
    for (size_t i = 0; i < size; i++) {
        float y = pos.y + startY + (2 * Pin::RADIUS + PADDING)*i;

        _outputs.emplace_back(Pin::Output, sf::Vector2f(pos.x + WIDTH, y));
        _inputs.emplace_back(Pin::Input, sf::Vector2f(pos.x, y));
        _inputs.back().connect(this);
    }

}

bool PassthroughCircuit::collide(sf::Vector2f p) const {
    return _shape.getGlobalBounds().contains(p);
}

Pin* PassthroughCircuit::collidePin(sf::Vector2f v) {
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

std::vector<sf::Transformable*> PassthroughCircuit::getTransforms() {
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

sf::FloatRect PassthroughCircuit::getBoundingBox() const {
    return _shape.getGlobalBounds();
}


void PassthroughCircuit::draw(sf::RenderTarget& target, sf::RenderStates) const {
    target.draw(_shape);
    for(const auto& p : _inputs) {
        target.draw(p);
    }
    for(const auto& p : _outputs) {
        target.draw(p);
    }
}

void PassthroughCircuit::setColor(sf::Color color) {
    _shape.setFillColor(color);
}

PassthroughCircuit* PassthroughCircuit::clone() {
    PassthroughCircuit* circuit = new PassthroughCircuit(_inputs.size(), _shape.getPosition());
    circuit->setColor(_shape.getFillColor());
    return circuit;
}

void PassthroughCircuit::update(Pin* pin) {
    size_t minPins = std::min(_outputs.size(), _inputs.size());
    for (size_t i = 0; i < minPins; i++) {
        _outputs[i].setState(_inputs[i].getState());
    }
}
