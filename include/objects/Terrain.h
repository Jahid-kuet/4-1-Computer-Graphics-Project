// Terrain.h — Ground plane, road networks, courtyards, paddy fields, and river ghat.
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Terrain {
    // Draws a large flat terrain plane, interconnected village road network,
    // swept clay courtyards (Uthan), river landing ghat, terraced paddy fields,
    // bamboo fences, and meadow grass.
    void draw(Shader& shader, const math::mat4& model);

    // Standalone traditional woven bamboo fence
    void drawBambooFence(Shader& shader, const math::mat4& model, const math::vec3& startPos, float length, float angleDeg);

    // Standalone clump of rural Bangladeshi rice plants (Dhan Gachh)
    void drawRiceClump(Shader& shader, const math::mat4& model, const math::vec3& pos = math::vec3(0.0f), float scaleVal = 1.0f, float seed = 0.0f);

    // Standalone clump of meadow grass
    void drawGrassClump(Shader& shader, const math::mat4& model, const math::vec3& pos = math::vec3(0.0f), float scaleVal = 1.0f);

    // Standalone river landing ghat (wooden/bamboo steps and platform)
    void drawGhat(Shader& shader, const math::mat4& model, const math::vec3& pos);
}
