#include "NandCircuit.h"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/System/Vector2.hpp"

NandCircuit::NandCircuit(const sf::Font& font, sf::Vector2f pos)
    : _text("NAND", font), _in1(Pin::Input), _in2(Pin::Input), _out(Pin::Output, {0, 0}, 1) {
    const sf::Vector2f pad = sf::Vector2f(PADDING, PADDING);
    _text.setStyle(sf::Text::Bold);

    _shape.setPosition(pos);
    _text.setPosition(pos + pad);

    _shape.setFillColor(sf::Color(0xe97531ff));
    _text.setFillColor(sf::Color::White);

    sf::Vector2f size = _text.getGlobalBounds().getSize();
    size.x += 2 * PADDING;
    size.y = _text.getCharacterSize() + 2 * PADDING;

    _shape.setSize(size);

    _out.setCenter(pos + sf::Vector2f(size.x, size.y / 2.f));

    size.y /= 4;
    _in1.setCenter(pos + sf::Vector2f(0, size.y * 1.f));
    _in2.setCenter(pos + sf::Vector2f(0, size.y * 3.f));

    _in1.connect(this);
    _in2.connect(this);
}
bool NandCircuit::collide(sf::Vector2f v) const { return _shape.getGlobalBounds().contains(v); }

Pin* NandCircuit::collidePin(sf::Vector2f v) {
    if (_in1.collide(v))
        return &_in1;
    if (_in2.collide(v))
        return &_in2;
    if (_out.collide(v))
        return &_out;
    return nullptr;
}

std::vector<sf::Transformable*> NandCircuit::getTransforms() {
    return {&_text, &_shape, &_in1.getTransform(), &_in2.getTransform(), &_out.getTransform()};
}

sf::FloatRect NandCircuit::getBoundingBox() const { return _shape.getGlobalBounds(); }

NandCircuit* NandCircuit::clone() {
    NandCircuit* c = new NandCircuit(*_text.getFont(), _shape.getPosition());
    return c;
}

void NandCircuit::draw(sf::RenderTarget& target, sf::RenderStates) const {
    target.draw(_shape);
    target.draw(_text);
    target.draw(_in1);
    target.draw(_in2);
    target.draw(_out);
}

void NandCircuit::update(Pin* pin) { _out.setState(!(_in1.getState() && _in2.getState())); }