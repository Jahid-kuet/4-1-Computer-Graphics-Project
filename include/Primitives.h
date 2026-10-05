// Primitives.h — Reusable unit-geometry primitives for OpenGL 3.3.
// Call init() once after GLAD is loaded. Each draw function sets the
// model matrix and objectColor uniform, binds the cached VAO, and draws.
#pragma once

#include "Shader.h"
#include "mathutil.h"

namespace Primitives {

    // Create all VAO/VBO/EBO. Call once during startup.
    void init();

    // Delete all GPU buffers. Call during cleanup.
    void cleanup();

    // Reset VAO state tracking (call before drawing custom meshes or after frame finish)
    void resetVAOState();

    // ── CANONICAL BASE PRIMITIVES (Strict project rule: only Cube, Triangle & Sphere) ──
    // Unit cube (1x1x1, centered at origin; each of the 6 faces made of 2 triangles)
    void drawCube(Shader& shader, const math::mat4& model, const math::vec3& color);

    // Unit triangle (base [-0.5, 0.5] along X at Y=0, apex at (0, 1, 0), double-sided normal)
    void drawTriangle(Shader& shader, const math::mat4& model, const math::vec3& color);

    // Unit sphere (radius 1, centered at origin)
    void drawSphere(Shader& shader, const math::mat4& model, const math::vec3& color);

    // ── PROCEDURAL COMPOUND HELPERS (Composed PURELY from Unit Cube, Triangle & Sphere) ──
    // Unit cylinder (radius 1, height 1, centered at origin, Y-axis; composed of unit cubes)
    void drawCylinder(Shader& shader, const math::mat4& model, const math::vec3& color);

    // Unit cone (base radius 1 at Y=0, tip at Y=1; composed of unit triangles and cubes)
    void drawCone(Shader& shader, const math::mat4& model, const math::vec3& color);

    // Unit plane (1x1, at Y=0, normal up; composed of unit cubes / 2 triangles)
    void drawPlane(Shader& shader, const math::mat4& model, const math::vec3& color);

    // Upper hemisphere (radius 1, Y>=0; composed of unit sphere)
    void drawHemisphere(Shader& shader, const math::mat4& model, const math::vec3& color);

    // Triangular prism (base width 1 in X, height 1 in Y, length 1 in Z; composed of unit triangles and cubes)
    void drawPrism(Shader& shader, const math::mat4& model, const math::vec3& color);

    // 4-sided pyramid (1x1 base at Y=0, apex at Y=1; composed of unit triangles and cubes)
    void drawPyramid(Shader& shader, const math::mat4& model, const math::vec3& color);

    // Arched half-cylinder shell (radius 0.5, length 1 along Z, open bottom; composed of unit cubes)
    void drawArch(Shader& shader, const math::mat4& model, const math::vec3& color);
}
