// Fisherman.h — Traditional Bangladeshi River Fisherman hunting fish (নদীতে মাছ শিকারী জেলে)
// Features:
// 1. Traditional slender wooden fishing dingi boat with upturned horns and thwarts.
// 2. Athletic sun-tanned village fisherman in tucked lungi (Malkocha) and tied crimson Gamcha.
// 3. Dynamic expanding circular Cast Net (Khepla Jal / ঝাঁকি জাল) with weighted lead sinkers & splash spray.
// 4. Energetic silver river fishes (Rupali Ilish / Rui) leaping and flopping out of the water spray.
// 5. Traditional woven bamboo fish basket (Khalui / খালুই) and bamboo fish plunge trap (Polo / পলো).
// 6. Multi-pronged barbed iron fish gig spear (Teta / টেঁটা) and Kerosene Hariken lantern.

#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Fisherman {
    // Draws the complete fishing boat with fisherman, cast net, leaping fish, and gear
    void draw(Shader& shader, const math::mat4& model, float animTime = 0.0f);

    // Standalone individual components for showcase and modular composition
    void drawFishermanFigure(Shader& shader, const math::mat4& model, float animTime = 0.0f);
    void drawCastNet(Shader& shader, const math::mat4& model, float animTime = 0.0f);
    void drawFish(Shader& shader, const math::mat4& model, float wiggle = 0.0f, float scaleVal = 1.0f);
    void drawGear(Shader& shader, const math::mat4& model);
}
