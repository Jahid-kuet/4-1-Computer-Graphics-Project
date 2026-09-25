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

// 2. Procedural Running-Bond Clay Brick & Mortar Texture (Mosque, roads, platforms)
void generateBrickTexture(std::vector<uint8_t>& data)
{
    data.resize(TEX_SIZE * TEX_SIZE * 4);
    const int brickH = 32;
    const int brickW = 64;
    const int mortarThickness = 4;

    for (int y = 0; y < TEX_SIZE; ++y) {
        int row = y / brickH;
        int yInBrick = y % brickH;
        int xOffset = (row % 2 == 1) ? (brickW / 2) : 0;

        for (int x = 0; x < TEX_SIZE; ++x) {
            int xInBrick = (x + xOffset) % brickW;

            bool isMortar = (yInBrick < mortarThickness) || (xInBrick < mortarThickness);

            int idx = (y * TEX_SIZE + x) * 4;
            if (isMortar) {
                // Light grey-cream cement mortar joint with subtle grain
                float noise = (float)((x * 17 + y * 31) % 19) / 19.0f * 15.0f;
                data[idx + 0] = toByte(190.0f + noise);
                data[idx + 1] = toByte(185.0f + noise);
                data[idx + 2] = toByte(175.0f + noise);
            } else {
                // Warm terracotta red-orange clay brick with kiln variations
                float noise = (float)((x * 37 + y * 73) % 23) / 23.0f * 24.0f;
                data[idx + 0] = toByte(175.0f + noise);
                data[idx + 1] = toByte( 75.0f + noise * 0.5f);
                data[idx + 2] = toByte( 48.0f + noise * 0.3f);
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
