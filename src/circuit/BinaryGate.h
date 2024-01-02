#pragma once

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

#include "asset/ResourceManager.h"
#include "circuit/Circuit.h"
#include "circuit/Pin.h"

class BinaryGate : public Circuit {
public:
    using Func = int (*)(int, int);

    BinaryGate(
        const std::string& id,
        const ResourceManager<BinaryGate::Func, sf::Texture>& textures,
        Func fn,
        sf::Vector2f pos = {0, 0}
    );
    ~BinaryGate();

    bool collide(sf::Vector2f) const override;

    virtual sf::Vector2f getPosition() const override;
    virtual void setPosition(sf::Vector2f) override;

    sf::FloatRect getBoundingBox() const override;

    BinaryGate* clone(const std::string& id) const override;

    void update(const sf::RenderWindow&, float dt) override;
    void draw(sf::RenderWindow& window) const override;

    inline Func getFunc() const { return _process; }
    inline std::vector<Pin*> getAllPins() override { return {&_in1, &_in2, &_out}; }

    void toJson(nlohmann::json& j) const override;

    static int And(int, int);
    static int Or(int, int);
    static int Nand(int, int);
    static int Nor(int, int);
    static int Xor(int, int);

private:
    static constexpr float PADDING = 20;

    const ResourceManager<BinaryGate::Func, sf::Texture>& _textures;

    sf::Sprite _sprite;

    Pin _in1, _in2, _out;
    Pin::PinFlag _flags[2] = {Pin::Dirty};

    Func _process;
};
