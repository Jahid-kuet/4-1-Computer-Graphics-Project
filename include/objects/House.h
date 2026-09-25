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
    void draw(Shader& shader, const math::mat4& model, HouseStyle style = HOUSE_CHOUCHALA, bool withStove = false);

    // Standalone traditional clay water pitcher (Kolshi)
    void drawKolshi(Shader& shader, const math::mat4& model, const math::vec3& pos = math::vec3(0.0f), float scaleVal = 1.0f);

    // Standalone outdoor clay cooking stove (Matir Chula) with cooking pot, firewood, and 3-sided bamboo fence
    void drawStove(Shader& shader, const math::mat4& model, bool withFence = true);

    // Traditional Rice Straw Stack (Khorer Paloi / খড়ের পালা) with central bamboo pole
    void drawStrawStack(Shader& shader, const math::mat4& model, const math::vec3& pos = math::vec3(0.0f), float scaleVal = 1.0f);

    // Traditional Thatched Cow Shed (Gowal Ghor / গোয়াল ঘর) with feeding trough
    void drawCowShed(Shader& shader, const math::mat4& model);

    // Procedural rural Bangladeshi cow (Deshi Goru / দেশি গরু) with shoulder hump, horns, dewlap, and tail
    void drawCow(Shader& shader, const math::mat4& model, bool lyingDown = true);

    // Traditional rural Bangladeshi hand-pump tubewell (Chapa Kol / টিউবওয়েল) with concrete washing pad & drainage trough
    void drawTubewell(Shader& shader, const math::mat4& model, float pumpAngle = 0.0f, bool isPumping = false);

    // Traditional rural chicken coop (Murgir Khopa / মোরগের খোঁপা) on stilts (door shut at night, ramp down by day)
    void drawChickenCoop(Shader& shader, const math::mat4& model, bool isNight = true);
}

