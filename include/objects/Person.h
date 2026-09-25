// Person.h — Parametric villager rig with hierarchical limbs.
#pragma once
#include "Shader.h"
#include "mathutil.h"

// Configurable appearance and pose for each villager instance
struct PersonParams {
    math::vec3 skinColor    = math::vec3(0.55f, 0.38f, 0.25f);   // Bengali skin tone
    math::vec3 shirtColor   = math::vec3(0.15f, 0.30f, 0.55f);   // blue shirt / kurta
    math::vec3 pantsColor   = math::vec3(0.50f, 0.12f, 0.08f);   // red/checkered lungi
    float leftArmAngle      = 0.0f;    // radians, swing forward (+) / backward (-)
    float rightArmAngle     = 0.0f;
    float leftLegAngle      = 0.0f;
    float rightLegAngle     = 0.0f;
    bool  seated            = false;   // sitting on a charpai / bench
    bool  crossLegged       = false;   // sitting cross-legged on the ground
    bool  isElder           = false;   // white hair & white cotton beard
    bool  hasGamcha         = false;   // traditional red/checkered gamcha over shoulder
    math::vec3 gamchaColor  = math::vec3(0.78f, 0.18f, 0.12f);
    math::vec3 hairColor    = math::vec3(0.12f, 0.09f, 0.06f);   // dark hair
    bool  hasFan            = false;   // holds traditional handmade fan (Haat Pakha) in right hand
    float fanSway           = 0.0f;    // fanning oscillation angle in radians
    bool  isWoman           = false;   // wears traditional Bengali Saree with draped head veil (Ghomta) & glass bangles
    bool  hasTupi           = false;   // wears traditional white cotton Muslim prayer cap (Tupi / টুপি)
};

namespace Person {
    // Draws a complete villager with hierarchical limbs.
    void draw(Shader& shader, const math::mat4& model,
              const PersonParams& params = PersonParams());

    // Draws an open schoolbook/primer with distinct open pages, center gutter, and printed text.
    void drawBook(Shader& shader, const math::mat4& model);

    // Draws a traditional rural carved wooden folding bookstand (Rehal / রেহাল)
    void drawRehal(Shader& shader, const math::mat4& model);
}
