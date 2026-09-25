// River.h — River body with sloped riverbanks.
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace River {
    // Draws river water plane and sloped banks on both sides.
    // time parameter is exposed for future wave animation (unused for now).
    void draw(Shader& shader, const math::mat4& model, float time = 0.0f);

    // Standalone water lily (Shapla) pad and blossom
    void drawShapla(Shader& shader, const math::mat4& model, const math::vec3& pos = math::vec3(0.0f), float scaleVal = 1.0f);

    // Standalone cluster of riverbank reeds / Kashbon (Catkins)
    void drawKashbonCluster(Shader& shader, const math::mat4& model, const math::vec3& pos = math::vec3(0.0f), int count = 5, float seed = 0.0f);

    // Standalone bamboo mooring stake (Khuti) with coiled rope
    void drawMooringStake(Shader& shader, const math::mat4& model, const math::vec3& rootPos = math::vec3(0.0f));

    // Standalone fishing net drying rack (Jal Shukanor Macha) along riverbank
    void drawNetRack(Shader& shader, const math::mat4& model, const math::vec3& pos = math::vec3(0.0f));
}
