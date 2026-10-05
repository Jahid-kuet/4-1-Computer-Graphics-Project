// CurvedObject.cpp — Procedural Curved 3D Objects for OpenGL 3.3 Core Profile.
// 1. Terracotta Surahi / Pitcher (Matir Surahi) sculpted from Unit Spheres & Unit Cubes
// 2. Traditional Curved Arched Bamboo Footbridge (Bansher Saako / বাঁশের সাঁকো)
// Strictly follows project rule: Built purely from Unit Cube, Unit Triangle, and Unit Sphere.

#include "objects/CurvedObject.h"
#include "Primitives.h"
#include <cmath>

using namespace math;

namespace CurvedObject {

void init()
{
    // No custom VAOs allocated; uses canonical unit primitives
}

void cleanup()
{
    // No custom VAOs to delete
}

// 1. Draw smooth Terracotta Pitcher / Vase (Surahi / সুরাহি)
// Sculpted purely from Unit Spheres and Unit Cubes
void drawBezierVase(Shader& shader, const mat4& model, const vec3& color)
{
    shader.setInt("uUseTexture", 0); // Natural earthenware terracotta clay

    // 1. Base foot pedestal
    mat4 foot = model;
    foot = translate(foot, vec3(0.0f, 0.02f, 0.0f));
    foot = scale(foot, vec3(0.56f, 0.04f, 0.56f));
    Primitives::drawCube(shader, foot, color * 0.86f);

    // 2. Wide bulging lower belly (globular body)
    mat4 body = model;
    body = translate(body, vec3(0.0f, 0.38f, 0.0f));
    body = scale(body, vec3(0.82f, 0.72f, 0.82f));
    Primitives::drawSphere(shader, body, color);

    // 3. Tapered upper shoulder
    mat4 shoulder = model;
    shoulder = translate(shoulder, vec3(0.0f, 0.65f, 0.0f));
    shoulder = scale(shoulder, vec3(0.48f, 0.34f, 0.48f));
    Primitives::drawSphere(shader, shoulder, color * 0.95f);

    // 4. Slender tapered neck column
    mat4 neck = model;
    neck = translate(neck, vec3(0.0f, 0.80f, 0.0f));
    neck = scale(neck, vec3(0.18f, 0.28f, 0.18f));
    Primitives::drawCube(shader, neck, color * 0.88f);

    // Neck decorative ridge ring
    mat4 collar = model;
    collar = translate(collar, vec3(0.0f, 0.78f, 0.0f));
    collar = scale(collar, vec3(0.24f, 0.035f, 0.24f));
    Primitives::drawCube(shader, collar, color * 0.82f);

    // 5. Flared mouth rim
    mat4 rim = model;
    rim = translate(rim, vec3(0.0f, 1.00f, 0.0f));
    rim = scale(rim, vec3(0.38f, 0.04f, 0.38f));
    Primitives::drawCube(shader, rim, color * 0.92f);

    // Inner shadow opening
    mat4 mouth = model;
    mouth = translate(mouth, vec3(0.0f, 1.01f, 0.0f));
    mouth = scale(mouth, vec3(0.24f, 0.03f, 0.24f));
    Primitives::drawCube(shader, mouth, color * 0.35f);
}

// 2. Draw traditional Curved Arched Bamboo Footbridge (Bansher Saako / বাঁশের সাঁকো)
void drawBambooBridge(Shader& shader, const mat4& model)
{
    const vec3 bambooCol (0.64f, 0.52f, 0.28f); // weathered golden-green bamboo
    const vec3 runnerCol (0.50f, 0.38f, 0.18f); // thick longitudinal bamboo culms
    const vec3 plankCol  (0.68f, 0.54f, 0.30f); // split bamboo tread slats
    const vec3 postCol   (0.38f, 0.26f, 0.12f); // vertical piling posts
    const vec3 lashingCol(0.22f, 0.16f, 0.08f); // jute coir rope lashings

    const float bridgeL = 7.6f;  // Total span length
    const float bridgeW = 1.15f; // Walkway width
    const float archH   = 1.05f; // Maximum parabolic arch rise at center

    // Parabolic arch elevation: y = archH * (1 - (2z/L)^2)
    auto getArchY = [&](float z) -> float {
        float normZ = (2.0f * z) / bridgeL; // -1 to +1
        return archH * (1.0f - normZ * normZ);
    };

    shader.setInt("uUseTexture", 0); // smooth natural aged bamboo culms and coir rope lashings

    // ── A. Seamless Curved Longitudinal Runner Poles (3 thick bamboo poles) ──
    const int numCurveSegments = 32;
    const float segStep = bridgeL / (float)numCurveSegments;

    float runnerOffsetsX[3] = { -bridgeW * 0.42f, 0.0f, bridgeW * 0.42f };

    for (int r = 0; r < 3; ++r) {
        float rx = runnerOffsetsX[r];
        for (int s = 0; s < numCurveSegments; ++s) {
            float z0 = -bridgeL * 0.5f + (float)s * segStep;
            float z1 = -bridgeL * 0.5f + (float)(s + 1) * segStep;
            float y0 = getArchY(z0);
            float y1 = getArchY(z1);

            float dy = y1 - y0;
            float dz = z1 - z0;
            float len = std::sqrt(dy * dy + dz * dz);
            float pitchAngle = -std::atan2(dy, dz); // Negative angle rotates +Z toward +Y for dy > 0

            mat4 segM = model;
            segM = translate(segM, vec3(rx, (y0 + y1) * 0.5f, (z0 + z1) * 0.5f));
            segM = rotate(segM, pitchAngle, vec3(1.0f, 0.0f, 0.0f));
            segM = scale(segM, vec3(0.065f, 0.065f, len * 1.03f));
            Primitives::drawCube(shader, segM, runnerCol);
        }
    }

    // ── B. Curved Cross-Slat Bamboo Tread Planks (Tightly spaced) ─────
    for (int p = 0; p < numCurveSegments; ++p) {
        float z0 = -bridgeL * 0.5f + (float)p * segStep;
        float z1 = -bridgeL * 0.5f + (float)(p + 1) * segStep;
        float zMid = (z0 + z1) * 0.5f;
        float yMid = getArchY(zMid);

        float dy = getArchY(z1) - getArchY(z0);
        float dz = z1 - z0;
        float pitchAngle = -std::atan2(dy, dz);

        mat4 plankM = model;
        plankM = translate(plankM, vec3(0.0f, yMid + 0.045f, zMid));
        plankM = rotate(plankM, pitchAngle, vec3(1.0f, 0.0f, 0.0f));
        plankM = scale(plankM, vec3(bridgeW, 0.035f, segStep * 0.88f));
        Primitives::drawCube(shader, plankM, (p % 2 == 0) ? plankCol : plankCol * 0.94f);
    }

    // ── C. Bamboo Piling Trestle Bents (Riverbank & Water supports) ───
    const float trestleZ[4] = { -2.6f, -0.9f, 0.9f, 2.6f };
    for (int t = 0; t < 4; ++t) {
        float tz = trestleZ[t];
        float ty = getArchY(tz);

        // Cross-beam transom under runners
        mat4 transom = model;
        transom = translate(transom, vec3(0.0f, ty - 0.045f, tz));
        transom = scale(transom, vec3(bridgeW * 1.25f, 0.065f, 0.065f));
        Primitives::drawCube(shader, transom, runnerCol);

        // A-frame angled bamboo pilings driven into the riverbed / bank
        for (int side = -1; side <= 1; side += 2) {
            float fside = (float)side;
            float px = fside * bridgeW * 0.50f;
            float postHeight = ty + 1.20f; // extends downward past water level

            mat4 post = model;
            post = translate(post, vec3(px + fside * 0.08f, ty - postHeight * 0.5f, tz));
            post = rotate(post, radians(fside * -6.5f), vec3(0.0f, 0.0f, 1.0f));
            post = scale(post, vec3(0.075f, postHeight, 0.075f));
            Primitives::drawCube(shader, post, postCol);

            // Coir rope lashings at transom junction
            mat4 lash = model;
            lash = translate(lash, vec3(px, ty - 0.045f, tz));
            lash = scale(lash, vec3(0.095f, 0.095f, 0.095f));
            Primitives::drawCube(shader, lash, lashingCol);
        }

        // Sway cross-brace X-strut between the pilings
        mat4 brace1 = model;
        brace1 = translate(brace1, vec3(0.0f, ty * 0.5f, tz));
        brace1 = rotate(brace1, radians(26.0f), vec3(0.0f, 0.0f, 1.0f));
        brace1 = scale(brace1, vec3(0.045f, ty * 0.95f + 0.3f, 0.045f));
        Primitives::drawCube(shader, brace1, runnerCol);

        mat4 brace2 = model;
        brace2 = translate(brace2, vec3(0.0f, ty * 0.5f, tz));
        brace2 = rotate(brace2, radians(-26.0f), vec3(0.0f, 0.0f, 1.0f));
        brace2 = scale(brace2, vec3(0.045f, ty * 0.95f + 0.3f, 0.045f));
        Primitives::drawCube(shader, brace2, runnerCol);
    }

    // ── D. Curved Bamboo Handrails (Guiding Rails on both sides) ──────
    const float railHeight = 0.88f; // Standard waist-height rail above deck

    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        float rx = fside * (bridgeW * 0.5f - 0.03f);

        // Continuous curved top handrail culm
        for (int s = 0; s < numCurveSegments; ++s) {
            float z0 = -bridgeL * 0.5f + (float)s * segStep;
            float z1 = -bridgeL * 0.5f + (float)(s + 1) * segStep;
            float y0 = getArchY(z0) + railHeight;
            float y1 = getArchY(z1) + railHeight;

            float dy = y1 - y0;
            float dz = z1 - z0;
            float len = std::sqrt(dy * dy + dz * dz);
            float pitchAngle = -std::atan2(dy, dz);

            mat4 railSeg = model;
            railSeg = translate(railSeg, vec3(rx, (y0 + y1) * 0.5f, (z0 + z1) * 0.5f));
            railSeg = rotate(railSeg, pitchAngle, vec3(1.0f, 0.0f, 0.0f));
            railSeg = scale(railSeg, vec3(0.05f, 0.05f, len * 1.03f));
            Primitives::drawCube(shader, railSeg, bambooCol);

            // Intermediate knee rail
            mat4 kneeSeg = model;
            kneeSeg = translate(kneeSeg, vec3(rx, (y0 + y1) * 0.5f - railHeight * 0.48f, (z0 + z1) * 0.5f));
            kneeSeg = rotate(kneeSeg, pitchAngle, vec3(1.0f, 0.0f, 0.0f));
            kneeSeg = scale(kneeSeg, vec3(0.038f, 0.038f, len * 1.03f));
            Primitives::drawCube(shader, kneeSeg, bambooCol * 0.94f);
        }

        // Vertical stanchion posts tying handrail to walkway runners
        const int numStanchions = 9;
        for (int st = 0; st < numStanchions; ++st) {
            float sz = -bridgeL * 0.45f + (float)st * (bridgeL * 0.90f / (float)(numStanchions - 1));
            float sy = getArchY(sz);

            mat4 stanchion = model;
            stanchion = translate(stanchion, vec3(rx, sy + railHeight * 0.5f, sz));
            stanchion = scale(stanchion, vec3(0.052f, railHeight + 0.10f, 0.052f));
            Primitives::drawCube(shader, stanchion, runnerCol);

            // Jute rope binding joints at rail crossings
            mat4 jointTop = model;
            jointTop = translate(jointTop, vec3(rx, sy + railHeight, sz));
            jointTop = scale(jointTop, vec3(0.07f, 0.06f, 0.07f));
            Primitives::drawCube(shader, jointTop, lashingCol);

            mat4 jointMid = model;
            jointMid = translate(jointMid, vec3(rx, sy + railHeight * 0.52f, sz));
            jointMid = scale(jointMid, vec3(0.065f, 0.05f, 0.065f));
            Primitives::drawCube(shader, jointMid, lashingCol);
        }
    }
}

} // namespace CurvedObject
