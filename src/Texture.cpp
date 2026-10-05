// Texture.cpp — Procedural 2D Texture Generator & OpenGL Texture Manager.
// Generates 256x256 RGBA procedural textures and uploads them to the GPU.

#include "Texture.h"
#include <vector>
#include <cmath>
#include <algorithm>

namespace {

GLuint g_textures[4] = { 0, 0, 0, 0 };
const int TEX_SIZE = 256;

// Helper: Clamps a float to 0..255 and returns uint8_t
inline uint8_t toByte(float v) {
    return static_cast<uint8_t>(std::clamp(v, 0.0f, 255.0f));
}

// 1. Procedural Wood Grain Texture (Timber boards, boat hull, charpai)
void generateWoodTexture(std::vector<uint8_t>& data)
{
    data.resize(TEX_SIZE * TEX_SIZE * 4);
    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            float u = (float)x / (float)TEX_SIZE;
            float v = (float)y / (float)TEX_SIZE;

            // Concentric distorted tree growth rings
            float dx = (u - 0.5f) * 3.5f;
            float dy = (v - 0.5f) * 0.9f;
            float dist = std::sqrt(dx * dx + dy * dy);
            float rings = 0.5f + 0.5f * std::sin(dist * 45.0f + 0.9f * std::sin(u * 16.0f));

            // Fine longitudinal wood fiber streaks
            float fibers = 0.5f + 0.5f * std::sin(v * 240.0f + u * 40.0f);

            // Blend rings and grain fibers
            float pattern = rings * 0.70f + fibers * 0.30f;

            // Warm seasoned Bengali timber palette (dark Sal / Teak wood)
            float r = 110.0f * (0.65f + 0.35f * pattern);
            float g =  68.0f * (0.65f + 0.35f * pattern);
            float b =  32.0f * (0.65f + 0.35f * pattern);

            int idx = (y * TEX_SIZE + x) * 4;
            data[idx + 0] = toByte(r);
            data[idx + 1] = toByte(g);
            data[idx + 2] = toByte(b);
            data[idx + 3] = 255;
        }
    }
}

// 2. Procedural Running-Bond Clay Brick & Mortar Texture with Ancient Moss, Algae & Slime Weathering
void generateBrickTexture(std::vector<uint8_t>& data)
{
    data.resize(TEX_SIZE * TEX_SIZE * 4);
    const int brickH = 32;
    const int brickW = 64;
    const int mortarThickness = 4;

    // Helper lambda for pseudo-random hash
    auto hash2D = [](int ix, int iy, uint32_t seed) -> float {
        uint32_t h = (uint32_t)(ix * 374761393 + iy * 668265263) ^ seed;
        h = (h ^ (h >> 13)) * 1274126177;
        return (float)(h & 0x7FFFFFFF) / 2147483647.0f;
    };

    for (int y = 0; y < TEX_SIZE; ++y) {
        int row = y / brickH;
        int yInBrick = y % brickH;
        int xOffset = (row % 2 == 1) ? (brickW / 2) : 0;
        float yNorm = (float)y / (float)TEX_SIZE;

        for (int x = 0; x < TEX_SIZE; ++x) {
            int col = (x + xOffset) / brickW;
            int xInBrick = (x + xOffset) % brickW;

            bool isMortar = (yInBrick < mortarThickness) || (xInBrick < mortarThickness);
            bool isEdge   = (!isMortar) && (yInBrick < mortarThickness + 2 || xInBrick < mortarThickness + 2 ||
                                            yInBrick > brickH - 3 || xInBrick > brickW - 3);

            // Per-brick unique identity & kiln firing variance
            float brickRand = hash2D(col, row, 1013904223);
            float brickMossProne = hash2D(col, row, 439281741);

            // Micro-grain noise
            float microGrain = hash2D(x, y, 73856093);

            // Coarse organic moisture patches (smooth cellular blend)
            int cellX = x / 48;
            int cellY = y / 48;
            float cellNoise = hash2D(cellX, cellY, 982451653);
            float moistureField = 0.40f * cellNoise + 0.35f * hash2D(x / 24, y / 24, 555555) + 0.25f * microGrain;

            // Height-biased dampness: lower rows collect more moisture & biological growth
            float groundDampness = std::clamp((0.75f - yNorm) * 1.4f, 0.0f, 1.0f);
            float totalBiological = moistureField * 0.55f + groundDampness * 0.45f;

            int idx = (y * TEX_SIZE + x) * 4;

            if (isMortar) {
                // Base aged lime-surkhi mortar joint (greyish cream with damp dark tones)
                float mr = 168.0f + microGrain * 16.0f;
                float mg = 162.0f + microGrain * 16.0f;
                float mb = 150.0f + microGrain * 14.0f;

                // Mortar is the primary habitat for green algae and velvety moss!
                if (totalBiological > 0.36f || brickMossProne > 0.52f) {
                    float t = std::clamp((totalBiological - 0.36f) * 2.5f + (brickMossProne > 0.65f ? 0.35f : 0.0f), 0.0f, 1.0f);
                    // Rich living algae & damp moss green (Bryophyta)
                    float alR = 48.0f + microGrain * 18.0f;
                    float alG = 88.0f + microGrain * 26.0f;
                    float alB = 36.0f + microGrain * 14.0f;
                    mr = mr * (1.0f - t) + alR * t;
                    mg = mg * (1.0f - t) + alG * t;
                    mb = mb * (1.0f - t) + alB * t;
                } else if (totalBiological > 0.25f) {
                    // Moisture-darkened damp mortar joint
                    mr *= 0.65f;
                    mg *= 0.68f;
                    mb *= 0.62f;
                }

                data[idx + 0] = toByte(mr);
                data[idx + 1] = toByte(mg);
                data[idx + 2] = toByte(mb);
            } else {
                // Warm terracotta red-orange clay brick with individual kiln firing variation
                float br = 165.0f + brickRand * 30.0f + microGrain * 12.0f;
                float bg =  68.0f + brickRand * 18.0f + microGrain *  8.0f;
                float bb =  42.0f + brickRand * 12.0f + microGrain *  6.0f;

                // Brick edges collect moss creeping out of the mortar joints
                if (isEdge && (totalBiological > 0.40f || brickMossProne > 0.58f)) {
                    float edgeMoss = 0.65f + 0.35f * microGrain;
                    float mR = 52.0f + microGrain * 16.0f;
                    float mG = 92.0f + microGrain * 24.0f;
                    float mB = 34.0f + microGrain * 12.0f;
                    br = br * (1.0f - edgeMoss) + mR * edgeMoss;
                    bg = bg * (1.0f - edgeMoss) + mG * edgeMoss;
                    bb = bb * (1.0f - edgeMoss) + mB * edgeMoss;
                }
                // Weathered moss patches & damp slime on porous brick face
                else if (totalBiological > 0.55f && brickMossProne > 0.42f) {
                    float t = std::clamp((totalBiological - 0.55f) * 3.0f, 0.0f, 0.90f);
                    float mR = 55.0f + microGrain * 20.0f;
                    float mG = 96.0f + microGrain * 28.0f;
                    float mB = 38.0f + microGrain * 14.0f;
                    br = br * (1.0f - t) + mR * t;
                    bg = bg * (1.0f - t) + mG * t;
                    bb = bb * (1.0f - t) + mB * t;
                }
                // Moisture staining / damp darkening on lower courses
                else if (totalBiological > 0.42f) {
                    float t = (totalBiological - 0.42f) * 1.8f;
                    br *= (1.0f - 0.35f * t);
                    bg *= (1.0f - 0.28f * t);
                    bb *= (1.0f - 0.22f * t);
                }

                data[idx + 0] = toByte(br);
                data[idx + 1] = toByte(bg);
                data[idx + 2] = toByte(bb);
            }
            data[idx + 3] = 255;
        }
    }
}

// 3. Procedural Woven Bamboo Mat / Thatch Texture (Chatai / Dochala roofing / Fences)
void generateBambooTexture(std::vector<uint8_t>& data)
{
    data.resize(TEX_SIZE * TEX_SIZE * 4);
    const int stripSize = 16;

    for (int y = 0; y < TEX_SIZE; ++y) {
        for (int x = 0; x < TEX_SIZE; ++x) {
            // Diagonal woven bamboo split strips
            int uBlock = (x + y) / stripSize;
            int vBlock = (x - y + TEX_SIZE * 2) / stripSize;
            bool weftActive = ((uBlock + vBlock) % 2 == 0);

            // Longitudinal fiber lines along individual bamboo splits
            float splitPos = (float)((x + y) % stripSize) / (float)stripSize;
            float fiber = 0.75f + 0.25f * std::sin(splitPos * 3.14159f);

            int idx = (y * TEX_SIZE + x) * 4;
            if (weftActive) {
                // Lighter top bamboo strip
                data[idx + 0] = toByte(185.0f * fiber);
                data[idx + 1] = toByte(150.0f * fiber);
                data[idx + 2] = toByte( 82.0f * fiber);
            } else {
                // Darker shaded undercrossing bamboo strip
                data[idx + 0] = toByte(135.0f * fiber);
                data[idx + 1] = toByte(105.0f * fiber);
                data[idx + 2] = toByte( 55.0f * fiber);
            }
            data[idx + 3] = 255;
        }
    }
}

// 4. Procedural Checked Lungi / Gamcha Plaid Fabric Texture (Rural Bengali attire)
void generateFabricTexture(std::vector<uint8_t>& data)
{
    data.resize(TEX_SIZE * TEX_SIZE * 4);
    const int gridSize = 32;
    const int stripeW  = 6;

    for (int y = 0; y < TEX_SIZE; ++y) {
        int yMod = y % gridSize;
        bool yStripe = (yMod < stripeW);
        bool yBorder = (yMod == 0 || yMod == stripeW - 1);

        for (int x = 0; x < TEX_SIZE; ++x) {
            int xMod = x % gridSize;
            bool xStripe = (xMod < stripeW);
            bool xBorder = (xMod == 0 || xMod == stripeW - 1);

            int idx = (y * TEX_SIZE + x) * 4;

            if (xBorder || yBorder) {
                // White accent pinstripe borders
                data[idx + 0] = 235;
                data[idx + 1] = 235;
                data[idx + 2] = 235;
            } else if (xStripe && yStripe) {
                // Dark intersection check (deep navy / charcoal)
                data[idx + 0] = 28;
                data[idx + 1] = 40;
                data[idx + 2] = 68;
            } else if (xStripe || yStripe) {
                // Forest green plaid band
                data[idx + 0] = 38;
                data[idx + 1] = 115;
                data[idx + 2] = 62;
            } else {
                // Rich madder red field check
                data[idx + 0] = 188;
                data[idx + 1] =  42;
                data[idx + 2] =  34;
            }
            data[idx + 3] = 255;
        }
    }
}

// Upload raw RGBA data to an OpenGL 2D texture with mipmapping & linear filtering
GLuint uploadTextureToGPU(const std::vector<uint8_t>& data)
{
    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEX_SIZE, TEX_SIZE, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

} // anonymous namespace

namespace Texture {

void init()
{
    std::vector<uint8_t> data;

    // 0: Wood Grain
    generateWoodTexture(data);
    g_textures[TEX_WOOD] = uploadTextureToGPU(data);

    // 1: Clay Brick
    generateBrickTexture(data);
    g_textures[TEX_BRICK] = uploadTextureToGPU(data);

    // 2: Bamboo Mat / Thatch
    generateBambooTexture(data);
    g_textures[TEX_BAMBOO] = uploadTextureToGPU(data);

    // 3: Checked Fabric (Lungi / Gamcha)
    generateFabricTexture(data);
    g_textures[TEX_FABRIC] = uploadTextureToGPU(data);
}

void cleanup()
{
    for (int i = 0; i < 4; ++i) {
        if (g_textures[i] != 0) {
            glDeleteTextures(1, &g_textures[i]);
            g_textures[i] = 0;
        }
    }
}

void bind(TextureType type, unsigned int unit)
{
    if (type >= 0 && type < 4 && g_textures[type] != 0) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, g_textures[type]);
    }
}

GLuint getID(TextureType type)
{
    if (type >= 0 && type < 4) {
        return g_textures[type];
    }
    return 0;
}

} // namespace Texture
