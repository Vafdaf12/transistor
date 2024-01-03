#include "BinaryGate.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"

BinaryGate::BinaryGate(
    const std::string& id,
    const ResourceManager<BinaryGate::Func, sf::Texture>& textures,
    Func fn,
    sf::Vector2f pos
)
    : Circuit(id), _in1("in1", Pin::Input), _in2("in2", Pin::Input), _textures(textures),
      _out("out", Pin::Output, {0, 0}, 0), _process(fn) {
    _sprite.setTexture(textures.get(fn));

    _in1.setParent(this);
    _in2.setParent(this);
    _out.setParent(this);

    _sprite.scale(0.7, 0.7);

    setPosition(pos);

    _in1.connect(_flags);
    _in2.connect(_flags + 1);
}

BinaryGate::~BinaryGate() {
    _in1.disconnect(_flags);
    _in2.disconnect(_flags + 1);
}
bool BinaryGate::collide(sf::Vector2f v) const { return _sprite.getGlobalBounds().contains(v); }

sf::Vector2f BinaryGate::getPosition() const { return _sprite.getPosition(); }
void BinaryGate::setPosition(sf::Vector2f pos) {
    _sprite.setPosition(pos);
    sf::Vector2f size = _sprite.getGlobalBounds().getSize();

    _out.setPosition(pos + sf::Vector2f(size.x, size.y / 2.f));

    size.y /= 4;
    _in1.setPosition(pos + sf::Vector2f(0, size.y * 1.f));
    _in2.setPosition(pos + sf::Vector2f(0, size.y * 3.f));
}

sf::FloatRect BinaryGate::getBoundingBox() const { return _sprite.getGlobalBounds(); }

BinaryGate* BinaryGate::clone(const std::string& newId) const {
    BinaryGate* c = new BinaryGate(newId, _textures, _process, _sprite.getPosition());
    return c;
}

void BinaryGate::update(const sf::RenderWindow& w, float dt) {
    if (_flags[0] == Pin::Dirty || _flags[1] == Pin::Dirty) {
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

int BinaryGate::And(int x, int y) { return x && y; }
int BinaryGate::Or(int x, int y) { return x || y; }
int BinaryGate::Nand(int x, int y) { return !And(x, y); }
int BinaryGate::Nor(int x, int y) { return !Or(x, y); }
int BinaryGate::Xor(int x, int y) { return x ^ y; }

void BinaryGate::toJson(nlohmann::json& j) const {
    const static std::unordered_map<Func, std::string> mapping = {
        {And, "and_gate"},
        {Or, "or_gate"},
        {Nand, "nand_gate"},
        {Nor, "nor_gate"},
        {Xor, "xor_gate"},
    };

    j["id"] = getId();
    j["type"] = mapping.at(_process);
    j["position"] = {
        {"x", _sprite.getPosition().x },
        {"y", _sprite.getPosition().y },
    };
}
