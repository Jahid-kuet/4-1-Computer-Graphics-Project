// Boat.cpp — Traditional Bangladeshi wooden boat (Dingi Nouka / Pal Tola Nouka)
// Features an authentic curved sheer hull with flared timber strakes,
// sculpted pointed Golui (prow) with auspicious painted eye (Noukar Chokh),
// swept Pachha (stern) with elevated stern seat (Gura),
// traditional arched bamboo canopy (Chhoi) with hanging Hariken lantern,
// internal wooden framing ribs, floorboards (Patan), thwarts (Goor),
// and an iconic billowing sail (Pal) with bamboo mast, yard spar, and authentic rigging.

#include "objects/Boat.h"
#include "objects/Charpai.h"
#include "Primitives.h"
#include "Texture.h"
#include <glad/glad.h>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace math;

namespace {

inline float length(const vec3& v) { return v.length(); }

struct MeshData {
    GLuint VAO = 0, VBO = 0, EBO = 0;
    int indexCount = 0;
};

static MeshData s_hullMesh;
static MeshData s_sailMesh;
static MeshData s_sailStripeMesh;
static bool s_meshesInitialized = false;

inline void pushVertex(std::vector<float>& v,
                       float px, float py, float pz,
                       float nx, float ny, float nz)
{
    v.push_back(px); v.push_back(py); v.push_back(pz);
    v.push_back(nx); v.push_back(ny); v.push_back(nz);
}

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

void drawMesh(const MeshData& md, Shader& shader,
              const mat4& model, const vec3& color)
{
    shader.setMat4("model", model);
    shader.setVec3("objectColor", color);
    glBindVertexArray(md.VAO);
    glDrawElements(GL_TRIANGLES, md.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Helper: draws a slender cylinder between two 3D points for rigging ropes, stays & lashings
void drawRope(Shader& shader, const mat4& model,
              const vec3& p1, const vec3& p2,
              float radius, const vec3& color)
{
    vec3 v = p2 - p1;
    float len = length(v);
    if (len < 1e-4f) return;
    vec3 dir = v / len;
    vec3 mid = (p1 + p2) * 0.5f;

    mat4 m = model;
    m = translate(m, mid);

    vec3 up(0.0f, 1.0f, 0.0f);
    vec3 axis = cross(up, dir);
    float dotVal = dot(up, dir);
    if (length(axis) > 1e-4f) {
        axis = normalize(axis);
        float angle = acosf(fmaxf(-1.0f, fminf(1.0f, dotVal)));
        m = rotate(m, angle, axis);
    } else if (dotVal < -0.999f) {
        m = rotate(m, radians(180.0f), vec3(1.0f, 0.0f, 0.0f));
    }

    m = scale(m, vec3(radius, len, radius));
    Primitives::drawCylinder(shader, m, color);
}

// ─── Procedural Dingi Hull Mesh Generation ───────────────────────────
// Constructs a continuous flared timber hull with authentic sheer curve,
// sweeping bow/stern lift, keel rocker, and solid timber plank thickness.
void buildHullMesh(MeshData& md)
{
    const int Nz = 36;
    const int Nu = 16;
    const int numVerts = (Nz + 1) * (Nu + 1);

    std::vector<vec3> outerPos(numVerts);
    std::vector<vec3> outerNorm(numVerts, vec3(0.0f));

    for (int i = 0; i <= Nz; ++i) {
        float tz = (float)i / (float)Nz;
        float z = -2.55f + tz * 5.20f; // Stern at -2.55m, Bow at +2.65m

        // Normalized centerline parameter: [-1.0, 1.0]
        float zc = (z - 0.05f) / 2.60f;
        zc = fmaxf(-1.0f, fminf(1.0f, zc));

        // Waterline breadth tapering: classic double-ended hull profile
        float wFactor = sqrtf(fmaxf(0.0f, 1.0f - zc * zc));
        float wHalf = 0.52f * wFactor * (1.0f - 0.15f * zc * zc);

        // Authentic Bengali sheer curve: sweeps up gracefully towards bow & stern
        float ySheer = 0.20f + 0.30f * (zc * zc) + 0.05f * zc; // Bow rises higher
        // Keel rocker depth: curves upward at both ends out of the water
        float yKeel  = -0.16f + 0.26f * (zc * zc);
        float depth  = ySheer - yKeel;

        for (int j = 0; j <= Nu; ++j) {
            float tu = (float)j / (float)Nu;
            float u = -1.5707963f + tu * 3.14159265f; // [-pi/2, +pi/2]

            // Flared concave-convex hull strake profile
            float x = wHalf * sinf(u);
            float y = yKeel + depth * (1.0f - cosf(u));

            outerPos[i * (Nu + 1) + j] = vec3(x, y, z);
        }
    }

    // Generate quad triangles & compute outward face normals
    std::vector<unsigned int> outerIndices;
    for (int i = 0; i < Nz; ++i) {
        for (int j = 0; j < Nu; ++j) {
            int i00 = i * (Nu + 1) + j;
            int i01 = i * (Nu + 1) + (j + 1);
            int i10 = (i + 1) * (Nu + 1) + j;
            int i11 = (i + 1) * (Nu + 1) + (j + 1);

            vec3 p0 = outerPos[i00];
            vec3 p1 = outerPos[i10];
            vec3 p2 = outerPos[i01];
            vec3 p3 = outerPos[i11];

            vec3 fn1 = cross(p1 - p0, p2 - p0);
            if (length(fn1) > 1e-6f) fn1 = normalize(fn1);

            vec3 fn2 = cross(p1 - p2, p3 - p2);
            if (length(fn2) > 1e-6f) fn2 = normalize(fn2);

            outerIndices.push_back(i00);
            outerIndices.push_back(i10);
            outerIndices.push_back(i01);

            outerIndices.push_back(i01);
            outerIndices.push_back(i10);
            outerIndices.push_back(i11);

            outerNorm[i00] += fn1;
            outerNorm[i10] += fn1 + fn2;
            outerNorm[i01] += fn1 + fn2;
            outerNorm[i11] += fn2;
        }
    }

    for (int k = 0; k < numVerts; ++k) {
        if (length(outerNorm[k]) > 1e-6f) {
            outerNorm[k] = normalize(outerNorm[k]);
        } else {
            outerNorm[k] = vec3(0.0f, -1.0f, 0.0f);
        }
    }

    // Build solid watertight hull: outer surface, inner surface, and gunwale rim
    const float tWood = 0.016f; // Realistic timber hull plank thickness
    std::vector<float> verts;
    verts.reserve(numVerts * 2 * 6);

    // 1. Push outer vertices
    for (int k = 0; k < numVerts; ++k) {
        pushVertex(verts,
                   outerPos[k].x, outerPos[k].y, outerPos[k].z,
                   outerNorm[k].x, outerNorm[k].y, outerNorm[k].z);
    }

    // 2. Push inner vertices (offset inward along normal with inverted normals)
    for (int k = 0; k < numVerts; ++k) {
        vec3 inP = outerPos[k] - outerNorm[k] * tWood;
        vec3 inN = -outerNorm[k];
        pushVertex(verts, inP.x, inP.y, inP.z, inN.x, inN.y, inN.z);
    }

    std::vector<unsigned int> allIndices;
    const unsigned int innerOffset = (unsigned int)numVerts;

    // Outer triangles
    for (size_t t = 0; t < outerIndices.size(); ++t) {
        allIndices.push_back(outerIndices[t]);
    }

    // Inner triangles (reversed winding so interior of boat is lit)
    for (size_t t = 0; t < outerIndices.size(); t += 3) {
        allIndices.push_back(innerOffset + outerIndices[t]);
        allIndices.push_back(innerOffset + outerIndices[t + 2]);
        allIndices.push_back(innerOffset + outerIndices[t + 1]);
    }

    // 3. Gunwale rim quads: bridges outer and inner edges along port (j = 0) and starboard (j = Nu)
    for (int i = 0; i < Nz; ++i) {
        // Port gunwale edge (j = 0)
        unsigned int o0 = i * (Nu + 1);
        unsigned int o1 = (i + 1) * (Nu + 1);
        unsigned int in0 = innerOffset + o0;
        unsigned int in1 = innerOffset + o1;
        allIndices.push_back(o0); allIndices.push_back(o1); allIndices.push_back(in0);
        allIndices.push_back(o1); allIndices.push_back(in1); allIndices.push_back(in0);

        // Starboard gunwale edge (j = Nu)
        unsigned int so0 = i * (Nu + 1) + Nu;
        unsigned int so1 = (i + 1) * (Nu + 1) + Nu;
        unsigned int sin0 = innerOffset + so0;
        unsigned int sin1 = innerOffset + so1;
        allIndices.push_back(so0); allIndices.push_back(sin0); allIndices.push_back(so1);
        allIndices.push_back(so1); allIndices.push_back(sin0); allIndices.push_back(sin1);
    }

    uploadMesh(md, verts, allIndices);
}

// ─── Procedural Billowing Bengali Sail Mesh Generation ───────────────
// Creates a deeply billowed aerodynamic sail with double-sided lighting
// and an authentic terracotta folk accent stripe across the midsection.
void buildSailMesh(MeshData& sailMd, MeshData& stripeMd)
{
    const int Nx = 20;
    const int Ny = 16;
    const int numVerts = (Nx + 1) * (Ny + 1);

    const float W = 2.10f;       // Sail width across yard
    const float H = 1.75f;       // Sail height
    const float yMin = 1.30f;    // Bottom edge height
    const float zBase = 1.18f;   // Fore-and-aft position ahead of mast
    const float maxBillow = 0.38f; // Aerodynamic belly depth catching wind

    std::vector<vec3> sailPos(numVerts);
    std::vector<vec3> sailNorm(numVerts, vec3(0.0f));

    for (int j = 0; j <= Ny; ++j) {
        float ty = (float)j / (float)Ny;
        float y = yMin + ty * H;

        for (int i = 0; i <= Nx; ++i) {
            float tx = (float)i / (float)Nx;
            float x = -W * 0.5f + tx * W;

            // Parabolic billow profile: zero at edges, maximum in center belly
            float billow = maxBillow * sinf(3.14159265f * ty) * cosf(3.14159265f * (x / W));
            float z = zBase + billow;

            sailPos[j * (Nx + 1) + i] = vec3(x, y, z);
        }
    }

    std::vector<unsigned int> indices;
    for (int j = 0; j < Ny; ++j) {
        for (int i = 0; i < Nx; ++i) {
            int i00 = j * (Nx + 1) + i;
            int i10 = j * (Nx + 1) + (i + 1);
            int i01 = (j + 1) * (Nx + 1) + i;
            int i11 = (j + 1) * (Nx + 1) + (i + 1);

            vec3 p0 = sailPos[i00];
            vec3 p1 = sailPos[i10];
            vec3 p2 = sailPos[i01];
            vec3 fn = cross(p1 - p0, p2 - p0);
            if (length(fn) > 1e-6f) fn = normalize(fn);

            indices.push_back(i00); indices.push_back(i10); indices.push_back(i01);
            indices.push_back(i10); indices.push_back(i11); indices.push_back(i01);

            sailNorm[i00] += fn;
            sailNorm[i10] += fn;
            sailNorm[i01] += fn;
            sailNorm[i11] += fn;
        }
    }

    for (int k = 0; k < numVerts; ++k) {
        if (length(sailNorm[k]) > 1e-6f) {
            sailNorm[k] = normalize(sailNorm[k]);
        } else {
            sailNorm[k] = vec3(0.0f, 0.0f, 1.0f);
        }
    }

    // Double-sided vertices so sail receives light from both front and rear
    std::vector<float> sailVerts;
    sailVerts.reserve(numVerts * 2 * 6);
    for (int k = 0; k < numVerts; ++k) {
        pushVertex(sailVerts, sailPos[k].x, sailPos[k].y, sailPos[k].z,
                   sailNorm[k].x, sailNorm[k].y, sailNorm[k].z);
    }
    for (int k = 0; k < numVerts; ++k) {
        pushVertex(sailVerts, sailPos[k].x, sailPos[k].y, sailPos[k].z,
                   -sailNorm[k].x, -sailNorm[k].y, -sailNorm[k].z);
    }

    std::vector<unsigned int> allSailIndices;
    const unsigned int backOffset = (unsigned int)numVerts;
    for (size_t t = 0; t < indices.size(); ++t) {
        allSailIndices.push_back(indices[t]);
    }
    for (size_t t = 0; t < indices.size(); t += 3) {
        allSailIndices.push_back(backOffset + indices[t]);
        allSailIndices.push_back(backOffset + indices[t + 2]);
        allSailIndices.push_back(backOffset + indices[t + 1]);
    }

    uploadMesh(sailMd, sailVerts, allSailIndices);

    // Terracotta folk accent stripe across the midsection (j from 7 to 10)
    std::vector<float> stripeVerts;
    std::vector<unsigned int> stripeIndices;
    int jStart = 7, jEnd = 10;
    int stripeVertCount = 0;

    for (int j = jStart; j <= jEnd; ++j) {
        for (int i = 0; i <= Nx; ++i) {
            int k = j * (Nx + 1) + i;
            vec3 pos = sailPos[k] + sailNorm[k] * 0.003f; // Slight offset to prevent z-fighting
            pushVertex(stripeVerts, pos.x, pos.y, pos.z, sailNorm[k].x, sailNorm[k].y, sailNorm[k].z);
            stripeVertCount++;
        }
    }

    for (int j = 0; j < (jEnd - jStart); ++j) {
        for (int i = 0; i < Nx; ++i) {
            int i00 = j * (Nx + 1) + i;
            int i10 = j * (Nx + 1) + (i + 1);
            int i01 = (j + 1) * (Nx + 1) + i;
            int i11 = (j + 1) * (Nx + 1) + (i + 1);

            stripeIndices.push_back(i00); stripeIndices.push_back(i10); stripeIndices.push_back(i01);
            stripeIndices.push_back(i10); stripeIndices.push_back(i11); stripeIndices.push_back(i01);
        }
    }

    uploadMesh(stripeMd, stripeVerts, stripeIndices);
}

void initMeshes()
{
    if (s_meshesInitialized) return;
    buildHullMesh(s_hullMesh);
    buildSailMesh(s_sailMesh, s_sailStripeMesh);
    s_meshesInitialized = true;
}

} // anonymous namespace

namespace Boat {

void cleanup()
{
    if (s_hullMesh.VAO) {
        glDeleteVertexArrays(1, &s_hullMesh.VAO);
        glDeleteBuffers(1, &s_hullMesh.VBO);
        glDeleteBuffers(1, &s_hullMesh.EBO);
        s_hullMesh = MeshData();
    }
    if (s_sailMesh.VAO) {
        glDeleteVertexArrays(1, &s_sailMesh.VAO);
        glDeleteBuffers(1, &s_sailMesh.VBO);
        glDeleteBuffers(1, &s_sailMesh.EBO);
        s_sailMesh = MeshData();
    }
    if (s_sailStripeMesh.VAO) {
        glDeleteVertexArrays(1, &s_sailStripeMesh.VAO);
        glDeleteBuffers(1, &s_sailStripeMesh.VBO);
        glDeleteBuffers(1, &s_sailStripeMesh.EBO);
        s_sailStripeMesh = MeshData();
    }
    s_meshesInitialized = false;
}

void draw(Shader& shader, const mat4& model, bool hasPal, float animTime)
{
    initMeshes();

    // ── Traditional Boat Colors ─────────────────────────────────
    vec3 hullWood    (0.32f, 0.20f, 0.09f); // Dark oiled / tarred sal timber
    vec3 gunwaleWood (0.46f, 0.30f, 0.14f); // Seasoned teak gunwales & thwarts
    vec3 chhoiBamboo (0.64f, 0.54f, 0.32f); // Golden woven split-bamboo matting
    vec3 ribWood     (0.26f, 0.16f, 0.07f); // Inner framing ribs
    vec3 plankWood   (0.40f, 0.26f, 0.12f); // Deck floorboards
    vec3 brassGold   (0.85f, 0.72f, 0.25f); // Polished brass Golui Patti plates
    vec3 bambooPole  (0.66f, 0.56f, 0.32f); // Sturdy bamboo mast & spars
    vec3 bambooNode  (0.35f, 0.24f, 0.12f); // Dark bamboo nodal rings
    vec3 ropeColor   (0.55f, 0.45f, 0.28f); // Coir / jute rigging rope
    vec3 sailCloth   (0.93f, 0.90f, 0.82f); // Unbleached cotton / khadi canvas
    vec3 sailStripe  (0.76f, 0.36f, 0.20f); // Traditional terracotta folk stripe
    vec3 sailPatch   (0.84f, 0.76f, 0.62f); // Reinforced corner patches

    // ── 1. Continuous Flared Timber Hull ─────────────────────────
    Texture::bind(TEX_WOOD, 0);
    shader.setInt("uTextureType", (int)TEX_WOOD);
    drawMesh(s_hullMesh, shader, model, hullWood);

    // ── 2. Sculpted Bow Beak (Golui / গলুই) with Brass Patti ──────
    // Extends forward and sweeps upward from Z = 2.45m to 2.75m
    mat4 goluiBlock = model;
    goluiBlock = translate(goluiBlock, vec3(0.0f, 0.46f, 2.58f));
    goluiBlock = rotate(goluiBlock, radians(-14.0f), vec3(1.0f, 0.0f, 0.0f));
    goluiBlock = scale(goluiBlock, vec3(0.09f, 0.12f, 0.32f));
    Primitives::drawCube(shader, goluiBlock, gunwaleWood);

    // Pointed beak tip
    mat4 goluiTip = model;
    goluiTip = translate(goluiTip, vec3(0.0f, 0.51f, 2.72f));
    goluiTip = rotate(goluiTip, radians(78.0f), vec3(1.0f, 0.0f, 0.0f));
    goluiTip = scale(goluiTip, vec3(0.045f, 0.18f, 0.045f));
    Primitives::drawCone(shader, goluiTip, gunwaleWood);

    // Brass protective cap plate (Golui Patti)
    mat4 goluiPatti = model;
    goluiPatti = translate(goluiPatti, vec3(0.0f, 0.50f, 2.68f));
    goluiPatti = rotate(goluiPatti, radians(-14.0f), vec3(1.0f, 0.0f, 0.0f));
    goluiPatti = scale(goluiPatti, vec3(0.096f, 0.025f, 0.08f));
    Primitives::drawCube(shader, goluiPatti, brassGold);

    // Auspicious Painted Boat Eyes (Noukar Chokh / নৌকার চোখ)
    // Famous folk tradition: painted eyes near the prow guide the craft
    for (int side = -1; side <= 1; side += 2) {
        float sx = (float)side * 0.082f;
        // White sclera almond
        mat4 eyeSclera = model;
        eyeSclera = translate(eyeSclera, vec3(sx, 0.42f, 2.44f));
        eyeSclera = rotate(eyeSclera, radians((float)side * 18.0f), vec3(0.0f, 1.0f, 0.0f));
        eyeSclera = scale(eyeSclera, vec3(0.012f, 0.038f, 0.065f));
        Primitives::drawSphere(shader, eyeSclera, vec3(0.95f, 0.95f, 0.95f));

        // Dark pupil
        mat4 eyePupil = model;
        eyePupil = translate(eyePupil, vec3(sx * 1.05f, 0.42f, 2.44f));
        eyePupil = rotate(eyePupil, radians((float)side * 18.0f), vec3(0.0f, 1.0f, 0.0f));
        eyePupil = scale(eyePupil, vec3(0.014f, 0.020f, 0.028f));
        Primitives::drawSphere(shader, eyePupil, vec3(0.08f, 0.08f, 0.08f));

        // Crimson iris dot
        mat4 eyeDot = model;
        eyeDot = translate(eyeDot, vec3(sx * 1.08f, 0.425f, 2.435f));
        eyeDot = scale(eyeDot, vec3(0.016f, 0.008f, 0.008f));
        Primitives::drawSphere(shader, eyeDot, vec3(0.85f, 0.15f, 0.10f));
    }

    // ── 3. Swept Stern (Pachha) & Raised Boatman Seat (Gura) ──────
    mat4 sternBlock = model;
    sternBlock = translate(sternBlock, vec3(0.0f, 0.40f, -2.48f));
    sternBlock = rotate(sternBlock, radians(16.0f), vec3(1.0f, 0.0f, 0.0f));
    sternBlock = scale(sternBlock, vec3(0.09f, 0.11f, 0.28f));
    Primitives::drawCube(shader, sternBlock, gunwaleWood);

    // Stern finial cap
    mat4 sternCap = model;
    sternCap = translate(sternCap, vec3(0.0f, 0.44f, -2.58f));
    sternCap = scale(sternCap, vec3(0.08f, 0.04f, 0.08f));
    Primitives::drawSphere(shader, sternCap, brassGold);

    // Elevated stern thwart / seat (Pachhar Gura) where Majhi sits
    mat4 sternSeat = model;
    sternSeat = translate(sternSeat, vec3(0.0f, 0.14f, -1.30f));
    sternSeat = scale(sternSeat, vec3(0.72f, 0.035f, 0.32f));
    Primitives::drawCube(shader, sternSeat, gunwaleWood);

    // ── 4. Wooden Cross-Beams / Thwarts (Goor / গুঁড়) ─────────────
    // Sturdy horizontal timbers tying the port and starboard gunwales together
    const float thwartsZ[] = { 1.80f, 1.15f, 0.45f, -0.45f };
    const float thwartsW[] = { 0.62f, 0.88f, 0.98f,  0.96f };
    const float thwartsY[] = { 0.28f, 0.22f, 0.20f,  0.20f };

    for (int t = 0; t < 4; ++t) {
        mat4 thwartM = model;
        thwartM = translate(thwartM, vec3(0.0f, thwartsY[t], thwartsZ[t]));
        thwartM = scale(thwartM, vec3(thwartsW[t], 0.035f, 0.065f));
        Primitives::drawCube(shader, thwartM, gunwaleWood);
    }

    // Mast-step reinforcement collar (at Z = 1.15m)
    mat4 mastCollar = model;
    mastCollar = translate(mastCollar, vec3(0.0f, 0.23f, 1.15f));
    mastCollar = scale(mastCollar, vec3(0.14f, 0.045f, 0.14f));
    Primitives::drawCube(shader, mastCollar, ribWood);

    // ── 5. Recessed Floorboards (Patan / পাটাতন) ───────────────────
    mat4 floorM = model;
    floorM = translate(floorM, vec3(0.0f, 0.02f, 0.0f));
    floorM = scale(floorM, vec3(0.48f, 0.024f, 2.60f));
    Primitives::drawCube(shader, floorM, plankWood);

    // Floorboard center divider seam
    mat4 floorSeam = model;
    floorSeam = translate(floorSeam, vec3(0.0f, 0.033f, 0.0f));
    floorSeam = scale(floorSeam, vec3(0.012f, 0.005f, 2.58f));
    Primitives::drawCube(shader, floorSeam, ribWood);

    // ── 6. Internal Curved Framing Ribs (Bata / Dara) ────────────
    for (int r = -3; r <= 3; ++r) {
        float rz = (float)r * 0.42f;
        float factor = sqrtf(fmaxf(0.0f, 1.0f - (rz * rz) / (2.4f * 2.4f)));
        float ribW = 0.62f * factor;
        float ribY = -0.06f + 0.14f * (1.0f - factor);

        mat4 rib = model;
        rib = translate(rib, vec3(0.0f, ribY, rz));
        rib = scale(rib, vec3(ribW, 0.028f, 0.038f));
        Primitives::drawCube(shader, rib, ribWood);
    }

    // ── 7. Iconic Arched Bamboo Canopy (Chhoi / ছই) ───────────────
    // Spans the mid-rear section from Z = -0.65m to +0.65m
    mat4 chhoi = model;
    chhoi = translate(chhoi, vec3(0.0f, 0.18f, 0.0f));
    chhoi = scale(chhoi, vec3(0.92f, 0.62f, 1.30f));
    Primitives::drawArch(shader, chhoi, chhoiBamboo);

    // Bamboo framing arch hoops (front, center, and back)
    const float hoopZ[] = { -0.65f, -0.22f, 0.22f, 0.65f };
    for (int h = 0; h < 4; ++h) {
        mat4 hoop = model;
        hoop = translate(hoop, vec3(0.0f, 0.18f, hoopZ[h]));
        hoop = scale(hoop, vec3(0.94f, 0.635f, 0.045f));
        Primitives::drawArch(shader, hoop, gunwaleWood);
    }

    // Longitudinal split-bamboo purlin battens
    mat4 ridgeBatten = model;
    ridgeBatten = translate(ridgeBatten, vec3(0.0f, 0.495f, 0.0f));
    ridgeBatten = scale(ridgeBatten, vec3(0.035f, 0.025f, 1.34f));
    Primitives::drawCube(shader, ridgeBatten, gunwaleWood);

    for (int side = -1; side <= 1; side += 2) {
        mat4 flankBatten = model;
        flankBatten = translate(flankBatten, vec3((float)side * 0.42f, 0.32f, 0.0f));
        flankBatten = scale(flankBatten, vec3(0.025f, 0.025f, 1.32f));
        Primitives::drawCube(shader, flankBatten, gunwaleWood);
    }

    // ── 8. Traditional Bengali Hariken (Kerosene Hurricane Lantern)
    // Suspended under the forward arch hoop of the Chhoi at Z = 0.72m
    mat4 boatLantern = model;
    boatLantern = translate(boatLantern, vec3(0.0f, 0.28f, 0.72f));
    boatLantern = scale(boatLantern, vec3(0.82f, 0.82f, 0.82f));
    Charpai::drawLantern(shader, boatLantern);

    // Wire hanger suspension
    mat4 hanger = model;
    hanger = translate(hanger, vec3(0.0f, 0.50f, 0.72f));
    hanger = scale(hanger, vec3(0.008f, 0.075f, 0.008f));
    Primitives::drawCylinder(shader, hanger, vec3(0.25f, 0.25f, 0.25f));

    // ── 9. Iconic Billowing Sail (Pal / পাল) & Bamboo Mast ────────
    if (hasPal) {
        // Bamboo Mast (Mastul / বাঁশের মাস্তুল)
        // Stepped at Z = 1.15m, rising gracefully into the sky to Y = 3.35m
        const float mastBaseY = 0.14f;
        const float mastHeight = 3.20f;
        const float mastZ = 1.15f;

        mat4 mast = model;
        mast = translate(mast, vec3(0.0f, mastBaseY + mastHeight * 0.5f, mastZ));
        mast = scale(mast, vec3(0.065f, mastHeight, 0.065f));
        Primitives::drawCylinder(shader, mast, bambooPole);

        // Bamboo nodal rings along mast
        for (int node = 1; node <= 6; ++node) {
            float nodeY = mastBaseY + (float)node * 0.48f;
            mat4 nRing = model;
            nRing = translate(nRing, vec3(0.0f, nodeY, mastZ));
            nRing = scale(nRing, vec3(0.076f, 0.022f, 0.076f));
            Primitives::drawCylinder(shader, nRing, bambooNode);
        }

        // Masthead finial / truck block
        mat4 mastTop = model;
        mastTop = translate(mastTop, vec3(0.0f, mastBaseY + mastHeight + 0.03f, mastZ));
        mastTop = scale(mastTop, vec3(0.085f, 0.06f, 0.085f));
        Primitives::drawCylinder(shader, mastTop, gunwaleWood);

        // Gentle river breeze dynamic wind sway for the billowing sail assembly
        float sailSwayY = (animTime > 0.0f) ? (sinf(animTime * 2.2f) * radians(1.5f)) : 0.0f;
        float sailSwayX = (animTime > 0.0f) ? (cosf(animTime * 1.8f) * radians(1.2f)) : 0.0f;

        mat4 sailGroup = model;
        sailGroup = translate(sailGroup, vec3(0.0f, 2.20f, mastZ));
        sailGroup = rotate(sailGroup, sailSwayY, vec3(0.0f, 1.0f, 0.0f));
        sailGroup = rotate(sailGroup, sailSwayX, vec3(1.0f, 0.0f, 0.0f));
        sailGroup = translate(sailGroup, vec3(0.0f, -2.20f, -mastZ));

        // Horizontal Yard Spar (Dondi / ডান্ডি)
        // Spans horizontally across the top of the sail at Y = 3.08m
        mat4 yard = sailGroup;
        yard = translate(yard, vec3(0.0f, 3.08f, mastZ + 0.03f));
        yard = rotate(yard, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
        yard = scale(yard, vec3(0.045f, 2.30f, 0.045f));
        Primitives::drawCylinder(shader, yard, bambooPole);

        // Lower boom / foot spar along bottom of sail
        mat4 boom = sailGroup;
        boom = translate(boom, vec3(0.0f, 1.28f, mastZ + 0.03f));
        boom = rotate(boom, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
        boom = scale(boom, vec3(0.032f, 2.14f, 0.032f));
        Primitives::drawCylinder(shader, boom, bambooPole);

        // Draw Billowing Sail Cloth (Pal) in natural unbleached handloom cotton / canvas
        shader.setInt("uUseTexture", 0);
        shader.setInt("uTextureType", 0);
        drawMesh(s_sailMesh, shader, sailGroup, sailCloth);

        // Traditional Terracotta Folk Stripe Band across midsection
        drawMesh(s_sailStripeMesh, shader, sailGroup, sailStripe);

        // Reinforced Corner Patches (Kona Patti) at the 4 sail corners
        const float cX[] = { -1.02f, 1.02f, -1.02f, 1.02f };
        const float cY[] = {  3.03f, 3.03f,  1.32f, 1.32f };
        for (int c = 0; c < 4; ++c) {
            mat4 patch = sailGroup;
            patch = translate(patch, vec3(cX[c], cY[c], mastZ + 0.035f));
            patch = scale(patch, vec3(0.12f, 0.10f, 0.015f));
            Primitives::drawCube(shader, patch, sailPatch);
        }

        // Restore texture mode
        shader.setInt("uUseTexture", 2);
        Texture::bind(TEX_WOOD, 0);
        shader.setInt("uTextureType", (int)TEX_WOOD);

        // ── Rigging & Ropes (Boal / Kasi / Rashi / কাছি) ─────────────
        const vec3 mastHead(0.0f, mastBaseY + mastHeight, mastZ);

        // 1. Forestay: runs from masthead straight down to bow Golui base
        drawRope(shader, model, mastHead, vec3(0.0f, 0.48f, 2.50f), 0.008f, ropeColor);

        // 2. Port Shroud Stay: anchors masthead down to port gunwale
        drawRope(shader, model, mastHead, vec3(-0.48f, 0.24f, 0.40f), 0.008f, ropeColor);

        // 3. Starboard Shroud Stay: anchors masthead down to starboard gunwale
        drawRope(shader, model, mastHead, vec3(0.48f, 0.24f, 0.40f), 0.008f, ropeColor);

        // 4. Port Sheet Line (Kasi): ties bottom-left sail corner down to thwart
        drawRope(shader, sailGroup, vec3(-1.05f, 1.28f, mastZ + 0.03f),
                 vec3(-0.45f, 0.24f, -0.05f), 0.007f, ropeColor);

        // 5. Starboard Sheet Line (Kasi): ties bottom-right sail corner down to thwart
        drawRope(shader, sailGroup, vec3(1.05f, 1.28f, mastZ + 0.03f),
                 vec3(0.45f, 0.24f, -0.05f), 0.007f, ropeColor);

        // 6. Halyard Lashing: twisted loops binding the yard spar to the masthead
        mat4 halyardLash = model;
        halyardLash = translate(halyardLash, vec3(0.0f, 3.08f, mastZ + 0.02f));
        halyardLash = scale(halyardLash, vec3(0.09f, 0.07f, 0.09f));
        Primitives::drawCylinder(shader, halyardLash, ropeColor);

        // Restore wood texture mode
        Texture::bind(TEX_WOOD, 0);
        shader.setInt("uTextureType", (int)TEX_WOOD);
    }
}

} // namespace Boat
