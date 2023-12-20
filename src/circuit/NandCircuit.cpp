#include "NandCircuit.h"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

NandCircuit::NandCircuit(const std::string& id, const Assets& assets, sf::Vector2f pos)
    : Circuit(id), _text("NAND", assets.fonts.get("default")), _in1("in1", Pin::Input),
      _in2("in2", Pin::Input), _out("out", Pin::Output, {0, 0}, 1), _assets(assets) {
    _text.setStyle(sf::Text::Bold);

    _in1.setParent(this);
    _in2.setParent(this);
    _out.setParent(this);

    _shape.setFillColor(sf::Color(0xe97531ff));
    _text.setFillColor(sf::Color::White);
    setPosition(pos);

    _in1.connect(_flags);
    _in2.connect(_flags + 1);
}
NandCircuit::~NandCircuit() {
    _in1.disconnect(_flags);
    _in2.disconnect(_flags + 1);
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

sf::Vector2f NandCircuit::getPosition() const {
    return _shape.getPosition();
}
void NandCircuit::setPosition(sf::Vector2f pos) {
    static const sf::Vector2f pad = sf::Vector2f(PADDING, PADDING);

    _shape.setPosition(pos);
    _text.setPosition(pos + pad);

    sf::Vector2f size = _text.getGlobalBounds().getSize();
    size.x += 2 * PADDING;
    size.y = _text.getCharacterSize() + 2 * PADDING;

    _shape.setSize(size);

    _out.setCenter(pos + sf::Vector2f(size.x, size.y / 2.f));

    size.y /= 4;
    _in1.setCenter(pos + sf::Vector2f(0, size.y * 1.f));
    _in2.setCenter(pos + sf::Vector2f(0, size.y * 3.f));
}

sf::FloatRect NandCircuit::getBoundingBox() const { return _shape.getGlobalBounds(); }

NandCircuit* NandCircuit::clone(const std::string& newId) const {
    NandCircuit* c = new NandCircuit(newId, _assets, _shape.getPosition());
    return c;
}

void NandCircuit::draw(sf::RenderWindow& window) const {
    window.draw(_shape);
    window.draw(_text);
    _in1.draw(window);
    _in2.draw(window);
    _out.draw(window);
}

void NandCircuit::update(const sf::RenderWindow& w, float dt) {
    if(_flags[0] == Pin::Dirty || _flags[1] == Pin::Dirty) {
        _out.setValue(!(_in1.getValue() && _in2.getValue()));
        _flags[0] = Pin::None;
        _flags[1] = Pin::None;
    }
    _out.update(w, dt);
    _in1.update(w, dt);
    _in2.update(w, dt);
}


Pin* NandCircuit::queryPin(const std::string& id) {
    if (_in1.getId() == id)
        return &_in1;
    if (_in2.getId() == id)
        return &_in2;
    if (_out.getId() == id)
        return &_out;
    return nullptr;
}
