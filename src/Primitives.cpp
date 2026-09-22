// Primitives.cpp — Generates and caches unit-geometry VAOs for reuse.

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

MeshData sphereMesh, cylinderMesh, coneMesh, cubeMesh, planeMesh, hemiMesh;
MeshData prismMesh, pyramidMesh, archMesh;

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

// Helper: common draw logic
void drawMesh(const MeshData& md, Shader& shader,
              const mat4& model, const vec3& color)
{
    shader.setMat4("model", model);
    shader.setVec3("objectColor", color);
    glBindVertexArray(md.VAO);
    glDrawElements(GL_TRIANGLES, md.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ─── Sphere generation ───────────────────────────────────────────────
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
            pushVertex(verts, x, y, z, x, y, z);   // normal = position for unit sphere
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

// ─── Hemisphere generation (upper half, Y >= 0) ─────────────────────
void generateHemisphere(MeshData& md)
{
    std::vector<float> verts;
    std::vector<unsigned int> idx;

    int halfStacks = STACKS / 2;
    float sectorStep = 2.0f * PI / SECTORS;
    float stackStep  = PI / STACKS;

    // Dome rings (from equator up to pole)
    for (int i = 0; i <= halfStacks; i++) {
        float stackAngle = i * stackStep;          // 0 (equator) → PI/2 (pole)
        float y  = sinf(stackAngle);
        float xz = cosf(stackAngle);

        for (int j = 0; j <= SECTORS; j++) {
            float sectorAngle = j * sectorStep;
            float x = xz * cosf(sectorAngle);
            float z = xz * sinf(sectorAngle);
            pushVertex(verts, x, y, z, x, y, z);
        }
    }

    for (int i = 0; i < halfStacks; i++) {
        int k1 = i * (SECTORS + 1);
        int k2 = k1 + SECTORS + 1;
        for (int j = 0; j < SECTORS; j++, k1++, k2++) {
            if (i != 0)              { idx.push_back(k1); idx.push_back(k2); idx.push_back(k1 + 1); }
            if (i != halfStacks - 1) { idx.push_back(k1 + 1); idx.push_back(k2); idx.push_back(k2 + 1); }
            else                     { idx.push_back(k1 + 1); idx.push_back(k2); idx.push_back(k2 + 1); }
        }
    }

    // Bottom cap (flat disc at Y = 0, normal pointing down)
    unsigned int baseCenter = (unsigned int)(verts.size() / 6);
    pushVertex(verts, 0, 0, 0, 0, -1, 0);
    for (int j = 0; j <= SECTORS; j++) {
        float angle = j * sectorStep;
        float x = cosf(angle);
        float z = sinf(angle);
        pushVertex(verts, x, 0, z, 0, -1, 0);
    }
    for (int j = 0; j < SECTORS; j++) {
        idx.push_back(baseCenter);
        idx.push_back(baseCenter + 1 + j + 1);
        idx.push_back(baseCenter + 1 + j);
    }

    uploadMesh(md, verts, idx);
}

// ─── Cylinder generation (radius 1, height 1, Y from -0.5 to 0.5) ──
void generateCylinder(MeshData& md)
{
    std::vector<float> verts;
    std::vector<unsigned int> idx;

    float sectorStep = 2.0f * PI / SECTORS;

    // Side vertices: bottom ring and top ring
    for (int ring = 0; ring <= 1; ring++) {
        float y = ring == 0 ? -0.5f : 0.5f;
        for (int j = 0; j <= SECTORS; j++) {
            float angle = j * sectorStep;
            float x = cosf(angle);
            float z = sinf(angle);
            pushVertex(verts, x, y, z, x, 0, z);   // side normals point outward
        }
    }

    // Side indices
    for (int j = 0; j < SECTORS; j++) {
        int bot = j;
        int top = j + SECTORS + 1;
        idx.push_back(bot);     idx.push_back(top);     idx.push_back(bot + 1);
        idx.push_back(bot + 1); idx.push_back(top);     idx.push_back(top + 1);
    }

    // Top cap
    unsigned int topCenter = (unsigned int)(verts.size() / 6);
    pushVertex(verts, 0, 0.5f, 0, 0, 1, 0);
    for (int j = 0; j <= SECTORS; j++) {
        float angle = j * sectorStep;
        pushVertex(verts, cosf(angle), 0.5f, sinf(angle), 0, 1, 0);
    }
    for (int j = 0; j < SECTORS; j++) {
        idx.push_back(topCenter);
        idx.push_back(topCenter + 1 + j);
        idx.push_back(topCenter + 1 + j + 1);
    }

    // Bottom cap
    unsigned int botCenter = (unsigned int)(verts.size() / 6);
    pushVertex(verts, 0, -0.5f, 0, 0, -1, 0);
    for (int j = 0; j <= SECTORS; j++) {
        float angle = j * sectorStep;
        pushVertex(verts, cosf(angle), -0.5f, sinf(angle), 0, -1, 0);
    }
    for (int j = 0; j < SECTORS; j++) {
        idx.push_back(botCenter);
        idx.push_back(botCenter + 1 + j + 1);
        idx.push_back(botCenter + 1 + j);
    }

    uploadMesh(md, verts, idx);
}

// ─── Cone generation (base radius 1 at Y=0, tip at Y=1) ─────────────
void generateCone(MeshData& md)
{
    std::vector<float> verts;
    std::vector<unsigned int> idx;

    float sectorStep = 2.0f * PI / SECTORS;
    float invLen = 1.0f / sqrtf(2.0f);     // for R=1, H=1: normal slope

    // Side vertices: base ring + tip ring (same position at tip, different normals per sector)
    for (int j = 0; j <= SECTORS; j++) {
        float angle = j * sectorStep;
        float cx = cosf(angle);
        float cz = sinf(angle);
        // Normal for cone side: outward and upward
        float nx = cx * invLen;
        float ny = invLen;
        float nz = cz * invLen;

        // Base vertex
        pushVertex(verts, cx, 0, cz, nx, ny, nz);
        // Tip vertex
        pushVertex(verts, 0, 1, 0, nx, ny, nz);
    }

    // Side indices (triangle strip → triangles)
    for (int j = 0; j < SECTORS; j++) {
        int base = j * 2;
        int next = (j + 1) * 2;
        // Triangle: base_j, tip_j, base_j+1
        idx.push_back(base);
        idx.push_back(base + 1);
        idx.push_back(next);
        // Triangle: base_j+1, tip_j, tip_j+1 (same tip position, different normal)
        idx.push_back(next);
        idx.push_back(base + 1);
        idx.push_back(next + 1);
    }

    // Base cap
    unsigned int capCenter = (unsigned int)(verts.size() / 6);
    pushVertex(verts, 0, 0, 0, 0, -1, 0);
    for (int j = 0; j <= SECTORS; j++) {
        float angle = j * sectorStep;
        pushVertex(verts, cosf(angle), 0, sinf(angle), 0, -1, 0);
    }
    for (int j = 0; j < SECTORS; j++) {
        idx.push_back(capCenter);
        idx.push_back(capCenter + 1 + j + 1);
        idx.push_back(capCenter + 1 + j);
    }

    uploadMesh(md, verts, idx);
}

// ─── Cube generation (1x1x1, centered at origin) ───────────────────
void generateCube(MeshData& md)
{
    // 6 faces × 4 vertices = 24 vertices, 6 faces × 6 indices = 36
    float v[] = {
        // position          normal
        // Front (+Z)
        -0.5f, -0.5f,  0.5f,  0, 0, 1,
         0.5f, -0.5f,  0.5f,  0, 0, 1,
         0.5f,  0.5f,  0.5f,  0, 0, 1,
        -0.5f,  0.5f,  0.5f,  0, 0, 1,
        // Back (-Z)
         0.5f, -0.5f, -0.5f,  0, 0,-1,
        -0.5f, -0.5f, -0.5f,  0, 0,-1,
        -0.5f,  0.5f, -0.5f,  0, 0,-1,
         0.5f,  0.5f, -0.5f,  0, 0,-1,
        // Left (-X)
        -0.5f, -0.5f, -0.5f, -1, 0, 0,
        -0.5f, -0.5f,  0.5f, -1, 0, 0,
        -0.5f,  0.5f,  0.5f, -1, 0, 0,
        -0.5f,  0.5f, -0.5f, -1, 0, 0,
        // Right (+X)
         0.5f, -0.5f,  0.5f,  1, 0, 0,
         0.5f, -0.5f, -0.5f,  1, 0, 0,
         0.5f,  0.5f, -0.5f,  1, 0, 0,
         0.5f,  0.5f,  0.5f,  1, 0, 0,
        // Top (+Y)
        -0.5f,  0.5f,  0.5f,  0, 1, 0,
         0.5f,  0.5f,  0.5f,  0, 1, 0,
         0.5f,  0.5f, -0.5f,  0, 1, 0,
        -0.5f,  0.5f, -0.5f,  0, 1, 0,
        // Bottom (-Y)
        -0.5f, -0.5f, -0.5f,  0,-1, 0,
         0.5f, -0.5f, -0.5f,  0,-1, 0,
         0.5f, -0.5f,  0.5f,  0,-1, 0,
        -0.5f, -0.5f,  0.5f,  0,-1, 0,
    };

    unsigned int indices[] = {
         0, 1, 2,  2, 3, 0,       // front
         4, 5, 6,  6, 7, 4,       // back
         8, 9,10, 10,11, 8,       // left
        12,13,14, 14,15,12,       // right
        16,17,18, 18,19,16,       // top
        20,21,22, 22,23,20,       // bottom
    };

    std::vector<float> vv(v, v + sizeof(v) / sizeof(float));
    std::vector<unsigned int> ii(indices, indices + 36);
    uploadMesh(md, vv, ii);
}

// ─── Plane generation (1x1, at Y=0, normal up) ──────────────────────
void generatePlane(MeshData& md)
{
    float v[] = {
        -0.5f, 0, -0.5f,  0, 1, 0,
         0.5f, 0, -0.5f,  0, 1, 0,
         0.5f, 0,  0.5f,  0, 1, 0,
        -0.5f, 0,  0.5f,  0, 1, 0,
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    std::vector<float> vv(v, v + 24);
    std::vector<unsigned int> ii(indices, indices + 6);
    uploadMesh(md, vv, ii);
}

// ─── Triangular Prism generation ────────────────────────────────────
// Base in X: [-0.5, 0.5] at Y=0, apex at X=0, Y=1, length Z in [-0.5, 0.5]
void generatePrism(MeshData& md)
{
    float nSlope = 1.0f / sqrtf(1.0f + 0.25f); // normal slope: (1, 0.5) normalized -> (2/sqrt(5), 1/sqrt(5))
    float nxR = 2.0f / sqrtf(5.0f);
    float nyR = 1.0f / sqrtf(5.0f);
    float nxL = -nxR;

    float v[] = {
        // Bottom (Y=0, normal down)
        -0.5f, 0.0f, -0.5f,  0.0f, -1.0f, 0.0f,
         0.5f, 0.0f, -0.5f,  0.0f, -1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,  0.0f, -1.0f, 0.0f,
        -0.5f, 0.0f,  0.5f,  0.0f, -1.0f, 0.0f,

        // Right slope (from (0.5, 0) to (0, 1))
         0.5f, 0.0f, -0.5f,  nxR, nyR, 0.0f,
         0.0f, 1.0f, -0.5f,  nxR, nyR, 0.0f,
         0.0f, 1.0f,  0.5f,  nxR, nyR, 0.0f,
         0.5f, 0.0f,  0.5f,  nxR, nyR, 0.0f,

        // Left slope (from (0, 1) to (-0.5, 0))
         0.0f, 1.0f, -0.5f,  nxL, nyR, 0.0f,
        -0.5f, 0.0f, -0.5f,  nxL, nyR, 0.0f,
        -0.5f, 0.0f,  0.5f,  nxL, nyR, 0.0f,
         0.0f, 1.0f,  0.5f,  nxL, nyR, 0.0f,

        // Front gable (+Z, normal +Z)
        -0.5f, 0.0f, 0.5f,   0.0f, 0.0f, 1.0f,
         0.5f, 0.0f, 0.5f,   0.0f, 0.0f, 1.0f,
         0.0f, 1.0f, 0.5f,   0.0f, 0.0f, 1.0f,

        // Back gable (-Z, normal -Z)
         0.5f, 0.0f, -0.5f,  0.0f, 0.0f, -1.0f,
        -0.5f, 0.0f, -0.5f,  0.0f, 0.0f, -1.0f,
         0.0f, 1.0f, -0.5f,  0.0f, 0.0f, -1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0,        // bottom
        4, 5, 6,  6, 7, 4,        // right slope
        8, 9, 10, 10, 11, 8,      // left slope
        12, 13, 14,               // front gable
        15, 16, 17                // back gable
    };

    std::vector<float> vv(v, v + sizeof(v) / sizeof(float));
    std::vector<unsigned int> ii(indices, indices + sizeof(indices) / sizeof(unsigned int));
    uploadMesh(md, vv, ii);
}

// ─── 4-sided Pyramid generation ─────────────────────────────────────
// Base in XZ: [-0.5, 0.5] at Y=0, apex at (0, 1, 0)
void generatePyramid(MeshData& md)
{
    float n1 = 2.0f / sqrtf(5.0f);
    float n2 = 1.0f / sqrtf(5.0f);

    float v[] = {
        // Base (normal down)
        -0.5f, 0.0f, -0.5f,  0.0f, -1.0f, 0.0f,
         0.5f, 0.0f, -0.5f,  0.0f, -1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,  0.0f, -1.0f, 0.0f,
        -0.5f, 0.0f,  0.5f,  0.0f, -1.0f, 0.0f,

        // Front face (+Z)
        -0.5f, 0.0f, 0.5f,   0.0f, n2, n1,
         0.5f, 0.0f, 0.5f,   0.0f, n2, n1,
         0.0f, 1.0f, 0.0f,   0.0f, n2, n1,

        // Back face (-Z)
         0.5f, 0.0f, -0.5f,  0.0f, n2, -n1,
        -0.5f, 0.0f, -0.5f,  0.0f, n2, -n1,
         0.0f, 1.0f, 0.0f,   0.0f, n2, -n1,

        // Left face (-X)
        -0.5f, 0.0f, -0.5f,  -n1, n2, 0.0f,
        -0.5f, 0.0f,  0.5f,  -n1, n2, 0.0f,
         0.0f, 1.0f, 0.0f,   -n1, n2, 0.0f,

        // Right face (+X)
         0.5f, 0.0f,  0.5f,   n1, n2, 0.0f,
         0.5f, 0.0f, -0.5f,   n1, n2, 0.0f,
         0.0f, 1.0f, 0.0f,    n1, n2, 0.0f,
    };

    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0, // base
        4, 5, 6,           // front
        7, 8, 9,           // back
        10, 11, 12,        // left
        13, 14, 15         // right
    };

    std::vector<float> vv(v, v + sizeof(v) / sizeof(float));
    std::vector<unsigned int> ii(indices, indices + sizeof(indices) / sizeof(unsigned int));
    uploadMesh(md, vv, ii);
}

// ─── Arched Half-Cylinder Shell generation ──────────────────────────
// Semicircular arch (radius 0.5), length 1 along Z [-0.5, 0.5]
void generateArch(MeshData& md)
{
    std::vector<float> verts;
    std::vector<unsigned int> idx;

    int steps = 16;
    float dTheta = PI / steps;

    // Both outer and inner faces for solid visibility from all angles
    for (int i = 0; i <= steps; i++) {
        float theta = i * dTheta;
        float x = 0.5f * cosf(theta);
        float y = 0.5f * sinf(theta);
        float nx = cosf(theta);
        float ny = sinf(theta);

        // Near vertex (-Z)
        pushVertex(verts, x, y, -0.5f, nx, ny, 0.0f);
        // Far vertex (+Z)
        pushVertex(verts, x, y,  0.5f, nx, ny, 0.0f);
    }

    for (int i = 0; i < steps; i++) {
        int v0 = i * 2;
        int v1 = v0 + 1;
        int v2 = (i + 1) * 2;
        int v3 = v2 + 1;

        // Outer surface
        idx.push_back(v0); idx.push_back(v2); idx.push_back(v1);
        idx.push_back(v1); idx.push_back(v2); idx.push_back(v3);

        // Inner surface (reversed winding so inside of boat hood is lit)
        idx.push_back(v0); idx.push_back(v1); idx.push_back(v2);
        idx.push_back(v1); idx.push_back(v3); idx.push_back(v2);
    }

    uploadMesh(md, verts, idx);
}

} // anonymous namespace

// ─── Public API ──────────────────────────────────────────────────────
namespace Primitives {

void init()
{
    generateSphere(sphereMesh);
    generateHemisphere(hemiMesh);
    generateCylinder(cylinderMesh);
    generateCone(coneMesh);
    generateCube(cubeMesh);
    generatePlane(planeMesh);
    generatePrism(prismMesh);
    generatePyramid(pyramidMesh);
    generateArch(archMesh);
}

void cleanup()
{
    auto del = [](MeshData& m) {
        if (m.VAO) glDeleteVertexArrays(1, &m.VAO);
        if (m.VBO) glDeleteBuffers(1, &m.VBO);
        if (m.EBO) glDeleteBuffers(1, &m.EBO);
        m = {};
    };
    del(sphereMesh); del(hemiMesh); del(cylinderMesh);
    del(coneMesh);   del(cubeMesh); del(planeMesh);
    del(prismMesh);  del(pyramidMesh); del(archMesh);
}

void drawSphere    (Shader& s, const mat4& m, const vec3& c) { drawMesh(sphereMesh,   s, m, c); }
void drawCylinder  (Shader& s, const mat4& m, const vec3& c) { drawMesh(cylinderMesh, s, m, c); }
void drawCone      (Shader& s, const mat4& m, const vec3& c) { drawMesh(coneMesh,     s, m, c); }
void drawCube      (Shader& s, const mat4& m, const vec3& c) { drawMesh(cubeMesh,     s, m, c); }
void drawPlane     (Shader& s, const mat4& m, const vec3& c) { drawMesh(planeMesh,    s, m, c); }
void drawHemisphere(Shader& s, const mat4& m, const vec3& c) { drawMesh(hemiMesh,     s, m, c); }
void drawPrism     (Shader& s, const mat4& m, const vec3& c) { drawMesh(prismMesh,    s, m, c); }
void drawPyramid   (Shader& s, const mat4& m, const vec3& c) { drawMesh(pyramidMesh,  s, m, c); }
void drawArch      (Shader& s, const mat4& m, const vec3& c) { drawMesh(archMesh,     s, m, c); }

} // namespace Primitives
