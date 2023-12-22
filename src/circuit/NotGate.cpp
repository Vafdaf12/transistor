#include "NotGate.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

NotGate::NotGate(const std::string& id, const Assets& assets, sf::Vector2f pos)
    : Circuit(id), _input("in", Pin::Input), _output("out", Pin::Output),
      _assets(assets) {
    _sprite.setTexture(assets.textures.get("gate_not"));

    _input.setParent(this);
    _output.setParent(this);

    _sprite.scale(0.7, 0.7);
    setPosition(pos);

    _input.connect(&_flag);
}
NotGate::~NotGate() {
    _input.disconnect(&_flag);
}
bool NotGate::collide(sf::Vector2f v) const { return _sprite.getGlobalBounds().contains(v); }

Pin* NotGate::collidePin(sf::Vector2f v) {
    if (_input.collide(v))
        return &_input;
    if (_output.collide(v))
        return &_output;
    return nullptr;
}

sf::Vector2f NotGate::getPosition() const {
    return _sprite.getPosition();
}
void NotGate::setPosition(sf::Vector2f pos) {
    _sprite.setPosition(pos);
    sf::Vector2f size = _sprite.getGlobalBounds().getSize();

    _output.setCenter(pos + sf::Vector2f(size.x, size.y / 2.f));
    _input.setCenter(pos + sf::Vector2f(0, size.y / 2.f));
}

sf::FloatRect NotGate::getBoundingBox() const { return _sprite.getGlobalBounds(); }

NotGate* NotGate::clone(const std::string& newId) const {
    NotGate* c = new NotGate(newId, _assets, _sprite.getPosition());
    return c;
}

void NotGate::update(const sf::RenderWindow& w, float dt) {
    if(_flag == Pin::Dirty) {
        _output.setValue(!_input.getValue());
    }
    _input.update(w, dt);
    _output.update(w, dt);
}

void NotGate::draw(sf::RenderWindow& window) const {
    window.draw(_sprite);
    _input.draw(window);
    _output.draw(window);
}

Pin* NotGate::queryPin(const std::string& id) {
    if (_input.getId() == id)
        return &_input;
    if (_output.getId() == id)
        return &_output;
    return nullptr;
}

void NotGate::toJson(nlohmann::json& j) const {
    j["id"] = getId();
    j["type"] = "not_gate";
    j["position"] = {
        {"x", _sprite.getPosition().x },
        {"y", _sprite.getPosition().y },
    };
}
