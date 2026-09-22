// Tree.h — Procedural trees (palm, banana, bamboo, general).
#pragma once
#include "Shader.h"
#include "mathutil.h"

enum TreeType { TREE_GENERAL, TREE_PALM, TREE_BANANA, TREE_BAMBOO };

namespace Tree {
    // Draws a tree of the specified type
    void draw(Shader& shader, const math::mat4& model, TreeType type = TREE_GENERAL);
}
