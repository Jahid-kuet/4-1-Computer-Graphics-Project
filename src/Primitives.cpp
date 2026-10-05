// Primitives.cpp — Generates and caches canonical unit-geometry VAOs for reuse.
// STRICT PROJECT CONSTRAINT:
// Only 3 fundamental primitives exist in GPU buffers:
//   1. Unit Cube (each of 6 faces composed of 2 triangles, total 12 triangles)
//   2. Unit Triangle (canonical 2D/3D triangle with exact normals)
//   3. Unit Sphere (canonical latitude-longitude tessellated sphere)
// All objects and helper shapes in the entire project are constructed purely using these 3 primitives.

#include "Primitives.h"
#include <glad/glad.h>
#include <vector>
#include <cmath>

using namespace math;

// ─── Internal State ──────────────────────────────────────────────────
namespace {

struct MeshData {
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    int indexCount = 0;
};

// ONLY 3 GPU VAOs for the entire project
MeshData cubeMesh, triangleMesh, sphereMesh;

const int SECTORS = 24;
const int STACKS  = 12;

// Helper: push vertex (position + normal) into buffer
inline void pushVertex(std::vector<float>& v,
                       float px, float py, float pz,
                       float nx, float ny, float nz)
{
    v.push_back(px); v.push_back(py); v.push_back(pz);
    v.push_back(nx); v.push_back(ny); v.push_back(nz);
}

// Helper: upload mesh to GPU and fill MeshData
void uploadMesh(MeshData& md,
                const std::vector<float>& verts,
                const std::vector<unsigned int>& indices)
{
    md.indexCount = (int)indices.size();

    glGenVertexArrays(1, &md.VAO);
    glGenBuffers(1, &md.VBO);
    glGenBuffers(1, &md.EBO);

    glBindVertexArray(md.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, md.VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, md.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

// Static state tracker to eliminate redundant driver VAO unbind/bind state changes
static unsigned int s_currentBoundVAO = 0;

// Helper: common draw logic with zero-overhead uniform binding and VAO state tracking
void drawMesh(const MeshData& md, Shader& shader,
              const mat4& model, const vec3& color)
{
    shader.setFastModel(model);
    shader.setFastColor(color);
    if (s_currentBoundVAO != md.VAO) {
        glBindVertexArray(md.VAO);
        s_currentBoundVAO = md.VAO;
    }
    glDrawElements(GL_TRIANGLES, md.indexCount, GL_UNSIGNED_INT, 0);
}

// ─── 1. Canonical Unit Cube (1x1x1, centered at origin) ─────────────
// Each of the 6 square faces is composed of 2 triangles (total 12 triangles = 36 indices)
void generateCube(MeshData& md)
{
    float v[] = {
        // position          normal
        // Front (+Z)
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        // Back (-Z)
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        // Left (-X)
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        // Right (+X)
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        // Top (+Y)
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        // Bottom (-Y)
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    };

    unsigned int indices[] = {
         0, 1, 2,  2, 3, 0,       // front face (2 triangles)
         4, 5, 6,  6, 7, 4,       // back face (2 triangles)
         8, 9,10, 10,11, 8,       // left face (2 triangles)
        12,13,14, 14,15,12,       // right face (2 triangles)
        16,17,18, 18,19,16,       // top face (2 triangles)
        20,21,22, 22,23,20,       // bottom face (2 triangles)
    };

    std::vector<float> vv(v, v + sizeof(v) / sizeof(float));
    std::vector<unsigned int> ii(indices, indices + 36);
    uploadMesh(md, vv, ii);
}

// ─── 2. Canonical Unit Triangle ──────────────────────────────────────
// Base along X in [-0.5, 0.5] at Y = 0, apex at (0, 1, 0)
// Formed with double-sided normals (+Z front, -Z back) for robust two-sided shading
void generateTriangle(MeshData& md)
{
    float v[] = {
        // Front face (normal +Z)
        -0.5f, 0.0f, 0.0f,  0.0f, 0.0f,  1.0f,
         0.5f, 0.0f, 0.0f,  0.0f, 0.0f,  1.0f,
         0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  1.0f,
        // Back face (normal -Z, reversed winding)
         0.5f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
        -0.5f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
         0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f
    };

    unsigned int indices[] = {
        0, 1, 2, // front triangle
        3, 4, 5  // back triangle
    };

    std::vector<float> vv(v, v + sizeof(v) / sizeof(float));
    std::vector<unsigned int> ii(indices, indices + 6);
    uploadMesh(md, vv, ii);
}

// ─── 3. Canonical Unit Sphere (radius 1, centered at origin) ─────────
void generateSphere(MeshData& md)
{
    std::vector<float> verts;
    std::vector<unsigned int> idx;

    float sectorStep = 2.0f * PI / SECTORS;
    float stackStep  = PI / STACKS;

    for (int i = 0; i <= STACKS; i++) {
        float stackAngle = PI / 2.0f - i * stackStep;
        float y  = sinf(stackAngle);
        float xz = cosf(stackAngle);

        for (int j = 0; j <= SECTORS; j++) {
            float sectorAngle = j * sectorStep;
            float x = xz * cosf(sectorAngle);
            float z = xz * sinf(sectorAngle);
            pushVertex(verts, x, y, z, x, y, z); // normal = position for unit sphere
        }
    }

    for (int i = 0; i < STACKS; i++) {
        int k1 = i * (SECTORS + 1);
        int k2 = k1 + SECTORS + 1;
        for (int j = 0; j < SECTORS; j++, k1++, k2++) {
            if (i != 0)            { idx.push_back(k1); idx.push_back(k2); idx.push_back(k1 + 1); }
            if (i != STACKS - 1)   { idx.push_back(k1 + 1); idx.push_back(k2); idx.push_back(k2 + 1); }
        }
    }

    uploadMesh(md, verts, idx);
}

} // anonymous namespace

// ─── Public API ──────────────────────────────────────────────────────
namespace Primitives {

void init()
{
    generateCube(cubeMesh);
    generateTriangle(triangleMesh);
    generateSphere(sphereMesh);
}

void cleanup()
{
    resetVAOState();
    auto del = [](MeshData& m) {
        if (m.VAO) glDeleteVertexArrays(1, &m.VAO);
        if (m.VBO) glDeleteBuffers(1, &m.VBO);
        if (m.EBO) glDeleteBuffers(1, &m.EBO);
        m = {};
    };
    del(cubeMesh);
    del(triangleMesh);
    del(sphereMesh);
}

void resetVAOState()
{
    if (s_currentBoundVAO != 0) {
        glBindVertexArray(0);
        s_currentBoundVAO = 0;
    }
}

// ── Canonical Unit Primitives ────────────────────────────────────────
void drawCube(Shader& s, const mat4& m, const vec3& c)
{
    drawMesh(cubeMesh, s, m, c);
}

void drawTriangle(Shader& s, const mat4& m, const vec3& c)
{
    drawMesh(triangleMesh, s, m, c);
}

void drawSphere(Shader& s, const mat4& m, const vec3& c)
{
    drawMesh(sphereMesh, s, m, c);
}

// ── Procedural Compound Helpers (Composed PURELY from Cube, Triangle & Sphere) ──

// Unit plane (1x1, normal up): modeled as a flat unit cube (made of 2 triangles per face)
void drawPlane(Shader& s, const mat4& m, const vec3& c)
{
    drawCube(s, scale(m, vec3(1.0f, 0.001f, 1.0f)), c);
}

// Unit cylinder: 16-faceted column constructed entirely by intersecting rotated Unit Cubes
void drawCylinder(Shader& s, const mat4& m, const vec3& c)
{
    drawCube(s, scale(m, vec3(1.848f, 1.0f, 0.765f)), c);
    mat4 m45 = rotate(m, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
    drawCube(s, scale(m45, vec3(1.848f, 1.0f, 0.765f)), c);
    mat4 m90 = rotate(m, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    drawCube(s, scale(m90, vec3(1.848f, 1.0f, 0.765f)), c);
    mat4 m135 = rotate(m, radians(135.0f), vec3(0.0f, 1.0f, 0.0f));
    drawCube(s, scale(m135, vec3(1.848f, 1.0f, 0.765f)), c);
}

// Unit cone: 16-sided smooth cone constructed purely from 16 Unit Triangles meeting at apex (0, 1, 0)
void drawCone(Shader& s, const mat4& m, const vec3& c)
{
    const int steps = 16;
    const float dTheta = 2.0f * PI / (float)steps;
    const float halfDTheta = dTheta * 0.5f;
    const float R = 1.0f;
    const float d = R * cosf(halfDTheta);
    const float chord = 2.0f * R * sinf(halfDTheta);
    const float slantHeight = sqrtf(1.0f + d * d);
    const float tiltAngleDeg = atan2f(d, 1.0f) * 180.0f / PI; // angle to tilt inward toward apex (0, 1, 0)

    for (int i = 0; i < steps; ++i) {
        float midAngle = ((float)i + 0.5f) * (dTheta * 180.0f / PI);
        mat4 t = m;
        t = rotate(t, radians(midAngle), vec3(0.0f, 1.0f, 0.0f));
        t = translate(t, vec3(0.0f, 0.0f, d));
        t = rotate(t, radians(-tiltAngleDeg), vec3(1.0f, 0.0f, 0.0f)); // tilt INWARD toward apex (0, 1, 0)
        t = scale(t, vec3(chord, slantHeight, 1.0f));
        drawTriangle(s, t, c);
    }
}

// Upper hemisphere: drawn directly using Unit Sphere
void drawHemisphere(Shader& s, const mat4& m, const vec3& c)
{
    drawSphere(s, m, c);
}

// Triangular prism: 2 Unit Triangles (gables) + 3 Unit Cubes (base and slopes)
void drawPrism(Shader& s, const mat4& m, const vec3& c)
{
    // Front gable (+Z)
    mat4 f = translate(m, vec3(0.0f, 0.0f, 0.5f));
    drawTriangle(s, f, c);

    // Back gable (-Z)
    mat4 b = translate(m, vec3(0.0f, 0.0f, -0.5f));
    b = rotate(b, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    drawTriangle(s, b, c);

    // Bottom base
    mat4 bot = translate(m, vec3(0.0f, 0.0f, 0.0f));
    bot = scale(bot, vec3(1.0f, 0.001f, 1.0f));
    drawCube(s, bot, c);

    // Right slope (connects (0.5, 0) to (0, 1))
    mat4 rs = translate(m, vec3(0.25f, 0.5f, 0.0f));
    rs = rotate(rs, radians(-63.4349488f), vec3(0.0f, 0.0f, 1.0f));
    rs = scale(rs, vec3(1.118034f, 0.002f, 1.0f));
    drawCube(s, rs, c);

    // Left slope (connects (-0.5, 0) to (0, 1))
    mat4 ls = translate(m, vec3(-0.25f, 0.5f, 0.0f));
    ls = rotate(ls, radians(63.4349488f), vec3(0.0f, 0.0f, 1.0f));
    ls = scale(ls, vec3(1.118034f, 0.002f, 1.0f));
    drawCube(s, ls, c);
}

// 4-sided pyramid: 4 Unit Triangles meeting at apex (0, 1, 0) + 1 Unit Cube base
void drawPyramid(Shader& s, const mat4& m, const vec3& c)
{
    // Base closure (unit plane)
    mat4 bot = translate(m, vec3(0.0f, 0.0f, 0.0f));
    bot = scale(bot, vec3(1.0f, 0.001f, 1.0f));
    drawCube(s, bot, c);

    // 4 sloping triangular faces meeting exactly at apex (0, 1, 0)
    for (int i = 0; i < 4; ++i) {
        mat4 face = m;
        face = rotate(face, radians((float)i * 90.0f), vec3(0.0f, 1.0f, 0.0f));
        face = translate(face, vec3(0.0f, 0.0f, 0.5f));
        face = rotate(face, radians(-26.565051f), vec3(1.0f, 0.0f, 0.0f)); // tilt INWARD!
        face = scale(face, vec3(1.0f, 1.118034f, 1.0f));
        drawTriangle(s, face, c);
    }
}

// Arched half-cylinder shell: segmented curve formed of 8 Unit Cubes
void drawArch(Shader& s, const mat4& m, const vec3& c)
{
    const int steps = 8;
    const float dTheta = PI / (float)steps;
    const float segLen = 0.5f * dTheta * 1.08f;
    for (int i = 0; i < steps; ++i) {
        float theta = ((float)i + 0.5f) * dTheta;
        float x = 0.5f * cosf(theta);
        float y = 0.5f * sinf(theta);
        mat4 seg = translate(m, vec3(x, y, 0.0f));
        seg = rotate(seg, radians((theta * 180.0f / PI) - 90.0f), vec3(0.0f, 0.0f, 1.0f));
        seg = scale(seg, vec3(segLen, 0.04f, 1.0f));
        drawCube(s, seg, c);
    }
}

} // namespace Primitives
