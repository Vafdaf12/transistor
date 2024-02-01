#pragma once

#include "circuit/Circuit.h"
#include "circuit/view/CircuitView.h"
#include "circuit/Pin.h"

class BinaryGate : public Circuit {
public:
    using Func = int (*)(int, int);

    BinaryGate(
        const std::string& id,
        Func fn,
        CircuitView* view
    );
    BinaryGate(const BinaryGate& other);




    void setPosition(sf::Vector2f p) override;
    BinaryGate* clone(const std::string& id) const override;

    void update(const sf::RenderWindow&, float dt) override;
    void draw(sf::RenderWindow& window) const override;

    inline Func getFunc() const { return m_process; }
    inline std::vector<Pin*> getAllPins() override { return {&m_in1, &m_in2, &m_out}; }

    void toJson(nlohmann::json& j) const override;

    static int And(int, int);
    static int Or(int, int);
    static int Nand(int, int);
    static int Nor(int, int);
    static int Xor(int, int);

private:
    void layout();
    static constexpr float PADDING = 20;

    Pin m_in1;
    Pin m_in2;
    Pin m_out;

    Func m_process;
};
