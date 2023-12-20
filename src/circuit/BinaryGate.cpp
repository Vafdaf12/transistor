#include "BinaryGate.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

BinaryGate::BinaryGate(const std::string& id, const Assets& assets, Func fn, sf::Vector2f pos)
    : Circuit(id), _in1("in1", Pin::Input), _in2("in2", Pin::Input),
      _assets(assets), _out("out", Pin::Output, {0, 0}, 0), _process(fn) {
    if (fn == And) {
        _sprite.setTexture(assets.textures.get("gate_and"));
    } else if (fn == Xor) {
        _sprite.setTexture(assets.textures.get("gate_xor"));
    } else if (fn == Or) {
        _sprite.setTexture(assets.textures.get("gate_or"));
    }

    _in1.setParent(this);
    _in2.setParent(this);
    _out.setParent(this);

    _sprite.scale(0.7, 0.7);

    setPosition(pos);

    _in1.connect(_flags);
    _in2.connect(_flags+1);
}

BinaryGate::~BinaryGate() {
    _in1.disconnect(_flags);
    _in2.disconnect(_flags + 1);
}
bool BinaryGate::collide(sf::Vector2f v) const { return _sprite.getGlobalBounds().contains(v); }

Pin* BinaryGate::collidePin(sf::Vector2f v) {
    if (_in1.collide(v))
        return &_in1;
    if (_in2.collide(v))
        return &_in2;
    if (_out.collide(v))
        return &_out;
    return nullptr;
}
sf::Vector2f BinaryGate::getPosition() const {
    return _sprite.getPosition();
}
void BinaryGate::setPosition(sf::Vector2f pos) {
    _sprite.setPosition(pos);
    sf::Vector2f size = _sprite.getGlobalBounds().getSize();

    _out.setCenter(pos + sf::Vector2f(size.x, size.y / 2.f));

    size.y /= 3;
    _in1.setCenter(pos + sf::Vector2f(0, size.y * 1.f));
    _in2.setCenter(pos + sf::Vector2f(0, size.y * 2.f));
}

sf::FloatRect BinaryGate::getBoundingBox() const { return _sprite.getGlobalBounds(); }

BinaryGate* BinaryGate::clone(const std::string& newId) const {
    BinaryGate* c = new BinaryGate(newId, _assets, _process, _sprite.getPosition());
    return c;
}

void BinaryGate::update(const sf::RenderWindow& w, float dt) {
    if(_flags[0] == Pin::Dirty || _flags[1] == Pin::Dirty) {
        _out.setValue(_process(_in1.getValue(), _in2.getValue()));
        _flags[0] = Pin::None;
        _flags[1] = Pin::None;
    }
    _out.update(w, dt);
    _in1.update(w, dt);
    _in2.update(w, dt);
}

void BinaryGate::draw(sf::RenderWindow& window) const {
    window.draw(_sprite);
    _in1.draw(window);
    _in2.draw(window);
    _out.draw(window);
}

Pin* BinaryGate::queryPin(const std::string& id) {
    if (_in1.getId() == id)
        return &_in1;
    if (_in2.getId() == id)
        return &_in2;
    if (_out.getId() == id)
        return &_out;
    return nullptr;
}

int BinaryGate::And(int x, int y) { return x && y; }
int BinaryGate::Or(int x, int y) { return x || y; }
int BinaryGate::Nand(int x, int y) { return !And(x, y); }
int BinaryGate::Nor(int x, int y) { return !Or(x, y); }
int BinaryGate::Xor(int x, int y) { return x ^ y; }
