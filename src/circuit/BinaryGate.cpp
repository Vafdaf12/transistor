#include "BinaryGate.h"

#include "SFML/Graphics/Texture.hpp"

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/System/Vector2.hpp"

BinaryGate::BinaryGate(const std::string& id, const sf::Texture& tex, Func fn, sf::Vector2f pos)
    : Circuit(id), _sprite(tex), _in1("in1", Pin::Input), _in2("in2", Pin::Input),
      _out("out", Pin::Output, {0, 0}, 0), _process(fn) {

    _sprite.setPosition(pos);
    _sprite.scale(0.7, 0.7);

    sf::Vector2f size = _sprite.getGlobalBounds().getSize();

    _out.setCenter(pos + sf::Vector2f(size.x, size.y / 2.f));

    size.y /= 3;
    _in1.setCenter(pos + sf::Vector2f(0, size.y * 1.f));
    _in2.setCenter(pos + sf::Vector2f(0, size.y * 2.f));

    _in1.connect(this);
    _in2.connect(this);
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

std::vector<sf::Transformable*> BinaryGate::getTransforms() {
    return {&_sprite, &_sprite, &_in1.getTransform(), &_in2.getTransform(), &_out.getTransform()};
}

sf::FloatRect BinaryGate::getBoundingBox() const { return _sprite.getGlobalBounds(); }

BinaryGate* BinaryGate::clone(const std::string& newId) {
    BinaryGate* c = new BinaryGate(newId, *_sprite.getTexture(), _process, _sprite.getPosition());
    return c;
}

void BinaryGate::draw(sf::RenderTarget& target, sf::RenderStates) const {
    target.draw(_sprite);
    target.draw(_in1);
    target.draw(_in2);
    target.draw(_out);
}

void BinaryGate::update(Pin* pin) { _out.setState(_process(_in1.getState(), _in2.getState())); }

int BinaryGate::And(int x, int y) { return x && y; }
int BinaryGate::Or(int x, int y) { return x || y; }
int BinaryGate::Nand(int x, int y) { return !And(x, y); }
int BinaryGate::Nor(int x, int y) { return !Or(x, y); }
int BinaryGate::Xor(int x, int y) { return x ^ y; }
