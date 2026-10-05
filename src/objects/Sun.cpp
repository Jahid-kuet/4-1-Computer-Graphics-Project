// Sun.cpp — Radiant procedural Sun (Surjo / সূর্য) with glowing core and slender radial line rays.
// Emits bright daytime light across the rural village landscape.

#include "objects/Sun.h"
#include "Primitives.h"
#include <cmath>

using namespace math;

namespace Sun {

void draw(Shader& shader, const mat4& model, float animTime)
{
    shader.setInt("uUseTexture", 0);   // Pure radiant untextured celestial geometry
    shader.setFloat("emissive", 1.0f); // Maximum emissive glow (bypasses shading)

    // ── Radiant Solar Color Palette ─────────────────────────────
    const vec3 sunCoreCol  (1.00f, 0.96f, 0.48f); // Blazing incandescent golden-yellow core
    const vec3 rayLongCol  (1.00f, 0.88f, 0.22f); // Golden solar sunbeam lines
    const vec3 rayMedCol   (1.00f, 0.94f, 0.42f); // Warm bright daylight ray lines
    const vec3 rayShortCol (1.00f, 0.80f, 0.18f); // Warm amber solar flare ray lines

    // Dynamic solar pulsation (subtle gentle breathing)
    float pulseCore = 1.0f + 0.015f * sinf(animTime * 2.2f);

    // 1. Incandescent Solar Core Sphere
    mat4 core = model;
    core = scale(core, vec3(2.40f * pulseCore, 2.40f * pulseCore, 2.40f * pulseCore));
    Primitives::drawSphere(shader, core, sunCoreCol);

    // 2. Slender Radial Line Rays (32 rays in coronal plane, strictly line-like, no cone spikes)
    // 3-tier rhythmic hierarchy: Long (every 45°), Medium (every alternate 22.5°), Short (every 11.25°)
    const int numRays = 32;
    const float dAngle = (2.0f * 3.14159265f) / (float)numRays;
    const float rayThickness = 0.055f; // Slender line thickness (~1% of sun diameter)
    const float startRadius = 2.35f;  // Subtly embedded inside core sphere to avoid gaps

    for (int i = 0; i < numRays; ++i) {
        float angle = (float)i * dAngle;

        float baseLen;
        vec3 col;
        if (i % 4 == 0) {
            // 8 Major Cardinal & Diagonal Sunbeams
            baseLen = 2.10f;
            col = rayLongCol;
        } else if (i % 2 == 0) {
            // 8 Medium Intermediate Rays
            baseLen = 1.45f;
            col = rayMedCol;
        } else {
            // 16 Short Interstitial Solar Rays
            baseLen = 0.85f;
            col = rayShortCol;
        }

        // Gentle animated shimmer
        float shimmer = 1.0f + 0.05f * sinf(animTime * 2.8f + (float)i * 0.75f);
        float len = baseLen * shimmer;

        mat4 ray = model;
        ray = rotate(ray, angle, vec3(0.0f, 0.0f, 1.0f));
        ray = translate(ray, vec3(0.0f, startRadius + len * 0.5f, 0.0f));
        ray = scale(ray, vec3(rayThickness, len, rayThickness));
        Primitives::drawCube(shader, ray, col);
    }

    shader.setFloat("emissive", 0.0f);
}

vec3 getLightDirection(const vec3& sunPosition)
{
    // Direction vector FROM the sun toward the scene origin (0,0,0)
    return normalize(-sunPosition);
}

} // namespace Sun
