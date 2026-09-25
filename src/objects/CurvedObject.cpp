// CurvedObject.cpp — Procedural Curved 3D Objects for OpenGL 3.3 Core Profile.
// 1. Parametric Cubic Bézier Surface of Revolution (Terracotta Vase / Matir Surahi)
// 2. Traditional Curved Arched Bamboo Footbridge (Bansher Saako / বাঁশের সাঁকো)

#include "objects/CurvedObject.h"
#include "Primitives.h"
#include <glad/glad.h>
#include <vector>
#include <cmath>

using namespace math;

namespace {

struct MeshData {
    GLuint VAO = 0, VBO = 0, EBO = 0;
    int indexCount = 0;
};

MeshData g_bezierMesh;

// 2D Control point for profile curve: (radius, height)
struct Point2D { float r, y; };

// Cubic Bézier curve evaluation: B(u) for u in [0, 1]
inline Point2D evalBezier(Point2D p0, Point2D p1, Point2D p2, Point2D p3, float u)
{
    float u1 = 1.0f - u;
    float b0 = u1 * u1 * u1;
    float b1 = 3.0f * u1 * u1 * u;
    float b2 = 3.0f * u1 * u * u;
    float b3 = u * u * u;
    return {
        b0 * p0.r + b1 * p1.r + b2 * p2.r + b3 * p3.r,
        b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y
    };
}

// First derivative of Cubic Bézier curve for exact surface normal calculation
inline Point2D evalBezierDeriv(Point2D p0, Point2D p1, Point2D p2, Point2D p3, float u)
{
    float u1 = 1.0f - u;
    float d0 = -3.0f * u1 * u1;
    float d1 =  3.0f * u1 * (1.0f - 3.0f * u);
    float d2 =  3.0f * u * (2.0f - 3.0f * u);
    float d3 =  3.0f * u * u;
    return {
        d0 * p0.r + d1 * p1.r + d2 * p2.r + d3 * p3.r,
        d0 * p0.y + d1 * p1.y + d2 * p2.y + d3 * p3.y
    };
}

// Generate the Bézier Surface of Revolution mesh
void buildBezierVaseMesh()
{
    // Characteristic profile of an authentic Bengali terracotta pitcher / vase (Surahi)
    const Point2D P0 = { 0.28f, 0.00f }; // Base rim
    const Point2D P1 = { 0.82f, 0.38f }; // Bulging spherical body
    const Point2D P2 = { 0.16f, 0.78f }; // Slender tapered neck
    const Point2D P3 = { 0.36f, 1.00f }; // Flared elegant mouth rim

    const int SLICES_U  = 32; // Vertical parameter steps u
    const int SECTORS_V = 36; // Radial revolution steps theta

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // 1. Generate surface vertices (Pos + Normal: 6 floats per vertex)
    for (int i = 0; i <= SLICES_U; ++i) {
        float u = (float)i / (float)SLICES_U;
        Point2D pos2D = evalBezier(P0, P1, P2, P3, u);
        Point2D tan2D = evalBezierDeriv(P0, P1, P2, P3, u);

        for (int j = 0; j <= SECTORS_V; ++j) {
            float theta = (float)j * (2.0f * PI / (float)SECTORS_V);
            float cosT = std::cos(theta);
            float sinT = std::sin(theta);

            // 3D Position
            float px = pos2D.r * cosT;
            float py = pos2D.y;
            float pz = pos2D.r * sinT;

            // Analytic outward surface normal: normalize(dy/du * cosT, -dr/du, dy/du * sinT)
            vec3 norm(tan2D.y * cosT, -tan2D.r, tan2D.y * sinT);
            norm = norm.normalized();

            // Push vertex (location 0: pos, location 1: normal)
            vertices.push_back(px); vertices.push_back(py); vertices.push_back(pz);
            vertices.push_back(norm.x); vertices.push_back(norm.y); vertices.push_back(norm.z);
        }
    }

    // 2. Generate quad indices (2 triangles per patch)
    int rowStride = SECTORS_V + 1;
    for (int i = 0; i < SLICES_U; ++i) {
        for (int j = 0; j < SECTORS_V; ++j) {
            unsigned int i0 = i * rowStride + j;
            unsigned int i1 = i0 + 1;
            unsigned int i2 = (i + 1) * rowStride + j;
            unsigned int i3 = i2 + 1;

            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i1);

            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }

    // 3. Flat bottom base disc
    unsigned int baseCenterIdx = (unsigned int)(vertices.size() / 6);
    vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
    vertices.push_back(0.0f); vertices.push_back(-1.0f); vertices.push_back(0.0f);

    for (int j = 0; j <= SECTORS_V; ++j) {
        float theta = (float)j * (2.0f * PI / (float)SECTORS_V);
        vertices.push_back(P0.r * std::cos(theta));
        vertices.push_back(0.0f);
        vertices.push_back(P0.r * std::sin(theta));
        vertices.push_back(0.0f); vertices.push_back(-1.0f); vertices.push_back(0.0f);
    }
    for (int j = 0; j < SECTORS_V; ++j) {
        indices.push_back(baseCenterIdx);
        indices.push_back(baseCenterIdx + 1 + j + 1);
        indices.push_back(baseCenterIdx + 1 + j);
    }

    g_bezierMesh.indexCount = (int)indices.size();

    glGenVertexArrays(1, &g_bezierMesh.VAO);
    glGenBuffers(1, &g_bezierMesh.VBO);
    glGenBuffers(1, &g_bezierMesh.EBO);

    glBindVertexArray(g_bezierMesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, g_bezierMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_bezierMesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

} // anonymous namespace

namespace CurvedObject {

void init()
{
    buildBezierVaseMesh();
}

void cleanup()
{
    if (g_bezierMesh.VAO) {
        glDeleteVertexArrays(1, &g_bezierMesh.VAO);
        glDeleteBuffers(1, &g_bezierMesh.VBO);
        glDeleteBuffers(1, &g_bezierMesh.EBO);
        g_bezierMesh.VAO = g_bezierMesh.VBO = g_bezierMesh.EBO = 0;
    }
}

// 1. Draw smooth Cubic Bézier Surface of Revolution (Terracotta Pitcher / Surahi)
void drawBezierVase(Shader& shader, const mat4& model, const vec3& color)
{
    shader.setInt("uUseTexture", 0); // Disable texture to show smooth earthenware clay
    shader.setMat4("model", model);
    shader.setVec3("objectColor", color);

    glBindVertexArray(g_bezierMesh.VAO);
    glDrawElements(GL_TRIANGLES, g_bezierMesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Clay neck collar ring and mouth rim detail
    mat4 collarM = model;
    collarM = translate(collarM, vec3(0.0f, 0.78f, 0.0f));
    collarM = scale(collarM, vec3(0.22f, 0.035f, 0.22f));
    Primitives::drawCylinder(shader, collarM, color * 0.88f);

    mat4 rimM = model;
    rimM = translate(rimM, vec3(0.0f, 1.00f, 0.0f));
    rimM = scale(rimM, vec3(0.38f, 0.03f, 0.38f));
    Primitives::drawCylinder(shader, rimM, color * 0.92f);
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
        float y0 = getArchY(z0);
        float y1 = getArchY(z1);

        float dy = y1 - y0;
        float dz = z1 - z0;
        float pitchAngle = -std::atan2(dy, dz);

        mat4 plankM = model;
        plankM = translate(plankM, vec3(0.0f, (y0 + y1) * 0.5f + 0.045f, (z0 + z1) * 0.5f));
        plankM = rotate(plankM, pitchAngle, vec3(1.0f, 0.0f, 0.0f));
        plankM = scale(plankM, vec3(bridgeW, 0.035f, segStep * 0.88f));
        Primitives::drawCube(shader, plankM, (p % 2 == 0) ? plankCol : bambooCol);
    }

    // ── C. Curved Continuous Handrails & Vertical Stanchions ──────────
    const float railHeight = 0.85f;
    float railOffsetsX[2] = { -bridgeW * 0.52f, bridgeW * 0.52f };

    for (int side = 0; side < 2; ++side) {
        float railX = railOffsetsX[side];

        // 1. Seamless continuous curved bamboo handrail
        for (int s = 0; s < numCurveSegments; ++s) {
            float z0 = -bridgeL * 0.5f + (float)s * segStep;
            float z1 = -bridgeL * 0.5f + (float)(s + 1) * segStep;
            float y0 = getArchY(z0) + railHeight;
            float y1 = getArchY(z1) + railHeight;

            float dy = y1 - y0;
            float dz = z1 - z0;
            float len = std::sqrt(dy * dy + dz * dz);
            float pitchAngle = -std::atan2(dy, dz);

            mat4 railM = model;
            railM = translate(railM, vec3(railX, (y0 + y1) * 0.5f, (z0 + z1) * 0.5f));
            railM = rotate(railM, pitchAngle, vec3(1.0f, 0.0f, 0.0f));
            railM = scale(railM, vec3(0.045f, 0.045f, len * 1.03f));
            Primitives::drawCube(shader, railM, bambooCol);
        }

        // 2. Upright vertical stanchions supporting handrail
        for (int st = 0; st <= 6; ++st) {
            float z = -bridgeL * 0.48f + (float)st * (bridgeL * 0.96f / 6.0f);
            float yBase = getArchY(z);

            mat4 postM = model;
            postM = translate(postM, vec3(railX, yBase + railHeight * 0.5f, z));
            postM = scale(postM, vec3(0.04f, railHeight, 0.04f));
            Primitives::drawCylinder(shader, postM, postCol);

            // Jute rope tie joint at rail intersection
            mat4 tieM = model;
            tieM = translate(tieM, vec3(railX, yBase + railHeight, z));
            tieM = scale(tieM, vec3(0.07f, 0.05f, 0.07f));
            Primitives::drawCube(shader, tieM, lashingCol);
        }
    }

    // ── D. 4 Pairs of Cross-Braced Riverbank Bamboo Stilts ───────────
    float stiltZ[4] = { -bridgeL * 0.38f, -bridgeL * 0.15f, bridgeL * 0.15f, bridgeL * 0.38f };
    for (int k = 0; k < 4; ++k) {
        float z = stiltZ[k];
        float yDeck = getArchY(z);
        float stiltH = yDeck + 1.20f;

        // Pair of A-frame diagonal bamboo pilings driven into bank
        for (int leg = -1; leg <= 1; leg += 2) {
            mat4 legM = model;
            legM = translate(legM, vec3((float)leg * (bridgeW * 0.40f), (yDeck - 0.90f) * 0.5f, z));
            legM = rotate(legM, radians((float)leg * 11.0f), vec3(0.0f, 0.0f, 1.0f));
            legM = scale(legM, vec3(0.055f, stiltH, 0.055f));
            Primitives::drawCylinder(shader, legM, postCol);
        }

        // Horizontal cross-brace connecting the two piling legs
        mat4 braceM = model;
        braceM = translate(braceM, vec3(0.0f, yDeck * 0.35f, z));
        braceM = scale(braceM, vec3(bridgeW * 1.15f, 0.055f, 0.055f));
        Primitives::drawCube(shader, braceM, bambooCol);
    }
}

} // namespace CurvedObject
