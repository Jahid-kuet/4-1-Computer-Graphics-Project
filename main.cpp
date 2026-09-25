// main.cpp — Village Gathering by the River: A Bangladeshi Rural Night Scene in 3D
// OpenGL 3.3 Core Profile Computer Graphics Project
// Interactive orbit camera (left-drag = rotate, scroll = zoom, ESC = exit)
// Camera Presets: '1' = Courtyard Gathering, '2' = River & Boat, '3' = Full Overview, '4' = Homestead
// Lighting Toggle: 'L' = Cycle between Moonlit Night (Default), Golden Dusk, and Crisp Day

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <string>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"
#include "Primitives.h"
#include "mathutil.h"

// Object headers
#include "objects/Terrain.h"
#include "objects/River.h"
#include "objects/House.h"
#include "objects/Mosque.h"
#include "objects/Tree.h"
#include "objects/Hen.h"
#include "objects/Duck.h"
#include "objects/Person.h"
#include "objects/Charpai.h"
#include "objects/Boat.h"
#include "objects/Boatman.h"
#include "objects/Moon.h"
#include "objects/Stars.h"
#include "objects/Fireflies.h"
#include "Texture.h"
#include "objects/CurvedObject.h"

using namespace math;

// ─── Globals ─────────────────────────────────────────────────────────
static Camera      camera;
static bool        dragging   = false;
static double      lastMouseX = 0.0, lastMouseY = 0.0;
static int         winWidth   = 1280, winHeight = 720;
static Shader*     g_shader   = nullptr;
static GLFWwindow* g_window   = nullptr;

// Display Modes:
//   0 = Moonlit Bengali Night (Atmospheric rural night, silvery moonlight, glowing lanterns, stars, fireflies - Default)
//   1 = Full Day Scene (Bright sunlight with Blinn-Phong shading)
//   2 = Crisp Unlit 3D Facets (Daylight, 0 darkness, 0 shadows, clear 3D recognition - Lab Milestone Inspection)
//   3 = Pure Flat Object Color (Unshaded)
static int  lightingMode = 0;
static int  g_textureMode = 2; // 0 = Solid Shading, 1 = Procedural GLSL Detailing, 2 = GPU Texture Maps (Default)
static bool showTerrain  = true; // Key 'T' toggles terrain/river visibility
static bool g_tourActive = false; // Key SPACE toggles automated fly-through tour
static bool g_dirLightEnabled = true; // Key 'J' toggles Directional Light (Moonlight/Sunlight)

static void captureAllObjects(GLFWwindow* window, Shader& shader);

// ─── GLFW Callbacks ──────────────────────────────────────────────────
static void framebufferSizeCallback(GLFWwindow*, int w, int h)
{
    winWidth  = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    camera.aspectRatio = (float)w / (float)(h > 0 ? h : 1);
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        dragging = (action == GLFW_PRESS);
        glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
    }
}

static void cursorPosCallback(GLFWwindow*, double xpos, double ypos)
{
    if (dragging) {
        g_tourActive = false; // Mouse drag takes over from tour
        float dx = (float)(xpos - lastMouseX);
        float dy = (float)(lastMouseY - ypos); // inverted Y
        camera.processMouseDrag(dx, dy);
        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

static void scrollCallback(GLFWwindow*, double, double yoffset)
{
    camera.processScroll((float)yoffset);
}

// ─── Math Helpers for Cinematic Tour ──────────────────────────────────
static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
static inline vec3  lerp(const vec3& a, const vec3& b, float t) { return a + (b - a) * t; }

// ─── Cinematic Tour Keyframes & Interactive States ─────────────────────
struct TourKeyframe {
    float time;
    vec3  target;
    float yawDeg;
    float pitchDeg;
    float distance;
};

static const TourKeyframe g_tourFrames[] = {
    {  0.0f, vec3( 0.5f, 1.20f, -6.5f), -12.0f, 14.0f, 19.5f },  // 1: Broader View: Moonlit Village, House, Courtyard, Mosque, River & Moon
    { 10.0f, vec3( 8.5f, 0.40f, -6.0f),  15.0f, 14.0f,  8.5f },  // 2: Cruising Dingi & Rowing Boatman
    { 19.0f, vec3( 5.5f, 0.80f,  1.2f), -40.0f, 16.0f,  9.0f },  // 3: River Landing Ghat & Net Drying Racks
    { 28.0f, vec3(-3.5f, 0.75f,  1.0f),  45.0f, 16.0f,  7.0f },  // 4: Swept Courtyard, Elders, Children & Tubewell
    { 38.0f, vec3(-3.8f, 1.90f,-17.5f),  10.0f, 16.0f, 12.0f },  // 5: Historic Terracotta Village Mosque (Masjid)
    { 48.0f, vec3(-15.0f, 1.20f,-5.5f),  40.0f, 18.0f, 11.0f },  // 6: North Homestead, Cow Shed & Deshi Cow
    { 58.0f, vec3(-8.0f, 1.50f,  5.0f),  25.0f, 32.0f, 34.0f },  // 7: Terraced Paddy Fields, Dancing Jonaki & Moon
    { 68.0f, vec3( 0.5f, 1.20f, -6.5f), -12.0f, 14.0f, 19.5f }   // Loop back to broader view
};
static const int   g_numTourFrames = 8;
static const float g_tourDuration  = 68.0f;
static float       g_tourTime      = 0.0f;

// Interactive Controls State
static float       g_pumpTimer     = 0.0f; // Interactive Tubewell pumping
static int         g_lanternMode   = 0;    // 0: Normal, 1: High Flame, 2: Soft, 3: Extinguished
static float       g_animSpeed     = 1.0f; // Animation speed multiplier
static bool        g_windBreeze    = true; // Summer breeze wind sway

static void keyCallback(GLFWwindow* window, int key, int, int action, int)
{
    if (action != GLFW_PRESS) return;

    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }
    // SPACE: Toggle Cinematic Village Fly-Through Tour Mode
    else if (key == GLFW_KEY_SPACE) {
        g_tourActive = !g_tourActive;
        if (g_tourActive) {
            std::cout << "\n=======================================================\n";
            std::cout << ">> CINEMATIC FLY-THROUGH TOUR: ACTIVE\n";
            std::cout << "   Smooth automated camera tour through all village areas.\n";
            std::cout << "   Press [SPACE] or drag mouse to resume manual control.\n";
            std::cout << "=======================================================\n";
        } else {
            std::cout << ">> CINEMATIC TOUR: PAUSED (Manual camera control)\n";
        }
    }
    // Key 'P': Interactive Tubewell Pumping Action
    else if (key == GLFW_KEY_P) {
        g_pumpTimer = 3.6f;
        std::cout << ">> TUBEWELL PUMP ACTIVATED! Pumping groundwater into clay Kolshi! [Water flowing!]\n";
    }
    // Key 'J': Toggle Directional Light (Moonlight / Sunlight) On / Off
    else if (key == GLFW_KEY_J) {
        g_dirLightEnabled = !g_dirLightEnabled;
        std::cout << ">> DIRECTIONAL LIGHT (Moonlight / Sunlight): "
                  << (g_dirLightEnabled ? "ENABLED (Silvery Moonlight Blending)" : "DISABLED (Pure Point Light Inspection!)")
                  << std::endl;
    }
    // Key 'H': Toggle 6 Point Lights (Courtyard, Moored Boat, Cruising Boat, Mosque, Stove, Ghat)
    else if (key == GLFW_KEY_H) {
        g_lanternMode = (g_lanternMode + 1) % 4;
        const char* lanternNames[] = {
            "Normal Golden Glow (1.0x - All 6 Point Lights Active)",
            "Extra Bright Blazing Flame (1.85x - Maximum Illumination)",
            "Soft Low Amber Glow (0.45x - Gentle Nocturnal Ambiance)",
            "Extinguished (Point Lights OFF - Pure Directional Moonlight / Sunlight!)"
        };
        std::cout << ">> 6 POINT LIGHTS: " << lanternNames[g_lanternMode] << std::endl;
    }
    // Key 'B': Toggle Summer Breeze Wind Sway
    else if (key == GLFW_KEY_B) {
        g_windBreeze = !g_windBreeze;
        std::cout << ">> SUMMER BREEZE (Tree Wind Sway): " << (g_windBreeze ? "ACTIVE" : "CALM") << std::endl;
    }
    // Key '[' / ']': Animation Speed Control
    else if (key == GLFW_KEY_LEFT_BRACKET) {
        g_animSpeed = std::max(0.20f, g_animSpeed - 0.25f);
        std::cout << ">> ANIMATION SPEED: " << g_animSpeed << "x (Slow-motion)\n";
    }
    else if (key == GLFW_KEY_RIGHT_BRACKET) {
        g_animSpeed = std::min(3.0f, g_animSpeed + 0.25f);
        std::cout << ">> ANIMATION SPEED: " << g_animSpeed << "x (Fast-forward)\n";
    }
    // Key 'K': Pause / Resume Animation
    else if (key == GLFW_KEY_K) {
        g_animSpeed = (g_animSpeed > 0.01f) ? 0.0f : 1.0f;
        std::cout << ">> ANIMATION: " << (g_animSpeed > 0.01f ? "RESUMED" : "PAUSED") << std::endl;
    }
    // Lighting Mode Toggle
    else if (key == GLFW_KEY_L) {
        lightingMode = (lightingMode + 1) % 4;
        const char* modeNames[] = {
            "0: Moonlit Bengali Night (Atmospheric rural night, silvery moonlight, stars, fireflies, lanterns & stove embers - Default)",
            "1: Full Day Scene (Bright sunlight with Blinn-Phong shading)",
            "2: Crisp Unlit 3D Facets (Daylight, 0 darkness, 0 shadows - Lab Geometry Inspection)",
            "3: Pure Flat Object Color (Unshaded)"
        };
        std::cout << "Display Mode: " << modeNames[lightingMode] << std::endl;
    }
    // Terrain / Ground Visibility Toggle
    else if (key == GLFW_KEY_T) {
        showTerrain = !showTerrain;
        std::cout << "Terrain / Ground: " << (showTerrain ? "VISIBLE" : "HIDDEN (Freestanding Objects Only)") << std::endl;
    }
    // Camera Presets for Village Inspection
    else if (key == GLFW_KEY_1) {
        g_tourActive = false;
        camera.target   = vec3(-3.5f, 0.75f, 1.0f);
        camera.yaw      = radians(38.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 7.2f;
        camera.updatePosition();
        std::cout << "View 1: Courtyard Gathering (Charpai, Seated Elder with Fan, Neighbor, Children, Hens & Lantern)\n";
    }
    else if (key == GLFW_KEY_2) {
        g_tourActive = false;
        camera.target   = vec3(7.5f, 0.70f, -2.5f);
        camera.yaw      = radians(-25.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 12.5f;
        camera.updatePosition();
        std::cout << "View 2: River Shore, Landing Ghat, Moored Boat, Rowing Boatman & Fishing Net Racks\n";
    }
    else if (key == GLFW_KEY_3) {
        g_tourActive = false;
        camera.target   = vec3(-3.8f, 1.9f, -17.5f);
        camera.yaw      = radians(10.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 12.0f;
        camera.updatePosition();
        std::cout << "View 3: Historic Terracotta Village Mosque (Ancient Sultanate & Mughal Bengal Terracotta Brick Masjid with Curved Cornice, Octagonal Towers, Grand Dome & Paka Houz)\n";
    }
    else if (key == GLFW_KEY_4) {
        g_tourActive = false;
        camera.target   = vec3(-14.5f, 1.4f, -6.5f);
        camera.yaw      = radians(38.0f);
        camera.pitch    = radians(18.0f);
        camera.distance = 12.0f;
        camera.updatePosition();
        std::cout << "View 4: North Homestead (Dochala House, Thatched Cow Shed with Cow & Straw Stack)\n";
    }
    else if (key == GLFW_KEY_5) {
        g_tourActive = false;
        camera.target   = vec3(-10.5f, 1.2f, 17.5f);
        camera.yaw      = radians(28.0f);
        camera.pitch    = radians(18.0f);
        camera.distance = 15.0f;
        camera.updatePosition();
        std::cout << "View 5: South Homestead, Rice Straw Stack & Terraced Paddy Fields (Dhan Khet with Rice Clumps & Aal)\n";
    }
    else if (key == GLFW_KEY_6) {
        g_tourActive = false;
        camera.target   = vec3(1.5f, 1.6f, -4.5f);
        camera.yaw      = radians(-15.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 14.5f;
        camera.updatePosition();
        std::cout << "View 6: Rural Trees & Riverbank Reeds (Palms, Banana, Mango, Bamboo Groves & Kashbon)\n";
    }
    else if (key == GLFW_KEY_7) {
        g_tourActive = false;
        // If pressed during night scene, switch to daytime mode so animals are active and clearly visible!
        if (lightingMode == 0) {
            lightingMode = 1;
            std::cout << "[Switched to Daytime Mode for Animal Inspection]\n";
        }
        camera.target   = vec3(-4.5f, 0.4f, 0.5f);
        camera.yaw      = radians(25.0f);
        camera.pitch    = radians(14.0f);
        camera.distance = 6.5f;
        camera.updatePosition();
        std::cout << "View 7: Village Animals (Free-Range Pecking Hens & River Ducks Swimming in Water)\n";
    }
    else if (key == GLFW_KEY_8) {
        g_tourActive = false;
        camera.target   = vec3(0.5f, 1.2f, -6.5f);
        camera.yaw      = radians(-12.0f);
        camera.pitch    = radians(14.0f);
        camera.distance = 19.5f;
        camera.updatePosition();
        std::cout << "View 8: Broader View - Moonlit Rural Village & River Overview (House, Courtyard, Charpai, Mosque, River & Full Moon)\n";
    }
    else if (key == GLFW_KEY_9) {
        g_tourActive = false;
        camera.target   = vec3(-5.2f, 0.7f, -2.4f);
        camera.yaw      = radians(42.0f);
        camera.pitch    = radians(18.0f);
        camera.distance = 5.8f;
        camera.updatePosition();
        std::cout << "View 9: Village Tubewell & Courtyard Kitchen (Chapa Kol, Pitcher, Clay Stove & Pots - Press [P] to Pump!)\n";
    }
    else if (key == GLFW_KEY_0) {
        g_tourActive = false;
        camera.target   = vec3(-17.5f, 0.8f, -3.5f);
        camera.yaw      = radians(20.0f);
        camera.pitch    = radians(14.0f);
        camera.distance = 5.5f;
        camera.updatePosition();
        std::cout << "View 0: Thatched Cow Shed & Resting Deshi Cow (Gowal Ghor with Hay Trough & Humped Cattle)\n";
    }
    // Key 'X': Toggle Texture Mode (0: Solid Color, 1: Procedural Detailing, 2: GPU Texture Maps)
    else if (key == GLFW_KEY_X) {
        g_textureMode = (g_textureMode + 1) % 3;
        const char* texModeNames[] = {
            "0: Solid Color (Clean Geometry Shading)",
            "1: Procedural GLSL Detailing (Real-time GPU Micro-Grain & Fibers)",
            "2: GPU Texture Maps (Wood, Brick, Bamboo/Thatch, Fabric)"
        };
        std::cout << ">> TEXTURE MODE: " << texModeNames[g_textureMode] << std::endl;
    }
    // Key 'V': Inspect Curved Bamboo Footbridge & Bézier Terracotta Surahi
    else if (key == GLFW_KEY_V) {
        g_tourActive = false;
        camera.target   = vec3(7.6f, 1.1f, -14.5f);
        camera.yaw      = radians(35.0f);
        camera.pitch    = radians(18.0f);
        camera.distance = 11.0f;
        camera.updatePosition();
        std::cout << "View V: Curved Arched Bamboo Footbridge (Bansher Saako / বাঁশের সাঁকো) Spanning River Inlet & Bézier Surahi\n";
    }
    // Key 'C': Capture All Objects
    else if (key == GLFW_KEY_C) {
        if (g_window && g_shader) {
            captureAllObjects(g_window, *g_shader);
        }
    }
}

static void glfwErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error [" << error << "]: " << description << std::endl;
}

// ─── BMP Image Exporter ──────────────────────────────────────────────
#pragma pack(push, 1)
struct BMPHeader {
    uint16_t fileType{0x4D42}; // 'BM'
    uint32_t fileSize{0};
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t offsetData{54};

    uint32_t size{40};
    int32_t  width{0};
    int32_t  height{0};
    uint16_t planes{1};
    uint16_t bitCount{24};
    uint32_t compression{0};
    uint32_t sizeImage{0};
    int32_t  xPixelsPerMeter{2835};
    int32_t  yPixelsPerMeter{2835};
    uint32_t colorsUsed{0};
    uint32_t colorsImportant{0};
};
#pragma pack(pop)

static bool saveBMP(const std::string& filename, int width, int height)
{
    int rowPadding = (4 - (width * 3) % 4) % 4;
    int stride = width * 3 + rowPadding;
    std::vector<uint8_t> pixels(width * height * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());

    BMPHeader header;
    header.width = width;
    header.height = height;
    header.sizeImage = stride * height;
    header.fileSize = 54 + header.sizeImage;

    std::ofstream out(filename, std::ios::binary);
    if (!out) return false;

    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    for (int y = 0; y < height; ++y) {
        out.write(reinterpret_cast<const char*>(&pixels[y * width * 3]), width * 3);
        if (rowPadding > 0) {
            uint8_t pad[3] = {0, 0, 0};
            out.write(reinterpret_cast<const char*>(pad), rowPadding);
        }
    }
    return true;
}

struct ObjectCaptureItem {
    std::string name;
    std::string title;
    vec3 target;
    float distance;
    float yawDeg;
    float pitchDeg;
    std::function<void(Shader&)> drawFunc;
};

static void captureAllObjects(GLFWwindow* window, Shader& shader)
{
    std::cout << "\n========================================================\n";
    std::cout << "  CAPTURING ALL 30 INDIVIDUAL OBJECTS INTO 'object_images/'\n";
    std::cout << "========================================================\n";

    std::filesystem::create_directories("object_images");

    int fbW = 0, fbH = 0;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    if (fbW <= 0 || fbH <= 0) {
        fbW = 1280; fbH = 720;
    }
    glViewport(0, 0, fbW, fbH);
    camera.aspectRatio = (float)fbW / (float)fbH;

    // Clean Studio Backdrop & Lighting Setup
    vec3 clearColor(0.92f, 0.93f, 0.95f);
    vec3 lightDir = normalize(vec3(0.35f, -0.85f, -0.40f));
    vec3 lightColor(1.0f, 1.0f, 1.0f);

    std::vector<ObjectCaptureItem> items = {
        {
            "01_house_chouchala",
            "Traditional Chouchala House (4-Sloped Roof, Verandah & Mud Walls)",
            vec3(0.0f, 1.10f, 0.20f), 8.8f, 36.0f, 18.0f,
            [](Shader& s) {
                House::draw(s, mat4::identity(), HOUSE_CHOUCHALA);
            }
        },
        {
            "02_clay_cooking_stove",
            "Outdoor Clay Cooking Stove (Matir Chula with 3-Sided Bamboo Fence)",
            vec3(0.08f, 0.20f, 0.16f), 1.80f, 18.0f, 20.0f,
            [](Shader& s) {
                House::drawStove(s, mat4::identity(), true);
            }
        },
        {
            "03_clay_water_pitchers",
            "Terracotta Water Pitchers (Matir Kolshi)",
            vec3(0.0f, 0.22f, 0.0f), 1.20f, 26.0f, 16.0f,
            [](Shader& s) {
                House::drawKolshi(s, mat4::identity(), vec3(-0.22f, 0.0f, 0.0f), 1.0f);
                House::drawKolshi(s, mat4::identity(), vec3(0.20f, 0.0f, 0.10f), 0.82f);
            }
        },
        {
            "04_dingi_boat",
            "Traditional Pal Tola Dingi Nouka (Bangladeshi Country Sailboat)",
            vec3(0.0f, 1.40f, 0.0f), 5.6f, 34.0f, 16.0f,
            [](Shader& s) {
                Boat::draw(s, mat4::identity());
            }
        },
        {
            "05_hariken_lantern",
            "Rural Bengali Kerosene Hurricane Lantern (Hariken)",
            vec3(0.0f, 0.20f, 0.0f), 0.64f, 25.0f, 15.0f,
            [](Shader& s) {
                Charpai::drawLantern(s, mat4::identity());
            }
        },
        {
            "06_boatman_majhi",
            "Seated Boatman (Majhi with Mathal Hat & Boitha Oar)",
            vec3(0.08f, 0.40f, 0.05f), 1.85f, 36.0f, 7.0f,
            [](Shader& s) {
                Boatman::draw(s, mat4::identity(), 0.0f);
            }
        },
        {
            "07_charpai_bed",
            "Traditional Woven Bed (Charpai with Turned Legs & Jute Webbing)",
            vec3(0.0f, 0.32f, 0.0f), 2.90f, 36.0f, 22.0f,
            [](Shader& s) {
                Charpai::draw(s, mat4::identity());
            }
        },
        {
            "08_palm_fan_haat_pakha",
            "Traditional Handmade Fan (Haat Pakha with Nakshi Embroidered Trim)",
            vec3(0.0f, 0.05f, 0.0f), 0.94f, 0.0f, 7.0f,
            [](Shader& s) {
                Charpai::drawFan(s, mat4::identity(), 0.0f);
            }
        },
        {
            "09_seated_elder",
            "Seated Elder on Charpai Holding Haat Pakha",
            vec3(0.18f, 0.52f, 0.0f), 2.35f, 138.0f, 12.0f,
            [](Shader& s) {
                Charpai::draw(s, mat4::identity());
                PersonParams elder;
                elder.skinColor   = vec3(0.52f, 0.35f, 0.22f);
                elder.shirtColor  = vec3(0.92f, 0.90f, 0.85f);
                elder.pantsColor  = vec3(0.55f, 0.14f, 0.08f);
                elder.seated      = true;
                elder.isElder     = true;
                elder.hasGamcha   = true;
                elder.gamchaColor = vec3(0.80f, 0.20f, 0.14f);
                elder.hasFan      = true;
                elder.fanSway     = 0.0f;
                mat4 elderM = mat4::identity();
                elderM = translate(elderM, vec3(0.20f, 0.50f, 0.0f));
                elderM = rotate(elderM, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
                Person::draw(s, elderM, elder);
            }
        },
        {
            "10_reading_child",
            "Child Sitting Cross-Legged Reading Book",
            vec3(0.0f, 0.22f, 0.12f), 1.20f, 22.0f, 22.0f,
            [](Shader& s) {
                PersonParams child;
                child.skinColor   = vec3(0.52f, 0.36f, 0.22f);
                child.shirtColor  = vec3(0.88f, 0.45f, 0.15f);
                child.pantsColor  = vec3(0.25f, 0.38f, 0.20f);
                child.crossLegged = true;
                mat4 childM = mat4::identity();
                childM = scale(childM, vec3(0.72f, 0.72f, 0.72f));
                Person::draw(s, childM, child);
                mat4 rehalM = childM;
                rehalM = translate(rehalM, vec3(0.0f, 0.00f, 0.35f));
                Person::drawRehal(s, rehalM);
                mat4 bookM = rehalM;
                bookM = translate(bookM, vec3(0.0f, 0.155f, 0.0f));
                bookM = rotate(bookM, radians(20.0f), vec3(1.0f, 0.0f, 0.0f));
                Person::drawBook(s, bookM);
            }
        },
        {
            "11_village_hen",
            "Village Hen (Plump Body, Comb, Beak, Wings & Claws)",
            vec3(0.0f, 0.18f, 0.0f), 0.92f, 42.0f, 15.0f,
            [](Shader& s) {
                Hen::draw(s, mat4::identity());
            }
        },
        {
            "12_river_duck",
            "River Duck (Buoyant Body, Mallard Head & Flat Beak)",
            vec3(0.0f, 0.24f, 0.0f), 1.25f, 38.0f, 15.0f,
            [](Shader& s) {
                Duck::draw(s, mat4::identity());
            }
        },
        {
            "13_coconut_palm_tree",
            "Coconut Palm Tree (Narikel Gach with Coconuts & Cascading Fronds)",
            vec3(0.0f, 3.8f, 0.0f), 11.5f, 30.0f, 10.0f,
            [](Shader& s) {
                Tree::draw(s, mat4::identity(), TREE_PALM);
            }
        },
        {
            "14_banana_tree",
            "Banana Tree (Kola Gach with Paddle Leaves, Fruit & Flower Heart)",
            vec3(0.0f, 1.8f, 0.0f), 6.8f, 32.0f, 10.0f,
            [](Shader& s) {
                Tree::draw(s, mat4::identity(), TREE_BANANA);
            }
        },
        {
            "15_mango_tree",
            "Branching Mango/Banyan Tree (Aam Gach with Gnarled Trunk & Canopy)",
            vec3(0.0f, 2.7f, 1.0f), 8.5f, 25.0f, 12.0f,
            [](Shader& s) {
                Tree::draw(s, mat4::identity(), TREE_GENERAL);
            }
        },
        {
            "16_bamboo_grove",
            "Dense Bamboo Grove (Bansher Jhar with Segmented Culms & Nodes)",
            vec3(0.0f, 2.5f, 0.0f), 8.5f, 30.0f, 16.0f,
            [](Shader& s) {
                Tree::draw(s, mat4::identity(), TREE_BAMBOO);
            }
        },
        {
            "17_rice_plant_dhan",
            "Rural Rice Plant (Dhan Gachh with Drooping Golden Grain Panicles)",
            vec3(0.0f, 0.75f, 0.0f), 2.5f, 35.0f, 20.0f,
            [](Shader& s) {
                Terrain::drawRiceClump(s, mat4::identity(), vec3(0.0f), 2.0f, 1.0f);
            }
        },
        {
            "18_water_lily_shapla",
            "National Water Lily (Shapla with Floating Pad & Petals)",
            vec3(0.0f, 0.04f, 0.0f), 1.6f, 30.0f, 38.0f,
            [](Shader& s) {
                River::drawShapla(s, mat4::identity(), vec3(0.0f), 2.2f);
            }
        },
        {
            "19_kashbon_reeds",
            "Riverbank Catkin Reeds (Kashbon with Fluffy White Plumes)",
            vec3(0.0f, 0.95f, 0.0f), 3.2f, 30.0f, 18.0f,
            [](Shader& s) {
                River::drawKashbonCluster(s, mat4::identity(), vec3(0.0f), 7, 1.2f);
            }
        },
        {
            "20_bamboo_fence",
            "Rural Woven Bamboo Fence (Bansh-er Bera with Crossed Pickets)",
            vec3(0.0f, 0.55f, 0.0f), 3.4f, 25.0f, 16.0f,
            [](Shader& s) {
                Terrain::drawBambooFence(s, mat4::identity(), vec3(-1.5f, 0.0f, 0.0f), 3.0f, 0.0f);
            }
        },
        {
            "21_river_mooring_stake",
            "Bamboo Mooring Stake (Khuti with Coiled Jute Rope)",
            vec3(0.0f, 0.40f, 0.0f), 1.8f, 35.0f, 22.0f,
            [](Shader& s) {
                River::drawMooringStake(s, mat4::identity(), vec3(0.0f));
            }
        },
        {
            "22_full_moon",
            "Luminous Full Moon with Glowing Corona Halo",
            vec3(0.0f, 0.0f, 0.0f), 5.5f, 0.0f, 0.0f,
            [](Shader& s) {
                Moon::draw(s, mat4::identity());
            }
        },
        {
            "23_village_mosque",
            "Historic Bengal Terracotta Village Mosque (Ancient Sultanate Brick Masjid with Curved Cornice, Octagonal Towers & Dome)",
            vec3(0.0f, 1.8f, 0.0f), 12.0f, 32.0f, 18.0f,
            [](Shader& s) {
                Texture::bind(TEX_BRICK, 0);
                s.setInt("uTextureType", (int)TEX_BRICK);
                Mosque::draw(s, mat4::identity());
            }
        },
        {
            "24_house_dochala",
            "Traditional Dochala House (2-Sloped Curved Gable Roof & Mud Plinth)",
            vec3(0.40f, 1.10f, 0.0f), 9.0f, 36.0f, 18.0f,
            [](Shader& s) {
                Texture::bind(TEX_BAMBOO, 0);
                s.setInt("uTextureType", (int)TEX_BAMBOO);
                House::draw(s, mat4::identity(), HOUSE_DOCHALA, false);
            }
        },
        {
            "25_rice_straw_stack",
            "Traditional Rice Straw Stack (Khorer Paloi with Central Bamboo Pole)",
            vec3(0.0f, 1.35f, 0.0f), 5.4f, 32.0f, 14.0f,
            [](Shader& s) {
                House::drawStrawStack(s, mat4::identity(), vec3(0.0f), 1.0f);
            }
        },
        {
            "26_thatched_cow_shed",
            "Traditional Thatched Cow Shed (Gowal Ghor with Feeding Trough)",
            vec3(0.05f, 0.80f, 0.0f), 4.7f, 28.0f, 16.0f,
            [](Shader& s) {
                House::drawCowShed(s, mat4::identity());
            }
        },
        {
            "27_river_landing_ghat",
            "River Landing Ghat (Wooden Platform, Piles & Stepped Descents)",
            vec3(0.0f, 0.35f, 0.0f), 4.4f, 38.0f, 22.0f,
            [](Shader& s) {
                Terrain::drawGhat(s, mat4::identity(), vec3(0.0f));
            }
        },
        {
            "28_standing_villager",
            "Standing Villager (Traditional Lungi with Kocha Pleat & Kurta)",
            vec3(0.0f, 0.62f, 0.0f), 2.2f, 26.0f, 12.0f,
            [](Shader& s) {
                s.setInt("uUseTexture", 0);
                PersonParams p;
                p.skinColor  = vec3(0.55f, 0.38f, 0.25f);
                p.shirtColor = vec3(0.18f, 0.42f, 0.68f);
                p.pantsColor = vec3(0.52f, 0.14f, 0.10f);
                Person::draw(s, mat4::identity(), p);
            }
        },
        {
            "29_bezier_terracotta_vase",
            "Parametric Cubic Bézier Surface of Revolution (Matir Surahi / Pitcher)",
            vec3(0.0f, 0.45f, 0.0f), 2.2f, 32.0f, 16.0f,
            [](Shader& s) {
                s.setInt("uUseTexture", 0);
                CurvedObject::drawBezierVase(s, mat4::identity(), vec3(0.76f, 0.42f, 0.24f));
            }
        },
        {
            "30_bamboo_footbridge",
            "Traditional Curved Arched Bamboo Footbridge (Bansher Saako / বাঁশের সাঁকো)",
            vec3(0.0f, 0.70f, 0.0f), 8.5f, 40.0f, 18.0f,
            [](Shader& s) {
                Texture::bind(TEX_BAMBOO, 0);
                s.setInt("uTextureType", (int)TEX_BAMBOO);
                CurvedObject::drawBambooBridge(s, mat4::identity());
            }
        }
    };

    int index = 1;
    int totalCount = (int)items.size();
    for (const auto& item : items) {
        camera.target   = item.target;
        camera.distance = item.distance;
        camera.yaw      = radians(item.yawDeg);
        camera.pitch    = radians(item.pitchDeg);
        camera.updatePosition();

        vec3 bg = (item.name == "22_full_moon") ? vec3(0.04f, 0.06f, 0.14f) : clearColor;
        glClearColor(bg.x, bg.y, bg.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4("view", camera.getViewMatrix());
        shader.setMat4("projection", camera.getProjectionMatrix());
        shader.setVec3("lightDir", lightDir);
        shader.setVec3("lightColor", lightColor);
        shader.setVec3("viewPos", camera.position);
        shader.setFloat("ambientStrength", (item.name == "22_full_moon") ? 1.0f : 0.95f);
        shader.setFloat("specularStrength", 0.15f);
        shader.setFloat("shininess", 32.0f);
        shader.setFloat("emissive", 0.0f);
        shader.setInt("noLighting", (item.name == "22_full_moon") ? 0 : 1);
        shader.setFloat("pointLightIntensity", 0.0f);
        shader.setFloat("pointLight2Intensity", 0.0f);
        shader.setFloat("fogDensity", 0.0f);
        shader.setInt("uUseTexture", g_textureMode);
        Texture::bind(TEX_WOOD, 0);
        shader.setInt("uTextureType", (int)TEX_WOOD);

        item.drawFunc(shader);

        glFlush();
        std::string filename = "object_images/" + item.name + ".bmp";
        saveBMP(filename, fbW, fbH);

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::cout << "  [" << (index < 10 ? "0" : "") << index << "/" << totalCount << "] Captured: "
                  << item.title << " -> " << filename << "\n";
        index++;
    }

    std::cout << "========================================================\n";
    std::cout << "  ALL " << totalCount << " OBJECT IMAGES SUCCESSFULLY SAVED TO 'object_images/'!\n";
    std::cout << "========================================================\n\n";
}

// ─── Main ────────────────────────────────────────────────────────────
int main(int argc, char* argv[])
{
    glfwSetErrorCallback(glfwErrorCallback);

    // ── GLFW Init ───────────────────────────────────────────────
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Try 4x MSAA

    GLFWwindow* window = glfwCreateWindow(
        winWidth, winHeight,
        "Village Gathering by the River - Bangladeshi Rural Scene in 3D (OpenGL 3.3)",
        nullptr, nullptr
    );
    if (!window) {
        std::cerr << "Retrying window creation without MSAA..." << std::endl;
        glfwWindowHint(GLFW_SAMPLES, 0);
        window = glfwCreateWindow(
            winWidth, winHeight,
            "Village Gathering by the River - Bangladeshi Rural Scene in 3D (OpenGL 3.3)",
            nullptr, nullptr
        );
    }
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwShowWindow(window);
    glfwFocusWindow(window);
    glfwSwapInterval(1); // Enable VSync

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);

    // ── GLAD Init ───────────────────────────────────────────────
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Initial framebuffer size and viewport setup (after GLAD loaded)
    int fbW = 0, fbH = 0;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    if (fbW > 0 && fbH > 0) {
        glViewport(0, 0, fbW, fbH);
        camera.aspectRatio = (float)fbW / (float)fbH;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "========================================================\n";
    std::cout << "  BANGLADESHI RURAL VILLAGE 3D SCENE (OpenGL 3.3)\n";
    std::cout << "  Village Gathering by the River (Gramin Poribesh)\n";
    std::cout << "========================================================\n";
    std::cout << "CONTROLS & SHORTCUTS:\n";
    std::cout << "  Left-Drag Mouse : Orbit Camera Around Object\n";
    std::cout << "  Scroll Wheel    : Zoom In / Out\n";
    std::cout << "  W / S / A / D   : Move Camera Freely\n";
    std::cout << "  SPACE           : Toggle Cinematic Fly-Through Village Tour\n";
    std::cout << "  Key 'J'         : Toggle Directional Light (Moonlight/Sunlight ON/OFF)\n";
    std::cout << "  Key 'H'         : Cycle 6 Point Lights (Normal / Bright / Amber / OFF)\n";
    std::cout << "  Key 'P'         : Pump Tubewell (Interactive Water Flow into Kolshi)\n";
    std::cout << "  Key 'B'         : Toggle Summer Breeze (Tree Foliage Wind Sway)\n";
    std::cout << "  Key '[' / ']'   : Decrease / Increase Animation Speed (0.25x - 3.0x)\n";
    std::cout << "  Key 'K'         : Pause / Resume Continuous Village Animations\n";
    std::cout << "  Key 'L'         : Toggle Lighting (0: Moonlit Night | 1: Day | 2: Unlit Facets | 3: Flat)\n";
    std::cout << "  Key 'X'         : Toggle Texture Mode (0: Solid | 1: Procedural Detailing | 2: GPU Texture Maps)\n";
    std::cout << "  Key 'V'         : View V - Curved Objects (Bamboo Footbridge & Parametric Bézier Vase)\n";
    std::cout << "  Key '1'         : View 1 - Courtyard Gathering (Charpai, Elders, Children Reading, Hens)\n";
    std::cout << "  Key '2'         : View 2 - River Shore, Landing Ghat, Moored Boat & Rowing Boatman\n";
    std::cout << "  Key '3'         : View 3 - Historic Terracotta Village Mosque (Ancient Sultanate Brick Masjid)\n";
    std::cout << "  Key '4'         : View 4 - North Homestead (Dochala, Cow Shed & Straw Stack)\n";
    std::cout << "  Key '5'         : View 5 - South Homestead, Straw Stack & Terraced Paddy Fields\n";
    std::cout << "  Key '6'         : View 6 - Rural Trees & Riverbank Reeds\n";
    std::cout << "  Key '7'         : View 7 - Village Animals (Flocks of Hens & River Ducks)\n";
    std::cout << "  Key '8'         : View 8 - Broader View (Moonlit Village Overview matching Concept Artwork)\n";
    std::cout << "  Key '9'         : View 9 - Hand-Pump Tubewell & Clay Cooking Kitchen\n";
    std::cout << "  Key '0'         : View 0 - Thatched Cow Shed & Resting Deshi Cow\n";
    std::cout << "  Key 'T'         : Toggle Terrain/Ground Visibility\n";
    std::cout << "  Key 'C'         : Capture All 30 Objects to 'object_images/'\n";
    std::cout << "  ESC             : Exit\n";
    std::cout << "========================================================\n";

    // ── OpenGL State ────────────────────────────────────────────
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // ── Shader ──────────────────────────────────────────────────
    Shader shader;
    shader.init(Shader::phongVertexSrc, Shader::phongFragmentSrc);

    // ── Primitives & GPU Texture / Curved Mesh Generation ───────
    Primitives::init();
    Texture::init();
    CurvedObject::init();

    shader.use();
    shader.setInt("uTexture", 0);

    g_window = window;
    g_shader = &shader;

    // Check if capture mode was requested via command line flag
    if (argc > 1 && (std::string(argv[1]) == "--capture" || std::string(argv[1]) == "-c")) {
        captureAllObjects(window, shader);
        CurvedObject::cleanup();
        Boat::cleanup();
        Texture::cleanup();
        Primitives::cleanup();
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }

    // ── Camera Default Setup: Broader View of Moonlit Village Scene ──
    camera.target   = vec3(0.5f, 1.2f, -6.5f);
    camera.yaw      = radians(-12.0f);
    camera.pitch    = radians(14.0f);
    camera.distance = 19.5f;
    camera.updatePosition();

    // Lantern position in courtyard (acts as point light source)
    vec3 lanternPos(-3.0f, 0.40f, 0.6f);

    float lastFrameTime = (float)glfwGetTime();

    // ═════════════════════════════════════════════════════════════
    // RENDER LOOP
    // ═════════════════════════════════════════════════════════════
    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();
        float dt = time - lastFrameTime;
        lastFrameTime = time;
        if (dt > 0.1f) dt = 0.1f; // clamp delta time for stability

        // ── WASD Keyboard Camera Movement ───────────────────────
        float camFwd = 0.0f;
        float camRgt = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camFwd += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camFwd -= 1.0f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camRgt += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camRgt -= 1.0f;

        if (camFwd != 0.0f || camRgt != 0.0f) {
            g_tourActive = false; // User movement takes over manual control
            camera.processKeyboardMovement(camFwd, camRgt, dt);
        }

        // ── Automated Cinematic Fly-Through Tour ─────────────────
        if (g_tourActive) {
            g_tourTime += dt;
            if (g_tourTime >= g_tourDuration) {
                g_tourTime = fmodf(g_tourTime, g_tourDuration);
            }
            int idx = 0;
            for (int k = 0; k < g_numTourFrames - 1; ++k) {
                if (g_tourTime >= g_tourFrames[k].time && g_tourTime <= g_tourFrames[k+1].time) {
                    idx = k;
                    break;
                }
            }
            float segDuration = g_tourFrames[idx+1].time - g_tourFrames[idx].time;
            float t = (segDuration > 0.001f) ? ((g_tourTime - g_tourFrames[idx].time) / segDuration) : 0.0f;
            float s = t * t * (3.0f - 2.0f * t); // Smooth cubic interpolation

            camera.target   = lerp(g_tourFrames[idx].target, g_tourFrames[idx+1].target, s);
            float curYaw    = lerp(g_tourFrames[idx].yawDeg, g_tourFrames[idx+1].yawDeg, s);
            float curPitch  = lerp(g_tourFrames[idx].pitchDeg, g_tourFrames[idx+1].pitchDeg, s);
            camera.yaw      = radians(curYaw);
            camera.pitch    = radians(curPitch);
            camera.distance = lerp(g_tourFrames[idx].distance, g_tourFrames[idx+1].distance, s);
            camera.updatePosition();
        }

        // ── Dynamic Motion Time & Interactive Physics ────────────
        static float g_accumAnimTime = 0.0f;
        g_accumAnimTime += dt * g_animSpeed;
        float animTime = g_accumAnimTime;

        // Interactive Tubewell Hand-Pumping Physics
        if (g_pumpTimer > 0.0f) {
            g_pumpTimer -= dt;
        }
        bool  isCurrentlyPumping = (g_pumpTimer > 0.0f);
        float pumpHandleAngle   = isCurrentlyPumping ? (sinf(animTime * 12.0f) * radians(20.0f)) : 0.0f;

        // Cruising Dingi Nouka navigation kinematics (shared by Point Light 6 & Boat 2)
        float riverLoopLength = 42.0f;
        float boatNavSpeed    = 1.35f; // Steady rural rowing speed
        float boatZ = -22.0f + fmodf(animTime * boatNavSpeed, riverLoopLength);
        float riverCenterlineX = 1.8f * sinf(boatZ * 0.08f + 0.4f) + 0.6f * cosf(boatZ * 0.04f);
        float boatX = 8.0f + riverCenterlineX + 0.5f;
        float dRiverX = 1.8f * 0.08f * cosf(boatZ * 0.08f + 0.4f) - 0.6f * 0.04f * sinf(boatZ * 0.04f);
        float boatCourseYaw = atan2f(dRiverX, 1.0f);
        float rowCyclePhase = animTime * 2.2f;
        float rowingSurge   = sinf(rowCyclePhase) * 0.045f;
        float boatBob2      = (animTime > 0.0f) ? (sinf(rowCyclePhase * 2.0f) * 0.022f) : 0.0f;
        float boatRoll2     = (animTime > 0.0f) ? (sinf(rowCyclePhase) * radians(1.8f)) : 0.0f;
        float boatPitch2    = (animTime > 0.0f) ? (-cosf(rowCyclePhase) * radians(1.5f)) : 0.0f;
        float oarRowAnim    = (animTime > 0.0f) ? (sinf(rowCyclePhase) * radians(16.0f)) : 0.0f;

        // ── Lighting Mode Setup ─────────────────────────────────
        vec3 clearColor;
        vec3 lightDir;
        vec3 lightColor;
        float ambientStrength;
        float specularStrength = 0.0f;
        float shininess        = 32.0f;
        vec3  pointColor       (1.0f, 0.78f, 0.32f); // warm golden kerosene glow
        float pointIntensity   = 0.0f;
        vec3  fogCol;
        float fogDens          = 0.0f;
        int   noLightVal       = 1;

        if (lightingMode == 0) {
            // Default Mode: Moonlit Bengali Night (Atmospheric rural night with moon, stars, fireflies, lanterns & stove embers)
            clearColor       = vec3(0.04f, 0.06f, 0.13f); // deep midnight sky
            fogCol           = vec3(0.04f, 0.06f, 0.13f);
            lightDir         = normalize(vec3(-0.35f, -0.48f, 0.80f)); // luminous moonlight from full moon at (18, 10, -28)
            lightColor       = vec3(0.68f, 0.78f, 0.98f);             // cool luminous silvery moonlight
            ambientStrength  = 0.38f;                                  // soft ambient night with clear visibility
            specularStrength = 0.55f;
            pointIntensity   = 1.0f;                                   // lanterns & fire fully glowing!
            fogDens          = 0.005f;                                 // gentle distance night mist
            noLightVal       = 0;                                      // full Blinn-Phong lighting
        }
        else if (lightingMode == 1) {
            // Full Day Scene (Bright sunlight with Blinn-Phong shading)
            clearColor       = vec3(0.58f, 0.76f, 0.90f); // crisp daylight blue sky
            fogCol           = clearColor;
            lightDir         = normalize(vec3(0.35f, -0.85f, -0.40f)); // bright daylight sun
            lightColor       = vec3(1.0f, 0.98f, 0.92f);              // warm sunlight
            ambientStrength  = 0.50f;                                  // bright daytime ambient
            specularStrength = 0.20f;
            pointIntensity   = 0.0f;                                  // daytime: lantern illumination off
            fogDens          = 0.0f;                                   // clear daytime visibility
            noLightVal       = 0;                                      // full Blinn-Phong lighting
        }
        else if (lightingMode == 2) {
            // Crisp Unlit 3D Facets (Daylight, 0 darkness, 0 shadows, 0 point lights - Lab Milestone Inspection)
            clearColor      = vec3(0.82f, 0.88f, 0.94f); // clean daylight sky
            fogCol          = clearColor;
            lightDir        = normalize(vec3(0.35f, -0.90f, -0.40f));
            lightColor      = vec3(1.0f, 1.0f, 1.0f);
            ambientStrength = 1.0f;
            noLightVal      = 1; // gentle facet shading for crisp 3D recognition
            pointIntensity  = 0.0f;
            fogDens         = 0.0f;
        }
        else {
            // Pure Flat Object Color (completely unshaded)
            clearColor      = vec3(0.88f, 0.90f, 0.92f); // neutral light studio backdrop
            fogCol          = clearColor;
            lightDir        = normalize(vec3(0.0f, -1.0f, 0.0f));
            lightColor      = vec3(1.0f, 1.0f, 1.0f);
            ambientStrength = 1.0f;
            noLightVal      = 2; // pure flat unshaded objectColor
            pointIntensity  = 0.0f;
            fogDens         = 0.0f;
        }

        // ── Clear Framebuffer ───────────────────────────────────
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ── Uniforms (Scene-wide) ───────────────────────────────
        shader.use();
        shader.setMat4("view",       camera.getViewMatrix());
        shader.setMat4("projection", camera.getProjectionMatrix());
        shader.setVec3("lightDir",   lightDir);
        shader.setVec3("lightColor", lightColor);
        shader.setVec3("viewPos",    camera.position);
        shader.setFloat("ambientStrength",  ambientStrength);
        shader.setFloat("specularStrength", specularStrength);
        shader.setFloat("shininess",        shininess);
        shader.setFloat("emissive",         0.0f);
        shader.setInt("noLighting",         noLightVal);
        shader.setInt("dirLightEnabled",    g_dirLightEnabled ? 1 : 0);
        shader.setInt("pointLightsEnabled", (g_lanternMode != 3) ? 1 : 0);

        // Interactive Hariken Lantern Flame Mode
        float lanternMultiplier = 1.0f;
        if (g_lanternMode == 1) lanternMultiplier = 1.85f;      // Extra bright blazing flame
        else if (g_lanternMode == 2) lanternMultiplier = 0.45f; // Soft amber glow
        else if (g_lanternMode == 3) lanternMultiplier = 0.0f;  // Extinguished

        // Point Light 1: Courtyard Hurricane Lantern (Hariken on stool)
        float flameFlicker = (pointIntensity > 0.0f && lanternMultiplier > 0.0f) ? (1.0f + 0.06f * sinf(animTime * 11.3f) * cosf(animTime * 17.7f)) : 1.0f;
        shader.setVec3("pointLightPos",        vec3(lanternPos.x, 0.425f, lanternPos.z));
        shader.setVec3("pointLightColor",      vec3(1.0f, 0.72f, 0.28f)); // warm golden flame
        shader.setFloat("pointLightIntensity", pointIntensity * lanternMultiplier * flameFlicker * 1.25f);

        // Point Light 2: Moored Boat Hurricane Lantern (Hariken hanging under Chhoi)
        float boatBobLight = sinf(animTime * 1.6f) * 0.018f;
        float boatFlameFlicker = (pointIntensity > 0.0f && lanternMultiplier > 0.0f) ? (1.0f + 0.07f * sinf(animTime * 9.7f) * cosf(animTime * 14.3f)) : 1.0f;
        vec3 boatLanternWorldPos = vec3(7.0f, 0.11f + boatBobLight + 0.38f, 1.2f + 0.72f);
        shader.setVec3("pointLight2Pos",        boatLanternWorldPos);
        shader.setVec3("pointLight2Color",      vec3(1.0f, 0.68f, 0.22f)); // warm amber boat lantern
        shader.setFloat("pointLight2Intensity", pointIntensity * lanternMultiplier * boatFlameFlicker * 1.10f);

        // Point Light 3: Mosque Entrance Portal Lantern (Mehrab Arched Lamp)
        float mosqueLanternFlicker = (pointIntensity > 0.0f && lanternMultiplier > 0.0f) ? (1.0f + 0.03f * sinf(animTime * 7.1f) * cosf(animTime * 12.3f)) : 1.0f;
        vec3 mosqueLanternWorldPos = vec3(-3.8f, 2.28f, -14.42f);
        shader.setVec3("pointLight3Pos",        mosqueLanternWorldPos);
        shader.setVec3("pointLight3Color",      vec3(1.0f, 0.78f, 0.35f)); // warm brass lantern glow
        shader.setFloat("pointLight3Intensity", pointIntensity * lanternMultiplier * mosqueLanternFlicker * 1.15f);

        // Point Light 4: Kitchen Clay Cooking Stove (Matir Chula) Embers / Fire
        float stoveFlicker = (pointIntensity > 0.0f) ? (1.0f + 0.12f * sinf(animTime * 15.3f) * cosf(animTime * 23.7f) + 0.05f * sinf(animTime * 31.1f)) : 1.0f;
        vec3 stoveWorldPos = vec3(-6.2f, 0.35f, -6.6f);
        shader.setVec3("pointLight4Pos",        stoveWorldPos);
        shader.setVec3("pointLight4Color",      vec3(1.0f, 0.42f, 0.10f)); // glowing wood fire embers
        shader.setFloat("pointLight4Intensity", pointIntensity * stoveFlicker * 0.95f);

        // Point Light 5: River Landing Ghat Mooring Post Lantern
        float ghatFlameFlicker = (pointIntensity > 0.0f && lanternMultiplier > 0.0f) ? (1.0f + 0.05f * sinf(animTime * 8.3f) * cosf(animTime * 13.7f)) : 1.0f;
        vec3 ghatLanternWorldPos = vec3(4.7f, 0.70f, 0.0f);
        shader.setVec3("pointLight5Pos",        ghatLanternWorldPos);
        shader.setVec3("pointLight5Color",      vec3(1.0f, 0.74f, 0.30f)); // warm golden dock lantern
        shader.setFloat("pointLight5Intensity", pointIntensity * lanternMultiplier * ghatFlameFlicker * 1.15f);

        // Point Light 6: Cruising Dingi Nouka Dynamic Lantern (navigates along river in real time!)
        float boat2FlameFlicker = (pointIntensity > 0.0f && lanternMultiplier > 0.0f) ? (1.0f + 0.06f * sinf(animTime * 10.1f) * cosf(animTime * 16.3f)) : 1.0f;
        vec3 boat2LanternWorldPos = vec3(boatX, 0.10f + boatBob2 + 0.40f, boatZ + rowingSurge + 1.08f);
        shader.setVec3("pointLight6Pos",        boat2LanternWorldPos);
        shader.setVec3("pointLight6Color",      vec3(1.0f, 0.68f, 0.22f)); // warm amber moving lantern
        shader.setFloat("pointLight6Intensity", pointIntensity * lanternMultiplier * boat2FlameFlicker * 1.15f);

        // Atmospheric Distance Fog
        shader.setVec3("fogColor",  fogCol);
        shader.setFloat("fogDensity", fogDens);

        // Procedural & GPU Texture Mapping
        shader.setInt("uUseTexture", g_textureMode);
        Texture::bind(TEX_WOOD, 0);
        shader.setInt("uTextureType", (int)TEX_WOOD);

        // ─── 1. NIGHT SKY & CELESTIAL BODIES ─────────────────────
        bool isNight = (lightingMode == 0);
        if (isNight) {
            // Full Moon luminous in the upper-right night sky over the river
            mat4 moonM = mat4::identity();
            moonM = translate(moonM, vec3(18.0f, 10.0f, -28.0f));
            Moon::draw(shader, moonM);

            // Scattered stars across the sky dome with twinkling
            Stars::draw(shader, animTime);

            // Blinking fireflies (Jonaki Poka) over village clearing, river, paddy & trees
            Fireflies::draw(shader, animTime, true);
        }

        // ─── 2. TERRAIN, COURTYARDS, ROADS, RIVER & LANDING GHAT ─
        if (showTerrain) {
            mat4 terrainM = mat4::identity();
            Terrain::draw(shader, terrainM);

            mat4 riverM = mat4::identity();
            riverM = translate(riverM, vec3(8.0f, 0.0f, 0.0f));
            River::draw(shader, riverM, animTime);
        }

        // ─── 3. TRADITIONAL VILLAGE MOSQUE (GRAMIN MASJID) ───────
        mat4 mosqueM = mat4::identity();
        mosqueM = translate(mosqueM, vec3(-3.8f, 0.0f, -17.5f));
        Texture::bind(TEX_BRICK, 0);
        shader.setInt("uTextureType", (int)TEX_BRICK);
        Mosque::draw(shader, mosqueM);

        // ─── 4. VILLAGE HOUSES & HOMESTEADS (FULL VILLAGE SCENE) ─
        Texture::bind(TEX_BAMBOO, 0);
        shader.setInt("uTextureType", (int)TEX_BAMBOO);
        // House 1: Main Central Homestead (Chouchala 4-sloped hip roof, verandah & outdoor clay stove)
        mat4 house1 = mat4::identity();
        house1 = translate(house1, vec3(-8.5f, 0.0f, -6.0f));
        house1 = rotate(house1, radians(6.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house1, HOUSE_CHOUCHALA, true);

        // House 2: North Bari Family House (Dochala 2-sloped curved pitched roof & bamboo verandah)
        mat4 house2 = mat4::identity();
        house2 = translate(house2, vec3(-14.0f, 0.0f, -7.5f));
        house2 = rotate(house2, radians(-18.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house2, HOUSE_DOCHALA, false);

        // House 3: South Homestead (Chouchala House facing the path & terraced paddy fields)
        mat4 house3 = mat4::identity();
        house3 = translate(house3, vec3(-7.5f, 0.0f, 15.0f));
        house3 = rotate(house3, radians(170.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house3, HOUSE_CHOUCHALA, false);

        // House 4: Riverside Fisherman / Boatman Hut (Dochala style near northern riverbank path)
        mat4 house4 = mat4::identity();
        house4 = translate(house4, vec3(0.5f, 0.0f, -11.0f));
        house4 = rotate(house4, radians(82.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house4, HOUSE_DOCHALA, false);

        // House 5: East-Bari Homestead / Annex House (Dochala style facing central courtyard)
        mat4 house5 = mat4::identity();
        house5 = translate(house5, vec3(-14.5f, 0.0f, 0.5f));
        house5 = rotate(house5, radians(42.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house5, HOUSE_DOCHALA, false);

        // Auxiliary 1: Traditional Thatched Cow Shed (Gowal Ghor with wooden feeding trough and hay)
        mat4 cowShedM = mat4::identity();
        cowShedM = translate(cowShedM, vec3(-17.5f, 0.0f, -3.5f));
        cowShedM = rotate(cowShedM, radians(12.0f), vec3(0.0f, 1.0f, 0.0f));
        House::drawCowShed(shader, cowShedM);

        // Standing Deshi Cow resting in the North Homestead farmyard (near Gowal Ghor & Straw Stack)
        mat4 cowStandingM = mat4::identity();
        cowStandingM = translate(cowStandingM, vec3(-15.5f, 0.0f, -5.2f));
        cowStandingM = rotate(cowStandingM, radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
        House::drawCow(shader, cowStandingM, false);

        // Auxiliary 2: Traditional Rice Straw Stacks (Khorer Paloi / খড়ের পালা with center bamboo pole)
        House::drawStrawStack(shader, mat4::identity(), vec3(-16.2f, 0.0f, -1.2f), 1.15f);
        House::drawStrawStack(shader, mat4::identity(), vec3(-9.2f, 0.0f, 18.0f), 1.05f);
        House::drawStrawStack(shader, mat4::identity(), vec3(-6.8f, 0.0f, 4.8f), 1.05f); // Foreground stack framing view along fence

        // Auxiliary 3: Outdoor Thatched Kitchen Hut (Ranna Ghor)
        mat4 kitchenM = mat4::identity();
        kitchenM = translate(kitchenM, vec3(-5.2f, 0.0f, -9.5f));
        kitchenM = rotate(kitchenM, radians(-8.0f), vec3(0.0f, 1.0f, 0.0f));
        kitchenM = scale(kitchenM, vec3(0.72f, 0.82f, 0.72f));
        House::draw(shader, kitchenM, HOUSE_DOCHALA, false);

        // Auxiliary 4: Traditional Cast-Iron Tubewell (Chapa Kol / টিউবওয়েল) with concrete pad, spout & pitcher
        mat4 tubewellM = mat4::identity();
        tubewellM = translate(tubewellM, vec3(-5.2f, 0.0f, -2.4f));
        tubewellM = rotate(tubewellM, radians(28.0f), vec3(0.0f, 1.0f, 0.0f));
        House::drawTubewell(shader, tubewellM, pumpHandleAngle, isCurrentlyPumping);

        // Auxiliary 5: Terracotta Clay Water Pitchers (Matir Kolshi / মাটির কলসি) placed across functional zones of homestead & ghat
        House::drawKolshi(shader, mat4::identity(), vec3(-4.6f, 0.0f, -2.1f), 0.95f);  // Tubewell apron (for collecting fresh drinking water)
        House::drawKolshi(shader, mat4::identity(), vec3(-11.8f, 0.0f, -6.2f), 0.90f); // Dochala House 2 entrance yard
        House::drawKolshi(shader, mat4::identity(), vec3(-4.8f, 0.0f, -8.6f), 0.88f);  // Kitchen Hut (Ranna Ghor) for cooking water
        House::drawKolshi(shader, mat4::identity(), vec3(4.8f, 0.12f, 1.8f), 0.92f);   // River Landing Ghat (for bathing/washing)
        House::drawKolshi(shader, mat4::identity(), vec3(-6.8f, 0.0f, 14.2f), 0.85f);  // South Homestead House 3 entrance

        // Auxiliary 6: Traditional Village Chicken Coop (Murgir Khopa / মোরগের খোঁপা) on bamboo stilts
        // Placed in the homestead yard beside House 1 (near ghor, completely clear of the living verandah)
        mat4 coopM = mat4::identity();
        coopM = translate(coopM, vec3(-11.6f, 0.0f, -3.5f));
        coopM = rotate(coopM, radians(24.0f), vec3(0.0f, 1.0f, 0.0f));
        House::drawChickenCoop(shader, coopM, isNight);

        // ─── 4B. PARAMETRIC CUBIC BÉZIER CURVED OBJECTS (TERRACOTTA SURAHI / VASE) ─
        // Mathematical Bézier Surface of Revolution with cubic curve profile & analytic normals
        // Distributed to homestead entrances to enrich village architecture without cluttering the courtyard
        mat4 vaseM1 = mat4::identity();
        vaseM1 = translate(vaseM1, vec3(-12.5f, 0.08f, 1.8f)); // Front courtyard entrance of East-Bari House 5
        vaseM1 = scale(vaseM1, vec3(0.72f, 0.72f, 0.72f));
        Texture::bind(TEX_BRICK, 0);
        shader.setInt("uTextureType", (int)TEX_BRICK);
        CurvedObject::drawBezierVase(shader, vaseM1, vec3(0.76f, 0.42f, 0.24f));

        mat4 vaseM2 = mat4::identity();
        vaseM2 = translate(vaseM2, vec3(-6.2f, 0.08f, 13.8f)); // Entrance yard of South Homestead House 3
        vaseM2 = scale(vaseM2, vec3(0.62f, 0.62f, 0.62f));
        CurvedObject::drawBezierVase(shader, vaseM2, vec3(0.72f, 0.38f, 0.20f));

        // ─── 5. VEGETATION (RICH RURAL BENGALI TREE CANOPY) ──────
        Texture::bind(TEX_WOOD, 0);
        shader.setInt("uTextureType", (int)TEX_WOOD);
        // Summer breeze gentle wind sway for foliage and fronds
        float palmSway1  = g_windBreeze ? (sinf(animTime * 1.3f) * radians(2.2f)) : 0.0f;
        float palmSway2  = g_windBreeze ? (sinf(animTime * 1.5f + 1.1f) * radians(2.5f)) : 0.0f;
        float bananaSway = g_windBreeze ? (sinf(animTime * 2.1f + 0.4f) * radians(1.8f)) : 0.0f;
        float treeSway   = g_windBreeze ? (sinf(animTime * 1.1f + 0.7f) * radians(1.4f)) : 0.0f;
        float bambooSway = g_windBreeze ? (sinf(animTime * 1.9f + 1.8f) * radians(2.2f)) : 0.0f;

        // Coconut Palms (Narikel Gach with curved segmented trunks, coconuts & cascading fronds)
        mat4 palm1 = mat4::identity();
        palm1 = translate(palm1, vec3(4.2f, 0.0f, -8.5f));
        palm1 = rotate(palm1, radians(-12.0f), vec3(0.0f, 1.0f, 0.0f));
        palm1 = rotate(palm1, palmSway1, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, palm1, TREE_PALM);

        mat4 palm2 = mat4::identity();
        palm2 = translate(palm2, vec3(3.8f, 0.0f, 6.5f));
        palm2 = rotate(palm2, radians(25.0f), vec3(0.0f, 1.0f, 0.0f));
        palm2 = rotate(palm2, palmSway2, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, palm2, TREE_PALM);

        mat4 palm3 = mat4::identity();
        palm3 = translate(palm3, vec3(-8.8f, 0.0f, -13.0f));
        palm3 = rotate(palm3, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
        palm3 = rotate(palm3, palmSway1, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, palm3, TREE_PALM);

        mat4 palm4 = mat4::identity();
        palm4 = translate(palm4, vec3(-17.5f, 0.0f, 8.0f));
        palm4 = rotate(palm4, radians(-30.0f), vec3(0.0f, 1.0f, 0.0f));
        palm4 = rotate(palm4, palmSway2, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, palm4, TREE_PALM);

        mat4 palm5 = mat4::identity();
        palm5 = translate(palm5, vec3(-11.5f, 0.0f, 13.5f));
        palm5 = rotate(palm5, radians(15.0f), vec3(0.0f, 1.0f, 0.0f));
        palm5 = rotate(palm5, palmSway1, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, palm5, TREE_PALM);

        mat4 palm6 = mat4::identity();
        palm6 = translate(palm6, vec3(-3.2f, 0.0f, -16.5f));
        palm6 = rotate(palm6, radians(-40.0f), vec3(0.0f, 1.0f, 0.0f));
        palm6 = rotate(palm6, palmSway2, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, palm6, TREE_PALM);

        // Banana Trees (Kola Gach with paddle leaves, fruit bunch & purple heart)
        mat4 banana1 = mat4::identity();
        banana1 = translate(banana1, vec3(-13.5f, 0.0f, -4.5f));
        banana1 = rotate(banana1, bananaSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, banana1, TREE_BANANA);

        mat4 banana2 = mat4::identity();
        banana2 = translate(banana2, vec3(-15.0f, 0.0f, -3.2f));
        banana2 = rotate(banana2, radians(55.0f), vec3(0.0f, 1.0f, 0.0f));
        banana2 = rotate(banana2, -bananaSway, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, banana2, TREE_BANANA);

        mat4 banana3 = mat4::identity();
        banana3 = translate(banana3, vec3(-6.2f, 0.0f, -11.5f));
        banana3 = rotate(banana3, radians(-25.0f), vec3(0.0f, 1.0f, 0.0f));
        banana3 = rotate(banana3, bananaSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, banana3, TREE_BANANA);

        mat4 banana4 = mat4::identity();
        banana4 = translate(banana4, vec3(-10.2f, 0.0f, 16.5f));
        banana4 = rotate(banana4, radians(80.0f), vec3(0.0f, 1.0f, 0.0f));
        banana4 = rotate(banana4, -bananaSway, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, banana4, TREE_BANANA);

        mat4 banana5 = mat4::identity();
        banana5 = translate(banana5, vec3(1.5f, 0.0f, -13.5f));
        banana5 = rotate(banana5, radians(-60.0f), vec3(0.0f, 1.0f, 0.0f));
        banana5 = rotate(banana5, bananaSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, banana5, TREE_BANANA);

        // Foreground Banana Tree (framing the courtyard & river view)
        mat4 bananaFg = mat4::identity();
        bananaFg = translate(bananaFg, vec3(0.5f, 0.0f, 3.2f));
        bananaFg = rotate(bananaFg, radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
        bananaFg = rotate(bananaFg, bananaSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, bananaFg, TREE_BANANA);

        // Branching Banyan / Mango Trees (Bot / Aam Gach with spreading leafy canopy)
        mat4 mangoTree1 = mat4::identity();
        mangoTree1 = translate(mangoTree1, vec3(-16.5f, 0.0f, -11.5f));
        mangoTree1 = rotate(mangoTree1, treeSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, mangoTree1, TREE_GENERAL);

        mat4 mangoTree2 = mat4::identity();
        mangoTree2 = translate(mangoTree2, vec3(-18.5f, 0.0f, 13.0f));
        mangoTree2 = rotate(mangoTree2, radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
        mangoTree2 = rotate(mangoTree2, -treeSway, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, mangoTree2, TREE_GENERAL);

        mat4 mangoTree3 = mat4::identity();
        mangoTree3 = translate(mangoTree3, vec3(-1.5f, 0.0f, 10.5f));
        mangoTree3 = rotate(mangoTree3, radians(-45.0f), vec3(0.0f, 1.0f, 0.0f));
        mangoTree3 = rotate(mangoTree3, treeSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, mangoTree3, TREE_GENERAL);

        // Bamboo Groves (Bansher Jhar with segmented culms & annular joints)
        mat4 bambooGrove1 = mat4::identity();
        bambooGrove1 = translate(bambooGrove1, vec3(-19.5f, 0.0f, -14.5f));
        bambooGrove1 = rotate(bambooGrove1, radians(30.0f), vec3(0.0f, 1.0f, 0.0f));
        bambooGrove1 = rotate(bambooGrove1, bambooSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, bambooGrove1, TREE_BAMBOO);

        mat4 bambooGrove2 = mat4::identity();
        bambooGrove2 = translate(bambooGrove2, vec3(-17.0f, 0.0f, -17.5f));
        bambooGrove2 = rotate(bambooGrove2, radians(-15.0f), vec3(0.0f, 1.0f, 0.0f));
        bambooGrove2 = rotate(bambooGrove2, -bambooSway, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, bambooGrove2, TREE_BAMBOO);

        mat4 bambooGrove3 = mat4::identity();
        bambooGrove3 = translate(bambooGrove3, vec3(-19.0f, 0.0f, 21.0f));
        bambooGrove3 = rotate(bambooGrove3, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
        bambooGrove3 = rotate(bambooGrove3, bambooSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, bambooGrove3, TREE_BAMBOO);

        // ─── 6. VILLAGE GATHERING (UTHAN) & CHARACTERS ───────────
        // Traditional woven Charpai (bed)
        Texture::bind(TEX_WOOD, 0);
        shader.setInt("uTextureType", (int)TEX_WOOD);
        mat4 charpaiM = mat4::identity();
        charpaiM = translate(charpaiM, vec3(-4.2f, 0.0f, 0.6f));
        charpaiM = rotate(charpaiM, radians(-15.0f), vec3(0.0f, 1.0f, 0.0f));
        Charpai::draw(shader, charpaiM);

        // Person 1: Seated Elder on Charpai edge fanning himself with handmade fan (Haat Pakha)
        Texture::bind(TEX_FABRIC, 0);
        shader.setInt("uTextureType", (int)TEX_FABRIC);
        PersonParams elder;
        elder.skinColor   = vec3(0.52f, 0.35f, 0.22f);
        elder.shirtColor  = vec3(0.92f, 0.90f, 0.85f); // white cotton kurta
        elder.pantsColor  = vec3(0.55f, 0.14f, 0.08f); // maroon lungi
        elder.seated      = true;
        elder.isElder     = true;                      // white hair and beard
        elder.hasGamcha   = true;                      // red gamcha over shoulder
        elder.gamchaColor = vec3(0.80f, 0.20f, 0.14f);
        elder.hasFan      = true;                      // holds traditional handmade fan (Haat Pakha)
        elder.fanSway     = (animTime > 0.0f) ? (sinf(animTime * 2.6f) * radians(10.0f)) : 0.0f;

        mat4 elderM = charpaiM;
        elderM = translate(elderM, vec3(0.20f, 0.50f, 0.0f));
        elderM = rotate(elderM, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
        Person::draw(shader, elderM, elder);

        // Low wooden stool with glowing Hurricane Lantern (Hariken)
        mat4 stoolM = mat4::identity();
        stoolM = translate(stoolM, vec3(lanternPos.x, 0.12f, lanternPos.z));
        stoolM = scale(stoolM, vec3(0.38f, 0.24f, 0.38f));
        Primitives::drawCube(shader, stoolM, vec3(0.35f, 0.22f, 0.10f));

        mat4 lanternM = mat4::identity();
        lanternM = translate(lanternM, vec3(lanternPos.x, 0.24f, lanternPos.z));
        Charpai::drawLantern(shader, lanternM);

        // Person 2: Neighbor seated on wooden bench beside Charpai chatting with Elder
        mat4 benchM = mat4::identity();
        benchM = translate(benchM, vec3(-5.0f, 0.16f, 1.8f));
        benchM = rotate(benchM, radians(50.0f), vec3(0.0f, 1.0f, 0.0f));
        benchM = scale(benchM, vec3(0.42f, 0.30f, 0.90f));
        Primitives::drawCube(shader, benchM, vec3(0.38f, 0.25f, 0.12f));

        PersonParams neighbor;
        neighbor.skinColor   = vec3(0.50f, 0.34f, 0.20f);
        neighbor.shirtColor  = vec3(0.82f, 0.78f, 0.70f); // off-white kurta
        neighbor.pantsColor  = vec3(0.18f, 0.35f, 0.24f); // dark green lungi
        neighbor.seated      = true;
        mat4 neighborM = mat4::identity();
        neighborM = translate(neighborM, vec3(-5.0f, 0.16f, 1.8f));
        neighborM = rotate(neighborM, radians(50.0f), vec3(0.0f, 1.0f, 0.0f));
        neighborM = translate(neighborM, vec3(0.0f, 0.15f, 0.0f));
        Person::draw(shader, neighborM, neighbor);

        // Person 3: Standing Villager in traditional lungi and kurta along courtyard path
        PersonParams standing;
        standing.skinColor   = vec3(0.54f, 0.36f, 0.22f);
        standing.shirtColor  = vec3(0.18f, 0.38f, 0.62f); // blue kurta
        standing.pantsColor  = vec3(0.56f, 0.16f, 0.10f); // red checked lungi
        standing.leftArmAngle  = (animTime > 0.0f) ? (sinf(animTime * 1.8f) * radians(12.0f)) : 0.0f;
        standing.rightArmAngle = (animTime > 0.0f) ? (-sinf(animTime * 1.8f) * radians(12.0f)) : 0.0f;
        mat4 standingM = mat4::identity();
        standingM = translate(standingM, vec3(-1.8f, 0.0f, -1.2f));
        standingM = rotate(standingM, radians(-55.0f), vec3(0.0f, 1.0f, 0.0f));
        Person::draw(shader, standingM, standing);

        // Person 4: Child sitting cross-legged reading open Bengali poem book aloud
        PersonParams child;
        child.skinColor   = vec3(0.52f, 0.36f, 0.22f);
        child.shirtColor  = vec3(0.88f, 0.45f, 0.15f); // saffron shirt
        child.pantsColor  = vec3(0.25f, 0.38f, 0.20f); // green shorts
        child.crossLegged = true;
        float poemNod = (animTime > 0.0f) ? (sinf(animTime * 3.4f) * radians(4.5f)) : 0.0f;
        mat4 childM = mat4::identity();
        childM = translate(childM, vec3(-2.4f, 0.0f, 1.8f));
        childM = rotate(childM, radians(-65.0f), vec3(0.0f, 1.0f, 0.0f));
        childM = rotate(childM, poemNod, vec3(1.0f, 0.0f, 0.0f)); // subtle reciting rhythm
        childM = scale(childM, vec3(0.68f, 0.68f, 0.68f));
        Person::draw(shader, childM, child);

        mat4 rehalM = childM;
        rehalM = translate(rehalM, vec3(0.0f, 0.00f, 0.35f));
        Person::drawRehal(shader, rehalM);

        mat4 bookM = rehalM;
        bookM = translate(bookM, vec3(0.0f, 0.155f, 0.0f));
        bookM = rotate(bookM, radians(20.0f), vec3(1.0f, 0.0f, 0.0f));
        Person::drawBook(shader, bookM);

        // Person 5: Sibling / Playmate sitting cross-legged listening attentively
        PersonParams sibling;
        sibling.skinColor   = vec3(0.50f, 0.34f, 0.20f);
        sibling.shirtColor  = vec3(0.85f, 0.72f, 0.18f); // golden yellow kurta
        sibling.pantsColor  = vec3(0.35f, 0.18f, 0.45f); // purple shorts
        sibling.crossLegged = true;
        mat4 siblingM = mat4::identity();
        siblingM = translate(siblingM, vec3(-1.6f, 0.0f, 2.2f));
        siblingM = rotate(siblingM, radians(-120.0f), vec3(0.0f, 1.0f, 0.0f));
        siblingM = scale(siblingM, vec3(0.62f, 0.62f, 0.62f));
        Person::draw(shader, siblingM, sibling);

        // ─── 7. VILLAGE ANIMALS (HENS & DUCKS IN COURTYARD & RIVER) ─
        // In the concept art and evening gathering, free-range hens and ducks peck in the warm
        // golden glow of the courtyard lantern before roosting.
        {
            // Village Obstacle Bounding Boxes for Animal Collision Avoidance
            // Completely prevents hens and animals from entering houses, walls, plinths, or structures.
            struct VillageObstacle {
                float minX, maxX;
                float minZ, maxZ;
            };

            static const VillageObstacle s_villageObstacles[] = {
                // 1. House 1: Main Central Homestead (Chouchala + verandah & steps)
                { -11.0f,  -6.0f,  -8.0f,  -3.1f },
                // 2. House 2: North Bari Dochala House (walls, verandah & plinth)
                { -16.2f, -11.0f, -10.0f,  -5.1f },
                // 3. House 5: East-Bari Homestead / Annex House
                { -16.8f, -12.2f,  -1.8f,   2.8f },
                // 4. Kitchen Hut (Outdoor Thatched Kitchen)
                {  -7.0f,  -3.4f, -11.2f,  -7.8f },
                // 5. Cow Shed (Gowal Ghor)
                { -19.8f, -15.2f,  -5.2f,  -1.8f },
                // 6. Outdoor Cooking Stove (Matir Chula) & Bamboo Windbreak Fence
                {  -5.2f,  -3.4f,  -6.2f,  -4.6f },
                // 7. Chicken Coop on Stilts (Murgir Khopa - in yard beside House 1)
                { -12.2f, -11.0f,  -4.0f,  -3.0f },
                // 8. Tubewell Concrete Pad (Chapa Kol)
                {  -5.8f,  -4.6f,  -3.0f,  -1.8f },
                // 9. Charpai Bed & Seated Villagers
                {  -5.5f,  -3.2f,  -0.2f,   2.5f },
                // 10. Courtyard Perimeter Bamboo Fence (Fence 6: keeps poultry inside courtyard)
                {  -6.2f,   0.6f,   3.6f,   4.2f }
            };

            auto resolveHenCollisions = [](float& hx, float& hz, float bodyRadius) {
                for (const auto& obs : s_villageObstacles) {
                    float bMinX = obs.minX - bodyRadius;
                    float bMaxX = obs.maxX + bodyRadius;
                    float bMinZ = obs.minZ - bodyRadius;
                    float bMaxZ = obs.maxZ + bodyRadius;

                    if (hx > bMinX && hx < bMaxX && hz > bMinZ && hz < bMaxZ) {
                        float dLeft   = hx - bMinX;
                        float dRight  = bMaxX - hx;
                        float dBack   = hz - bMinZ;
                        float dFront  = bMaxZ - hz;

                        float minD = dLeft;
                        if (dRight < minD) minD = dRight;
                        if (dBack  < minD) minD = dBack;
                        if (dFront < minD) minD = dFront;

                        if (minD == dLeft)        hx = bMinX;
                        else if (minD == dRight)  hx = bMaxX;
                        else if (minD == dBack)   hz = bMinZ;
                        else                      hz = bMaxZ;
                    }
                }
            };

            // Flock of 5 Free-Range Village Hens (Deshi Murgi)
            // Trajectories placed in authentic open-yard zones, guaranteed never to cross walls
            struct HenPath {
                vec3  basePos;
                float radiusX;
                float radiusZ;
                float speed;
                float phase;
                float scaleVal;
            };
            static const HenPath s_henPaths[5] = {
                { vec3(-6.2f, 0.0f, -0.6f), 0.75f, 0.55f, 0.75f, 0.0f, 1.00f }, // Central courtyard (open earth)
                { vec3(-10.2f, 0.0f, -2.4f), 0.65f, 0.55f, 0.65f, 1.8f, 0.92f }, // Open yard in front of chicken coop
                { vec3(-6.8f, 0.0f,  1.0f), 0.55f, 0.50f, 0.80f, 3.4f, 0.95f }, // West courtyard open earth
                { vec3(-2.2f, 0.0f,  3.2f), 0.50f, 0.45f, 0.70f, 4.8f, 0.88f }, // Foreground courtyard inside fence
                { vec3(-12.8f, 0.0f, -3.4f), 0.70f, 0.55f, 0.60f, 2.2f, 0.90f } // North farmyard between Dochala & Cow Shed
            };

            for (int h = 0; h < 5; ++h) {
                const auto& hp = s_henPaths[h];
                float henTheta = animTime * hp.speed + hp.phase;
                float rawHx = hp.basePos.x + sinf(henTheta) * hp.radiusX;
                float rawHz = hp.basePos.z + cosf(henTheta) * hp.radiusZ;

                float hx = rawHx;
                float hz = rawHz;
                resolveHenCollisions(hx, hz, 0.18f);

                // Lookahead sample for tangent direction with collision awareness
                float nextTheta = henTheta + 0.06f;
                float nextHx = hp.basePos.x + sinf(nextTheta) * hp.radiusX;
                float nextHz = hp.basePos.z + cosf(nextTheta) * hp.radiusZ;
                resolveHenCollisions(nextHx, nextHz, 0.18f);

                float vhx = nextHx - hx;
                float vhz = nextHz - hz;
                float henYaw = (fabsf(vhx) > 1e-4f || fabsf(vhz) > 1e-4f) ?
                               atan2f(vhx, vhz) :
                               atan2f(cosf(henTheta) * hp.radiusX, -sinf(henTheta) * hp.radiusZ);

                // Periodic pecking behavior: when pausing momentarily in cycle
                float peckCycle = sinf(henTheta * 2.5f);
                float isPecking = (peckCycle > 0.25f);
                float henPeckAngle = isPecking ? (sinf(animTime * 8.5f + h) * radians(16.0f)) : 0.0f;

                // Authentic side-to-side waddle gait while walking
                float henWaddle = (!isPecking && animTime > 0.0f) ? (sinf(animTime * 9.0f + h * 2.0f) * radians(3.8f)) : 0.0f;

                mat4 henM = mat4::identity();
                henM = translate(henM, vec3(hx, 0.0f, hz));
                henM = rotate(henM, henYaw, vec3(0.0f, 1.0f, 0.0f));
                henM = rotate(henM, henWaddle, vec3(0.0f, 0.0f, 1.0f));
                henM = rotate(henM, henPeckAngle, vec3(1.0f, 0.0f, 0.0f));
                henM = scale(henM, vec3(hp.scaleVal));
                Hen::draw(shader, henM);
            }

            // Pair of white ducks (Pati Hash) resting and preening on riverbank grass near Landing Ghat
            mat4 duckYard1 = mat4::identity();
            duckYard1 = translate(duckYard1, vec3(4.2f, 0.0f, 0.4f));
            duckYard1 = rotate(duckYard1, radians(35.0f + (animTime > 0.0f ? sinf(animTime * 1.6f) * 6.0f : 0.0f)), vec3(0.0f, 1.0f, 0.0f));
            duckYard1 = scale(duckYard1, vec3(0.92f));
            Duck::draw(shader, duckYard1);

            mat4 duckYard2 = mat4::identity();
            duckYard2 = translate(duckYard2, vec3(4.5f, 0.0f, 0.7f));
            duckYard2 = rotate(duckYard2, radians(60.0f - (animTime > 0.0f ? sinf(animTime * 1.9f) * 5.0f : 0.0f)), vec3(0.0f, 1.0f, 0.0f));
            duckYard2 = scale(duckYard2, vec3(0.88f));
            Duck::draw(shader, duckYard2);

            // Flock of 4 River Ducks (Pati Hash) swimming exploratory loops along river shallows
            struct DuckPath {
                vec3  basePos;
                float swimRadiusX;
                float swimRadiusZ;
                float speed;
                float phase;
                float scaleVal;
            };
            static const DuckPath s_duckPaths[4] = {
                { vec3(8.8f, 0.0f, -1.8f), 0.75f, 1.40f, 0.45f, 0.0f, 1.00f }, // South open river shallows (clear of boat channel)
                { vec3(9.2f, 0.0f, -3.8f), 0.65f, 1.20f, 0.50f, 1.5f, 0.88f }, // Out on wide open water
                { vec3(6.8f, 0.0f,  3.8f), 0.80f, 1.60f, 0.40f, 3.1f, 0.95f }, // North reed shallows
                { vec3(6.2f, 0.0f,  5.6f), 0.60f, 1.10f, 0.48f, 4.6f, 0.92f }  // Near Shapla lilies
            };

            for (int d = 0; d < 4; ++d) {
                const auto& dp = s_duckPaths[d];
                float duckTheta = animTime * dp.speed + dp.phase;
                float dx = dp.basePos.x + sinf(duckTheta) * dp.swimRadiusX;
                float dz = dp.basePos.z + cosf(duckTheta) * dp.swimRadiusZ;

                // Facing heading tangent
                float vdx = cosf(duckTheta) * dp.swimRadiusX;
                float vdz = -sinf(duckTheta) * dp.swimRadiusZ;
                float duckYaw = atan2f(vdx, vdz);

                // Water bobbing and paddling pitch
                float duckBob = (animTime > 0.0f) ? (sinf(animTime * 2.8f + d * 1.4f) * 0.022f) : 0.0f;
                float duckPitch = (animTime > 0.0f) ? (sinf(animTime * 3.6f + d) * radians(2.2f)) : 0.0f;

                mat4 duckM = mat4::identity();
                duckM = translate(duckM, vec3(dx, 0.02f + duckBob, dz));
                duckM = rotate(duckM, duckYaw, vec3(0.0f, 1.0f, 0.0f));
                duckM = rotate(duckM, duckPitch, vec3(1.0f, 0.0f, 0.0f));
                duckM = scale(duckM, vec3(dp.scaleVal));
                Duck::draw(shader, duckM);
            }
        }

        // ─── 8. TRADITIONAL DINGI BOATS & ROWING BOATMAN ─────────
        Texture::bind(TEX_WOOD, 0);
        shader.setInt("uTextureType", (int)TEX_WOOD);
        // Boat 1: Moored Dingi Nouka with Hanging Hariken at Landing Ghat
        float boatBob1  = (animTime > 0.0f) ? (sinf(animTime * 1.6f) * 0.018f) : 0.0f;
        float boatRoll1 = (animTime > 0.0f) ? (sinf(animTime * 1.4f) * radians(1.2f)) : 0.0f;

        mat4 boatM1 = mat4::identity();
        boatM1 = translate(boatM1, vec3(7.0f, 0.11f + boatBob1, 1.2f));
        boatM1 = rotate(boatM1, radians(6.0f), vec3(0.0f, 1.0f, 0.0f));
        boatM1 = rotate(boatM1, boatRoll1, vec3(0.0f, 0.0f, 1.0f));
        Boat::draw(shader, boatM1, true, animTime);

        // Boat 2: Cruising Dingi Nouka navigating the winding river channel with rowing Boatman!
        mat4 boatM2 = mat4::identity();
        boatM2 = translate(boatM2, vec3(boatX, 0.10f + boatBob2, boatZ + rowingSurge));
        boatM2 = rotate(boatM2, boatCourseYaw, vec3(0.0f, 1.0f, 0.0f));
        boatM2 = rotate(boatM2, boatRoll2, vec3(0.0f, 0.0f, 1.0f));
        boatM2 = rotate(boatM2, boatPitch2, vec3(1.0f, 0.0f, 0.0f));
        Boat::draw(shader, boatM2, true, animTime);

        // Seated Boatman inside Boat 2 rowing with Boitha oar
        Texture::bind(TEX_FABRIC, 0);
        shader.setInt("uTextureType", (int)TEX_FABRIC);
        mat4 boatmanM2 = boatM2;
        boatmanM2 = translate(boatmanM2, vec3(0.0f, 0.13f, -1.30f));
        boatmanM2 = rotate(boatmanM2, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
        Boatman::draw(shader, boatmanM2, oarRowAnim);

        // ─── 9. CURVED ARCHED BAMBOO FOOTBRIDGE (BANSHER SAAKO / বাঁশের সাঁকো) ───
        // Traditional rural Bangladeshi curved footbridge spanning the river inlet.
        // Features parabolic arched bamboo walkway, 24 curved cross-slats,
        // curved double handrails, and cross-braced diagonal stilts.
        mat4 bridgeM = mat4::identity();
        bridgeM = translate(bridgeM, vec3(7.6f, 0.0f, -14.5f));
        bridgeM = rotate(bridgeM, radians(85.0f), vec3(0.0f, 1.0f, 0.0f));
        Texture::bind(TEX_BAMBOO, 0);
        shader.setInt("uTextureType", (int)TEX_BAMBOO);
        CurvedObject::drawBambooBridge(shader, bridgeM);

        // ── Swap Buffers & Poll Events ──────────────────────────
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ── Cleanup ─────────────────────────────────────────────────
    CurvedObject::cleanup();
    Boat::cleanup();
    Texture::cleanup();
    Primitives::cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}