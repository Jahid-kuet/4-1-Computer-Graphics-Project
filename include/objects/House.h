// House.h — Bangladeshi rural house (mud walls, thatched/tin roof, verandah).
#pragma once
#include "Shader.h"
#include "mathutil.h"

enum HouseStyle {
    HOUSE_CHOUCHALA = 0, // Traditional 4-sloped pitched hip roof with verandah
    HOUSE_DOCHALA   = 1  // Traditional 2-sloped gable roof with side shed
};

namespace House {
    // Draws a complete authentic Bangladeshi village house
    void draw(Shader& shader, const math::mat4& model, HouseStyle style = HOUSE_CHOUCHALA);
}

