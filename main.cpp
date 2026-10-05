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
#include <chrono>
#include <thread>

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
#include "objects/Sun.h"
#include "objects/Stars.h"
#include "objects/Fireflies.h"
#include "Texture.h"
#include "objects/CurvedObject.h"
#include "objects/Fisherman.h"

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
    {  0.0f, vec3(-3.0f, 1.5f,  0.0f), -40.0f, 26.0f, 48.0f },  // 1: Grand Full-Plane Panoramic Village View (West & East Villages, River, Mosque & Moon)
    { 10.0f, vec3( 8.8f, 0.60f,  0.0f), -82.0f, 14.0f, 19.5f },  // 2: River & Traditional Boats Panorama (Artwork Perspective)
    { 19.0f, vec3( 5.5f, 0.80f,  1.2f), -40.0f, 16.0f,  9.0f },  // 3: River Landing Ghat, Net Drying Racks & Fishing Cottages
    { 28.0f, vec3(-3.5f, 0.75f,  1.0f),  45.0f, 16.0f,  7.2f },  // 4: Central Courtyard Gathering, Elders, Children & Tubewell
    { 38.0f, vec3(-33.0f, 1.90f,-34.0f),  20.0f, 16.0f, 15.0f },  // 5: Village Mosque & Walking Devout Elder
    { 48.0f, vec3(-20.5f, 1.20f,-12.5f), 35.0f, 18.0f, 11.5f },  // 6: North Farmstead, Cow Shed, Deshi Cows & Straw Stacks
    { 58.0f, vec3(-23.0f, 1.20f,  5.5f), 45.0f, 18.0f, 12.5f },  // 7: West Homesteads, Weaver's House & Paddy Granary (Dhaner Gola)
    { 68.0f, vec3(-11.5f, 1.40f, 18.5f), 30.0f, 20.0f, 14.5f },  // 8: South Homestead, Vegetable Trellis & Terraced Paddy Fields
    { 78.0f, vec3(-3.0f, 1.5f,  0.0f), -40.0f, 26.0f, 48.0f }   // Loop back to full-plane panoramic village view
};
static const int   g_numTourFrames = 9;
static const float g_tourDuration  = 78.0f;
static float       g_tourTime      = 0.0f;

// Interactive Controls State
static float       g_pumpTimer     = 0.0f; // Interactive Tubewell pumping
static int         g_lanternMode   = 0;    // 0: Normal, 1: High Flame, 2: Soft, 3: Extinguished
static float       g_animSpeed     = 1.0f; // Animation speed multiplier
static bool        g_windBreeze    = true; // Summer breeze wind sway

// Interactive Bullock Cart (Gorur Gari) Drive State
static bool        g_driveCartMode         = false;    // Toggle with Key 'R' or Key 'G'
static float       g_cartX                 = 22.0f;    // Initial X on Grameen Rasta
static float       g_cartZ                 = 2.5f;     // Initial Z on Grameen Rasta
static float       g_cartHeading           = 180.0f;   // Heading angle in degrees (180 = heading North -Z)
static float       g_cartSpeed             = 0.0f;     // Forward/reverse speed (m/s)
static float       g_cartWheelRot          = 0.0f;     // Accumulated wheel rotation
static float       g_cartWalkPhase         = 0.0f;     // Oxen walking stride phase
static const float CART_MAX_SPEED          = 3.6f;     // Max trotting speed
static const float CART_ACCEL              = 3.4f;     // Acceleration
static const float CART_DECEL              = 4.2f;     // Deceleration/friction
static const float CART_TURN_SPEED         = 55.0f;    // Steering rate (deg/s)

// Key 'G' 180-Degree Step Advance State (Chaka Rotates 180° & Translates Ahead)
static const float CART_WHEEL_RADIUS       = 0.72f;       // Wheel radius in meters
static const float CART_STEP_ANGLE         = 3.14159265f; // 180 degrees (pi radians)
static const float CART_STEP_DIST          = CART_STEP_ANGLE * CART_WHEEL_RADIUS; // ~2.26195m
static float       g_cartStepDistRemaining = 0.0f;        // Remaining distance to translate ahead
static int         g_cartStepCount         = 0;           // Total 180-degree steps executed

// Interactive Dingi Nouka (Country Boat) Drive State
static bool        g_driveBoatMode         = false;    // Toggle with Key 'N'
static float       g_boatX                 = 9.82f;    // Initial X on River (around Z = 2.6f)
static float       g_boatZ                 = 2.6f;     // Initial Z on River
static float       g_boatYaw               = 0.15f;    // Heading angle in radians (aligned with river flow)
static float       g_boatSpeed             = 0.0f;     // Forward/reverse speed (m/s)
static float       g_boatOarPhase          = 0.0f;     // Dynamic rowing stroke phase
static const float BOAT_MAX_SPEED          = 4.2f;     // Max rowing sprint speed
static const float BOAT_ACCEL              = 2.8f;     // Rowing acceleration
static const float BOAT_DRAG               = 1.6f;     // Water hydrodynamic drag / resistance
static const float BOAT_TURN_SPEED         = 1.4f;     // Rudder steering rate (rad/s)

// Key 'N' Step Advance State (Nouka Moves Ahead on Every Press)
static const float BOAT_STEP_DIST          = 2.0f;     // Distance moved ahead (meters) per rowing stroke on Key 'N'
static float       g_boatStepDistRemaining = 0.0f;     // Remaining distance to glide ahead
static int         g_boatStepCount         = 0;        // Total boat advance steps executed

static void keyCallback(GLFWwindow* window, int key, int, int action, int)
{
    if (action != GLFW_PRESS && ((key != GLFW_KEY_G && key != GLFW_KEY_N) || action != GLFW_REPEAT)) return;

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
            "1: Radiant Daytime (Radiant Sun giving golden daylight, bright blue sky, clear village visibility)",
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
        g_driveCartMode = false;
        g_driveBoatMode = false;
        camera.target   = vec3(-3.5f, 0.75f, 1.0f);
        camera.yaw      = radians(38.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 7.2f;
        camera.updatePosition();
        std::cout << "View 1: Courtyard Gathering (Charpai, Seated Elder with Fan, Neighbor, Children, Hens & Lantern)\n";
    }
    else if (key == GLFW_KEY_2) {
        g_tourActive = false;
        g_driveCartMode = false;
        g_driveBoatMode = false;
        camera.target   = vec3(8.8f, 0.80f, 0.5f);
        camera.yaw      = radians(-85.0f);
        camera.pitch    = radians(12.0f);
        camera.distance = 21.0f;
        camera.updatePosition();
        std::cout << "View 2: Panoramic River & Boats View (Red-Sail Boat, Swimmers, Round Chhoi Boat, White-Sail Boat, Sandy Beaches & Far Bank Horizon) [Press 'N' to pilot & drive the Dingi Nouka!]\n";
    }
    else if (key == GLFW_KEY_3) {
        g_tourActive = false;
        g_driveCartMode = false;
        g_driveBoatMode = false;
        camera.target   = vec3(-33.0f, 1.8f, -34.0f);
        camera.yaw      = radians(12.0f);
        camera.pitch    = radians(14.0f);
        camera.distance = 18.0f;
        camera.updatePosition();
        std::cout << "View 3: Historic Terracotta Brick & Old Red Stone Village Mosque (Gramin Masjid with Burnt-Clay Terracotta Domes, Ancient Red Stone Plinth & Soaring Minaret)\n";
    }
    else if (key == GLFW_KEY_4) {
        g_tourActive = false;
        g_driveCartMode = false;
        g_driveBoatMode = false;
        camera.target   = vec3(-20.5f, 1.4f, -13.0f);
        camera.yaw      = radians(32.0f);
        camera.pitch    = radians(18.0f);
        camera.distance = 14.5f;
        camera.updatePosition();
        std::cout << "View 4: North Farmstead (Uttar Bari: Main Dochala House, Farm Cottage 2B, Thatched Cow Shed with 2 Deshi Cows & Straw Stacks)\n";
    }
    else if (key == GLFW_KEY_5) {
        g_tourActive = false;
        g_driveCartMode = false;
        g_driveBoatMode = false;
        camera.target   = vec3(-11.5f, 1.2f, 19.5f);
        camera.yaw      = radians(32.0f);
        camera.pitch    = radians(20.0f);
        camera.distance = 16.5f;
        camera.updatePosition();
        std::cout << "View 5: South Agricultural Expanse (Dokkhin Bari: Homesteads 3, 3B, 3C, Vegetable Trellis, Straw Stack & Terraced Paddy Fields)\n";
    }
    else if (key == GLFW_KEY_6) {
        g_tourActive = false;
        g_driveCartMode = false;
        g_driveBoatMode = false;
        camera.target   = vec3(1.5f, 1.6f, -4.5f);
        camera.yaw      = radians(-15.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 14.5f;
        camera.updatePosition();
        std::cout << "View 6: Rural Trees & Riverbank Reeds (Palms, Banana, Mango, Bamboo Groves & Kashbon)\n";
    }
    else if (key == GLFW_KEY_7) {
        g_tourActive = false;
        g_driveCartMode = false;
        g_driveBoatMode = false;
        if (lightingMode == 0) {
            // At night: inspect hens roosting safely inside the Chicken Coop (Murgir Khopa)
            camera.target   = vec3(-16.0f, 0.6f, -10.8f);
            camera.yaw      = radians(24.0f);
            camera.pitch    = radians(12.0f);
            camera.distance = 3.8f;
            camera.updatePosition();
            std::cout << "View 7 (Night): Village Hens safely roosting inside Chicken Coop (Murgir Khopa)\n";
        } else {
            camera.target   = vec3(-4.5f, 0.4f, 0.5f);
            camera.yaw      = radians(25.0f);
            camera.pitch    = radians(14.0f);
            camera.distance = 6.5f;
            camera.updatePosition();
            std::cout << "View 7 (Day): Free-Range Hens Pecking in Courtyard & Swimming Ducks\n";
        }
    }
    else if (key == GLFW_KEY_8) {
        g_tourActive = false;
        g_driveCartMode = false;
        g_driveBoatMode = false;
        camera.target   = vec3(-3.0f, 1.5f, 0.0f);
        camera.yaw      = radians(-40.0f);
        camera.pitch    = radians(26.0f);
        camera.distance = 48.0f;
        camera.updatePosition();
        std::cout << "View 8: Grand Full-Plane Panoramic Village View - Richly Distributed Bangladeshi Rural Village (33 Cottages, 10 Homesteads, West & East River Villages, Courtyards, River, Mosque, Fields & Moon)\n";
    }
    else if (key == GLFW_KEY_9) {
        g_tourActive = false;
        g_driveCartMode = false;
        g_driveBoatMode = false;
        camera.target   = vec3(1.2f, 0.55f, -2.6f);
        camera.yaw      = radians(-22.0f);
        camera.pitch    = radians(14.0f);
        camera.distance = 3.6f;
        camera.updatePosition();
        std::cout << "View 9: Riverbank Duck House (Hash-er Ghor) with Ducks Resting Inside at Night\n";
    }
    else if (key == GLFW_KEY_0) {
        g_tourActive = false;
        g_driveCartMode = false;
        g_driveBoatMode = false;
        camera.target   = vec3(-25.5f, 0.9f, -10.5f);
        camera.yaw      = radians(25.0f);
        camera.pitch    = radians(15.0f);
        camera.distance = 8.5f;
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
    // Key 'V': Inspect Parametric Curved Bézier Terracotta Surahi
    else if (key == GLFW_KEY_V) {
        g_tourActive = false;
        g_driveCartMode = false;
        g_driveBoatMode = false;
        camera.target   = vec3(-20.2f, 0.45f, 5.2f);
        camera.yaw      = radians(45.0f);
        camera.pitch    = radians(20.0f);
        camera.distance = 3.6f;
        camera.updatePosition();
        std::cout << "View V: Parametric Cubic Bézier Terracotta Vase (Surahi / সুরাহি) with Analytic Normals\n";
    }
    // Key 'F': View River Fisherman Hunting Fish
    else if (key == GLFW_KEY_F) {
        g_tourActive = false;
        g_driveCartMode = false;
        g_driveBoatMode = false;
        float fisherZ = 8.5f;
        float fisherX = 8.0f + (1.8f * sinf(fisherZ * 0.08f + 0.4f) + 0.6f * cosf(fisherZ * 0.04f)) + 1.8f;
        camera.target   = vec3(fisherX - 0.25f, 0.45f, fisherZ + 0.65f);
        camera.yaw      = radians(-38.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 5.2f;
        camera.updatePosition();
        std::cout << "View F: River Fisherman Hunting Fish (নদীতে খেপলা জাল ও মাছ শিকারী জেলে — Cast Net, Leaping Silver Fish, Polo & Bamboo Khalui)\n";
    }
    // Key 'C': Capture All Objects
    else if (key == GLFW_KEY_C) {
        if (g_window && g_shader) {
            captureAllObjects(g_window, *g_shader);
        }
    }
    // Key 'R': Inspect / Enter Interactive Driving Mode for Gorur Gari
    else if (key == GLFW_KEY_R) {
        g_tourActive = false;
        g_driveBoatMode = false;
        g_driveCartMode = true;
        camera.target   = vec3(g_cartX, 1.15f, g_cartZ);
        camera.yaw      = radians(-g_cartHeading + 180.0f);
        camera.pitch    = radians(14.0f);
        camera.distance = 9.2f;
        camera.updatePosition();
        std::cout << "\n========================================================\n"
                  << ">> GORUR GARI CHASE VIEW: ACTIVE! (Key 'R' / 'G')\n"
                  << "   [Key 'G']         : Rotate Chaka 180° (Half-Turn) & Translate Cart Ahead!\n"
                  << "   [W / Up Arrow]    : Drive Continuously Forward\n"
                  << "   [S / Down Arrow]  : Brake / Reverse\n"
                  << "   [A / Left Arrow]  : Steer Left\n"
                  << "   [D / Right Arrow] : Steer Right\n"
                  << "   [Key 'N']         : Switch to Boat Driving Mode\n"
                  << "   [Key '1' - '8']   : Return to Village Views\n"
                  << "========================================================\n";
    }
    // Key 'G': Gorur Gari step advance — for every press chaka rotates 180 degrees and translates ahead!
    else if (key == GLFW_KEY_G) {
        g_driveCartMode = true; // Switch camera to follow Gorur Gari
        g_driveBoatMode = false;
        g_tourActive    = false;

        // Ensure camera looks directly at the cart
        camera.target   = vec3(g_cartX, 1.15f, g_cartZ);
        camera.yaw      = radians(-g_cartHeading + 180.0f);
        camera.pitch    = radians(14.0f);
        camera.distance = 9.2f;
        camera.updatePosition();

        // Queue a 180-degree wheel rotation (PI rad) and forward translation ahead (~2.26m)
        g_cartStepDistRemaining += CART_STEP_DIST;
        g_cartStepCount++;

        float totalRotDeg = (g_cartWheelRot + g_cartStepDistRemaining / CART_WHEEL_RADIUS) * (180.0f / PI);
        std::cout << "\n>> [KEY 'G' PRESSED] Gorur Gari Step #" << g_cartStepCount << ":\n"
                  << "   * Chaka (Wheel) Rotating +180° (Half-Turn) [Target: "
                  << (int)std::round(totalRotDeg) << "°]\n"
                  << "   * Translating Ahead by " << std::fixed << std::setprecision(2)
                  << CART_STEP_DIST << "m along Grameen Rasta!\n"
                  << "   * Cart Position: (" << std::fixed << std::setprecision(2)
                  << g_cartX << ", " << g_cartZ << ") | Heading: "
                  << (int)std::round(g_cartHeading) << "°\n";
    }
    // Key 'N': Dingi Nouka step advance — for every press nouka moves ahead some distance and rows oar!
    else if (key == GLFW_KEY_N) {
        g_driveBoatMode = true; // Switch camera to follow Dingi Nouka
        g_driveCartMode = false;
        g_tourActive    = false;

        // Ensure camera looks directly at the boat from behind
        camera.target   = vec3(g_boatX, 0.65f, g_boatZ);
        camera.yaw      = g_boatYaw + PI;
        camera.pitch    = radians(15.0f);
        camera.distance = 7.5f;
        camera.updatePosition();

        // Queue a rowing stroke step: move boat ahead by 2.0 meters
        g_boatStepDistRemaining += BOAT_STEP_DIST;
        g_boatStepCount++;

        std::cout << "\n>> [KEY 'N' PRESSED] Dingi Nouka Step #" << g_boatStepCount << ":\n"
                  << "   * Boitha Oar Rowing Stroke Animated (Majhi Rowing)!\n"
                  << "   * Nouka Moving Ahead by " << std::fixed << std::setprecision(2)
                  << BOAT_STEP_DIST << "m along River Channel!\n"
                  << "   * Boat Position: (" << std::fixed << std::setprecision(2)
                  << g_boatX << ", " << g_boatZ << ") | Heading: "
                  << (int)std::round(g_boatYaw * (180.0f / PI)) << "°\n";
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
    std::cout << "  CAPTURING ALL 33 INDIVIDUAL OBJECTS INTO 'object_images/'\n";
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
            "Historic Terracotta Brick & Old Red Stone Village Mosque (Gramin Masjid with Burnt-Clay Terracotta Domes, Ancient Red Stone Plinth & Soaring Minaret)",
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
        },
        {
            "31_bullock_cart_gorur_gari",
            "Traditional Rural Bangladeshi Bullock Cart (Gorur Gari / গরুর গাড়ি with Pair of Oxen & Gariyal)",
            vec3(0.0f, 0.90f, 0.8f), 6.8f, 38.0f, 16.0f,
            [](Shader& s) {
                House::drawBullockCart(s, mat4::identity(), 0.0f);
            }
        },
        {
            "32_chhoi_boat_passengers",
            "Traditional Round Chhoi Ferry Boat with Rowing Majhi & 2 Passengers (খেয়া নৌকা ও যাত্রীদ্বয়)",
            vec3(0.0f, 0.48f, 0.25f), 4.3f, 32.0f, 22.0f,
            [](Shader& s) {
                Boat::draw(s, mat4::identity(), Boat::BOAT_STYLE_ROUND_CHHOI, 0.0f, 0.0f, 2);
            }
        },
        {
            "33_fisherman_hunting_fish",
            "Traditional River Fisherman Hunting Fish with Cast Net & Leaping River Fish (নদীতে খেপলা জাল ও মাছ শিকারী জেলে)",
            vec3(0.0f, 0.52f, 1.15f), 5.4f, 40.0f, 18.0f,
            [](Shader& s) {
                Fisherman::draw(s, mat4::identity(), 1.2f);
            }
        },
        {
            "34_radiant_sun",
            "Radiant Daytime Sun with Glowing Corona Halo & Solar Flare Rays (Surjo / সূর্য)",
            vec3(0.0f, 0.0f, 0.0f), 12.0f, 0.0f, 0.0f,
            [](Shader& s) {
                Sun::draw(s, mat4::identity(), 1.0f);
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

        vec3 bg = (item.name == "22_full_moon") ? vec3(0.04f, 0.06f, 0.14f) :
                  ((item.name == "34_radiant_sun") ? vec3(0.48f, 0.72f, 0.92f) : clearColor);
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
    std::cout << "  Key 'L'         : Toggle Lighting (0: Moonlit Night | 1: Radiant Day Sun & Light | 2: Unlit Facets | 3: Flat)\n";
    std::cout << "  Key 'X'         : Toggle Texture Mode (0: Solid | 1: Procedural Detailing | 2: GPU Texture Maps)\n";
    std::cout << "  Key 'V'         : View V - Parametric Curved Bézier Vase (Terracotta Surahi)\n";
    std::cout << "  Key 'F'         : View F - River Fisherman Hunting Fish (Cast Net, Leaping Silver Fish & Gear)\n";
    std::cout << "  Key '1'         : View 1 - Courtyard Gathering (Charpai, Elders, Children Reading, Hens)\n";
    std::cout << "  Key '2'         : View 2 - River Shore, Landing Ghat, Moored Boat & Rowing Boatman\n";
    std::cout << "  Key '3'         : View 3 - Traditional Bangladeshi Village Mosque (Gramin Masjid)\n";
    std::cout << "  Key '4'         : View 4 - North Homestead (Dochala, Cow Shed & Straw Stack)\n";
    std::cout << "  Key '5'         : View 5 - South Homestead, Straw Stack & Terraced Paddy Fields\n";
    std::cout << "  Key '6'         : View 6 - Rural Trees & Riverbank Reeds\n";
    std::cout << "  Key '7'         : View 7 - Village Animals (Flocks of Hens & River Ducks)\n";
    std::cout << "  Key '8'         : View 8 - Grand Full-Plane Panoramic Village View (33 Cottages, West & East River Villages)\n";
    std::cout << "  Key '9'         : View 9 - Hand-Pump Tubewell & Clay Cooking Kitchen\n";
    std::cout << "  Key '0'         : View 0 - Thatched Cow Shed & Resting Deshi Cow\n";
    std::cout << "  Key 'T'         : Toggle Terrain/Ground Visibility\n";
    std::cout << "  Key 'C'         : Capture All 34 Objects to 'object_images/'\n";
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

    bool g_shotMode  = (argc > 1 && (std::string(argv[1]) == "--shot" || std::string(argv[1]) == "-s"));
    bool g_shotDay   = (argc > 1 && (std::string(argv[1]) == "--shot-day" || std::string(argv[1]) == "--shot-sun"));
    bool g_shotField     = (argc > 1 && (std::string(argv[1]) == "--shot-field" || std::string(argv[1]) == "--shot-house"));
    bool g_shotBari8     = (argc > 1 && (std::string(argv[1]) == "--shot-bari8"));
    bool g_shotMosque    = (argc > 1 && (std::string(argv[1]) == "--shot-mosque"));
    bool g_shotOldMosque = (argc > 1 && (std::string(argv[1]) == "--shot-oldmosque"));
    bool g_shotPump      = (argc > 1 && (std::string(argv[1]) == "--shot-pump" || std::string(argv[1]) == "--shot-tubewell"));
    bool g_shotTrees     = (argc > 1 && (std::string(argv[1]) == "--shot-trees" || std::string(argv[1]) == "--shot-tree" || std::string(argv[1]) == "--shot-paddy"));
    bool g_shotRiver     = (argc > 1 && (std::string(argv[1]) == "--shot-river" || std::string(argv[1]) == "--shot-palm"));
    bool g_shotFisher    = (argc > 1 && (std::string(argv[1]) == "--shot-fisher" || std::string(argv[1]) == "--shot-boat"));
    bool g_shotCow       = (argc > 1 && (std::string(argv[1]) == "--shot-cow" || std::string(argv[1]) == "--shot-cowshed"));
    if (g_shotCow) {
        g_shotMode = true;
    }
    if (g_shotDay || g_shotField || g_shotBari8 || g_shotMosque || g_shotOldMosque || g_shotPump || g_shotTrees || g_shotRiver || g_shotFisher) {
        lightingMode = 1; // Radiant Daytime
        g_shotMode = true;
    }

    // ── Camera Default Setup: Grand Full-Plane Panoramic Bangladeshi Village View ──
    if (g_shotCow) {
        // Perspective framing shifted Uttar Bari Cow Shed, House 2, and farmyard cattle
        camera.target   = vec3(-25.5f, 0.9f, -10.5f);
        camera.yaw      = radians(25.0f);
        camera.pitch    = radians(15.0f);
        camera.distance = 8.5f;
        camera.updatePosition();
    } else if (g_shotFisher) {
        // Perspective focusing directly on the solitary fisherman's boat and shifted sailboat downstream
        float fisherZ = 8.5f;
        float fisherX = 8.0f + (1.8f * sinf(fisherZ * 0.08f + 0.4f) + 0.6f * cosf(fisherZ * 0.04f)) + 1.8f;
        camera.target   = vec3(fisherX - 0.25f, 0.45f, fisherZ + 0.65f);
        camera.yaw      = radians(-38.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 9.5f;
        camera.updatePosition();
    } else if (g_shotRiver) {
        // Perspective looking across the clear river channel and banks
        camera.target   = vec3(3.0f, 1.2f, 0.0f);
        camera.yaw      = radians(-55.0f);
        camera.pitch    = radians(18.0f);
        camera.distance = 16.5f;
        camera.updatePosition();
    } else if (g_shotTrees) {
        // Perspective matching user's photo looking across Plot 2 rice field towards Dokkhin Bari and shifted trees
        camera.target   = vec3(-26.8f, 1.3f, 26.0f);
        camera.yaw      = radians(-22.0f);
        camera.pitch    = radians(22.0f);
        camera.distance = 13.5f;
        camera.updatePosition();
    } else if (g_shotPump) {
        // Perspective looking down at shifted tubewell and House 1 verandah (matching user's view)
        camera.target   = vec3(-4.4f, 0.7f, -1.9f);
        camera.yaw      = radians(58.0f);
        camera.pitch    = radians(28.0f);
        camera.distance = 5.5f;
        camera.updatePosition();
    } else if (g_shotOldMosque) {
        // Perspective of old mosque location: now a cleared, open riverfront meadow
        camera.target   = vec3(-3.8f, 1.2f, -17.5f);
        camera.yaw      = radians(10.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 15.0f;
        camera.updatePosition();
    } else if (g_shotMosque) {
        // Panoramic perspective capturing the Historic Village Mosque in the North-West village corner
        camera.target   = vec3(-33.0f, 1.8f, -34.0f);
        camera.yaw      = radians(12.0f);
        camera.pitch    = radians(14.0f);
        camera.distance = 18.0f;
        camera.updatePosition();
    } else if (g_shotBari8) {
        // Elevated perspective looking at South Agricultural Settlement (Bari 8) and adjacent paddy fields
        camera.target   = vec3(-20.0f, 1.5f, 35.0f);
        camera.yaw      = radians(25.0f);
        camera.pitch    = radians(22.0f);
        camera.distance = 22.0f;
        camera.updatePosition();
    } else if (g_shotField) {
        // Perspective matching the user's photo: looking west over the rice field and shifted house on normal ground
        camera.target   = vec3(34.0f, 1.2f, 23.5f);
        camera.yaw      = radians(98.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 18.0f;
        camera.updatePosition();
    } else if (g_shotDay) {
        // Elevated perspective capturing both the radiant Sun in the sky and the sunlit village below
        camera.target   = vec3(-6.0f, 10.0f, 2.0f);
        camera.yaw      = radians(135.0f);
        camera.pitch    = radians(10.0f);
        camera.distance = 36.0f;
        camera.updatePosition();
    } else if (g_shotMode) {
        camera.target   = vec3(-0.4f, 1.2f, -14.0f);
        camera.yaw      = radians(140.0f);
        camera.pitch    = radians(22.0f);
        camera.distance = 12.0f;
        camera.updatePosition();
    } else {
        camera.target   = vec3(-3.0f, 1.5f, 0.0f);
        camera.yaw      = radians(-40.0f);
        camera.pitch    = radians(26.0f);
        camera.distance = 48.0f;
        camera.updatePosition();
    }

    // Lantern position in courtyard (acts as point light source)
    vec3 lanternPos(-3.0f, 0.40f, 0.6f);

    float lastFrameTime = (float)glfwGetTime();

    // ═════════════════════════════════════════════════════════════
    // RENDER LOOP
    // ═════════════════════════════════════════════════════════════
    while (!glfwWindowShouldClose(window))
    {
        // ── Hardware Throttling: Minimized / Iconified State ────────
        // If window is minimized, sleep and wait for events so CPU/GPU drop to 0%
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
            glfwWaitEventsTimeout(0.05); // Sleep 50ms waiting for OS restore event
            lastFrameTime = (float)glfwGetTime();
            continue;
        }

        // ── Hardware Protection: High-Precision Frame Pacer (Solid 60 FPS Cap) ──
        // Prevents runaway GPU rendering, high battery drain, and thermal fan noise
        // even if VSync is disabled by GPU driver control panel
        static auto s_lastFrameTimePoint = std::chrono::high_resolution_clock::now();
        const double targetFrameTime = 1.0 / 60.0; // 60 FPS cap (16.666 ms)

        auto frameStart = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> frameElapsed = frameStart - s_lastFrameTimePoint;
        if (frameElapsed.count() < targetFrameTime) {
            double sleepSeconds = targetFrameTime - frameElapsed.count();
            if (sleepSeconds > 0.002) {
                std::this_thread::sleep_for(std::chrono::microseconds(static_cast<long long>((sleepSeconds - 0.001) * 1e6)));
            }
            while (std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - s_lastFrameTimePoint).count() < targetFrameTime) {
                std::this_thread::yield();
            }
        }
        s_lastFrameTimePoint = std::chrono::high_resolution_clock::now();

        float time = (float)glfwGetTime();
        float dt = time - lastFrameTime;
        lastFrameTime = time;
        if (dt > 0.1f) dt = 0.1f; // clamp delta time for stability

        // ── Interactive Gorur Gari Driving Physics ───────────────────
        float cartThrottle = 0.0f;
        float cartSteerInput = 0.0f;

        // When in Cart Driving Mode, WASD and Arrow Keys control the cart
        if (g_driveCartMode) {
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                cartThrottle += 1.0f;
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                cartThrottle -= 1.0f;
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                cartSteerInput += 1.0f;
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                cartSteerInput -= 1.0f;
        } else if (!g_driveBoatMode) {
            // When not in boat mode, Arrow keys can also control cart
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    cartThrottle += 1.0f;
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  cartThrottle -= 1.0f;
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  cartSteerInput += 1.0f;
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) cartSteerInput -= 1.0f;
        }

        // Acceleration and deceleration
        if (cartThrottle > 0.0f) {
            g_cartSpeed += CART_ACCEL * dt;
            if (g_cartSpeed > CART_MAX_SPEED) g_cartSpeed = CART_MAX_SPEED;
        } else if (cartThrottle < 0.0f) {
            g_cartSpeed -= CART_ACCEL * dt;
            if (g_cartSpeed < -CART_MAX_SPEED * 0.45f) g_cartSpeed = -CART_MAX_SPEED * 0.45f;
        } else {
            // Natural ground friction deceleration
            if (g_cartSpeed > 0.0f) {
                g_cartSpeed -= CART_DECEL * dt;
                if (g_cartSpeed < 0.0f) g_cartSpeed = 0.0f;
            } else if (g_cartSpeed < 0.0f) {
                g_cartSpeed += CART_DECEL * dt;
                if (g_cartSpeed > 0.0f) g_cartSpeed = 0.0f;
            }
        }

        // Steering (turns cart heading)
        if (cartSteerInput != 0.0f && (fabsf(g_cartSpeed) > 0.01f || cartThrottle != 0.0f || g_cartStepDistRemaining > 0.001f)) {
            float turnSign = (g_cartSpeed >= 0.0f) ? 1.0f : -1.0f;
            g_cartHeading += cartSteerInput * CART_TURN_SPEED * dt * turnSign;
        }

        // 1. Process Key 'G' Step Advance (180° Wheel Rotation & Translation Ahead)
        if (g_cartStepDistRemaining > 0.0001f) {
            const float stepRollSpeed = 5.2f; // m/s (~0.43s per 180° half-turn)
            float moveDist = stepRollSpeed * dt;
            if (moveDist > g_cartStepDistRemaining) {
                moveDist = g_cartStepDistRemaining;
            }
            g_cartStepDistRemaining -= moveDist;

            // Rolling without slipping: Delta_s = R * Delta_theta => Delta_theta = Delta_s / R
            float dRot = moveDist / CART_WHEEL_RADIUS;
            g_cartWheelRot += dRot;

            // Translate cart position ahead along its heading
            float rad = radians(g_cartHeading);
            float dirX = sinf(rad);
            float dirZ = cosf(rad);
            g_cartX += dirX * moveDist;
            g_cartZ += dirZ * moveDist;

            // Oxen walking stride animation phase
            g_cartWalkPhase += moveDist * 4.4f;
        }

        // 2. Process Continuous Throttle Movement (W / S)
        if (fabsf(g_cartSpeed) > 0.001f) {
            float rad = radians(g_cartHeading);
            float dirX = sinf(rad);
            float dirZ = cosf(rad);

            g_cartX += dirX * g_cartSpeed * dt;
            g_cartZ += dirZ * g_cartSpeed * dt;

            // Accumulate wheel rotation (R = 0.72m)
            float distMoved = g_cartSpeed * dt;
            g_cartWheelRot += distMoved / CART_WHEEL_RADIUS;

            // Oxen walking stride animation phase
            g_cartWalkPhase += distMoved * 4.4f;
        }

        // Endless Road Wrapping along Grameen Rasta (Z in [-44m, +44m], X in [-46m, +46m])
        if (g_cartZ < -44.0f) g_cartZ =  44.0f;
        if (g_cartZ >  44.0f) g_cartZ = -44.0f;
        if (g_cartX < -46.0f) g_cartX = -46.0f;
        if (g_cartX >  46.0f) g_cartX =  46.0f;

        // ── Interactive Dingi Nouka (Boat) Driving Physics ────────────
        float boatThrottle = 0.0f;
        float boatSteerInput = 0.0f;

        if (g_driveBoatMode) {
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                boatThrottle += 1.0f;
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                boatThrottle -= 1.0f;
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                boatSteerInput += 1.0f;
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                boatSteerInput -= 1.0f;
        }

        // Boat acceleration, water hydrodynamic drag, and rudder steering
        if (boatThrottle > 0.0f) {
            g_boatSpeed += BOAT_ACCEL * dt;
            if (g_boatSpeed > BOAT_MAX_SPEED) g_boatSpeed = BOAT_MAX_SPEED;
        } else if (boatThrottle < 0.0f) {
            g_boatSpeed -= BOAT_ACCEL * dt;
            if (g_boatSpeed < -BOAT_MAX_SPEED * 0.40f) g_boatSpeed = -BOAT_MAX_SPEED * 0.40f;
        } else {
            // Water hydrodynamic drag
            if (g_boatSpeed > 0.0f) {
                g_boatSpeed -= BOAT_DRAG * dt;
                if (g_boatSpeed < 0.0f) g_boatSpeed = 0.0f;
            } else if (g_boatSpeed < 0.0f) {
                g_boatSpeed += BOAT_DRAG * dt;
                if (g_boatSpeed > 0.0f) g_boatSpeed = 0.0f;
            }
        }

        // Rudder turning: responsive with movement or active paddle strokes
        if (boatSteerInput != 0.0f) {
            float steerPower = (fabsf(g_boatSpeed) > 0.15f || g_boatStepDistRemaining > 0.001f) ? 1.0f : (boatThrottle != 0.0f ? 0.70f : 0.35f);
            float turnSign = (g_boatSpeed >= -0.05f) ? 1.0f : -1.0f;
            g_boatYaw -= boatSteerInput * BOAT_TURN_SPEED * steerPower * dt * turnSign;
        }

        // 1. Process Key 'N' Step Advance (Nouka Rowing Stroke & Ahead Translation)
        if (g_boatStepDistRemaining > 0.0001f) {
            const float boatStepSpeed = 3.8f; // m/s (~0.53s per 2-meter rowing glide)
            float moveDist = boatStepSpeed * dt;
            if (moveDist > g_boatStepDistRemaining) {
                moveDist = g_boatStepDistRemaining;
            }
            g_boatStepDistRemaining -= moveDist;

            // Forward displacement along boat heading direction
            float boatDirX = sinf(g_boatYaw);
            float boatDirZ = cosf(g_boatYaw);
            g_boatX += boatDirX * moveDist;
            g_boatZ += boatDirZ * moveDist;

            // Rhythmic rowing oar stroke phase (one complete 2*pi cycle per step)
            float oarRate = (2.0f * PI / BOAT_STEP_DIST) * moveDist;
            g_boatOarPhase += oarRate;

            // Ensure dynamic boat speed for wake ripples & water interaction
            if (g_boatSpeed < boatStepSpeed * 0.80f) {
                g_boatSpeed = boatStepSpeed * 0.80f;
            }
        }

        // 2. Process Continuous Throttle Forward Displacement along heading direction
        if (fabsf(g_boatSpeed) > 0.001f) {
            float boatDirX = sinf(g_boatYaw);
            float boatDirZ = cosf(g_boatYaw);
            g_boatX += boatDirX * g_boatSpeed * dt;
            g_boatZ += boatDirZ * g_boatSpeed * dt;
        }

        // Rowing animation cadence
        if (fabsf(g_boatSpeed) > 0.05f || boatThrottle != 0.0f || g_boatStepDistRemaining > 0.001f) {
            float strokeRate = 2.4f + fabsf(g_boatSpeed) * 1.6f;
            g_boatOarPhase += strokeRate * dt;
        } else {
            g_boatOarPhase += 0.8f * dt;
        }

        // River centerline at current Z: 1.8f * sinf(z * 0.08f + 0.4f) + 0.6f * cosf(z * 0.04f)
        float curRiverCenterline = 1.8f * sinf(g_boatZ * 0.08f + 0.4f) + 0.6f * cosf(g_boatZ * 0.04f);
        float curRiverCenterX = 8.0f + curRiverCenterline;

        // River channel lateral confinement (keeps boat within navigable water)
        float latOffset = g_boatX - curRiverCenterX;
        const float MAX_BOAT_LAT = 3.6f;
        if (latOffset > MAX_BOAT_LAT) {
            g_boatX = curRiverCenterX + MAX_BOAT_LAT;
        } else if (latOffset < -MAX_BOAT_LAT) {
            g_boatX = curRiverCenterX - MAX_BOAT_LAT;
        }

        // Endless River Wrapping: when reaching the end of the river channel, wrap around seamlessly!
        if (g_boatZ < -42.0f) {
            g_boatZ = 42.0f;
            float rCL = 1.8f * sinf(g_boatZ * 0.08f + 0.4f) + 0.6f * cosf(g_boatZ * 0.04f);
            g_boatX = 8.0f + rCL;
        } else if (g_boatZ > 42.0f) {
            g_boatZ = -42.0f;
            float rCL = 1.8f * sinf(g_boatZ * 0.08f + 0.4f) + 0.6f * cosf(g_boatZ * 0.04f);
            g_boatX = 8.0f + rCL;
        }

        // ── Camera Update: Drive Mode Chase Cam vs. Manual WASD Fly-Cam ──
        if (g_driveCartMode) {
            // Third-person smooth chase camera following the cart
            camera.target = vec3(g_cartX, 1.15f, g_cartZ);
            float targetYaw = radians(-g_cartHeading + 180.0f);
            float diff = targetYaw - camera.yaw;
            while (diff > PI)  diff -= 2.0f * PI;
            while (diff < -PI) diff += 2.0f * PI;
            camera.yaw += diff * 4.5f * dt;
            camera.distance = 9.2f;
            camera.pitch = radians(14.0f);
            camera.updatePosition();
        } else if (g_driveBoatMode) {
            // Third-person smooth chase camera following the Dingi Nouka boat
            camera.target = vec3(g_boatX, 0.65f, g_boatZ);
            float targetYaw = g_boatYaw + PI;
            float diff = targetYaw - camera.yaw;
            while (diff > PI)  diff -= 2.0f * PI;
            while (diff < -PI) diff += 2.0f * PI;
            camera.yaw += diff * 4.5f * dt;
            camera.distance = 7.5f;
            camera.pitch = radians(15.0f);
            camera.updatePosition();
        } else {
            // Normal free-flying camera with WASD keys
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

        // 3 Traditional Boats navigation kinematics matching folk artwork
        float rowCyclePhase = animTime * 2.2f;
        float rowingSurge   = (animTime > 0.0f) ? (sinf(rowCyclePhase) * 0.045f) : 0.0f;
        float oarRowAnim    = (animTime > 0.0f) ? (sinf(rowCyclePhase) * radians(16.0f)) : 0.0f;

        // Boat 1 (North / Upstream): Red-Sail Boat with Standing Majhi
        float boat1Z = -9.5f + (animTime > 0.0f ? sinf(animTime * 0.5f) * 0.30f : 0.0f);
        float riverCenterline1 = 1.8f * sinf(boat1Z * 0.08f + 0.4f) + 0.6f * cosf(boat1Z * 0.04f);
        float boat1X = 8.0f + riverCenterline1 - 0.5f;
        float dRiver1X = 1.8f * 0.08f * cosf(boat1Z * 0.08f + 0.4f) - 0.6f * 0.04f * sinf(boat1Z * 0.04f);
        float boat1Yaw = atan2f(dRiver1X, 1.0f) + radians(4.0f);
        float boatBob1 = (animTime > 0.0f) ? (sinf(animTime * 1.5f + 0.4f) * 0.018f) : 0.0f;
        float boatRoll1 = (animTime > 0.0f) ? (sinf(animTime * 1.3f + 0.2f) * radians(1.5f)) : 0.0f;
        float boatPitch1 = (animTime > 0.0f) ? (cosf(animTime * 1.4f) * radians(1.0f)) : 0.0f;

        // Boat 2 (Center): Round Golden-Straw Chhoi Dome Boat with Rowing Majhi (Interactive Controllable Dingi Nouka)
        float boat2X = g_boatX;
        float boat2Z = g_boatZ;
        float boat2Yaw = g_boatYaw;

        float boatBob2;
        float boatRoll2;
        float boatPitch2;

        if (g_driveBoatMode || fabsf(g_boatSpeed) > 0.05f || g_boatStepDistRemaining > 0.001f) {
            float oarStroke = sinf(g_boatOarPhase * 3.14159f);
            oarRowAnim = oarStroke * radians(26.0f);
            boatBob2 = sinf(g_boatOarPhase * 6.28318f) * 0.024f;
            float steerRoll = -boatSteerInput * (fabsf(g_boatSpeed) / BOAT_MAX_SPEED) * radians(4.5f);
            boatRoll2 = steerRoll + sinf(animTime * 2.2f) * radians(1.4f);
            boatPitch2 = (oarStroke * radians(2.5f)) - (g_boatSpeed / BOAT_MAX_SPEED) * radians(2.2f);
        } else {
            float idlePhase = animTime * 2.2f;
            boatBob2 = (animTime > 0.0f) ? (sinf(idlePhase * 2.0f) * 0.022f) : 0.0f;
            boatRoll2 = (animTime > 0.0f) ? (sinf(idlePhase) * radians(1.8f)) : 0.0f;
            boatPitch2 = (animTime > 0.0f) ? (-cosf(idlePhase) * radians(1.5f)) : 0.0f;
        }

        // Boat 3 (South / Downstream): White Sailboat with Tall Bamboo Mast
        // Shifted south to Z ≈ 22.0m to completely clear the fisherman's boat at Z = 8.5m
        float boat3Z = 22.0f + (animTime > 0.0f ? cosf(animTime * 0.45f) * 0.30f : 0.0f);
        float riverCenterline3 = 1.8f * sinf(boat3Z * 0.08f + 0.4f) + 0.6f * cosf(boat3Z * 0.04f);
        float boat3X = 8.0f + riverCenterline3 + 0.2f;
        float dRiver3X = 1.8f * 0.08f * cosf(boat3Z * 0.08f + 0.4f) - 0.6f * 0.04f * sinf(boat3Z * 0.04f);
        float boat3Yaw = atan2f(dRiver3X, 1.0f) - radians(6.0f);
        float boatBob3 = (animTime > 0.0f) ? (sinf(animTime * 1.7f + 2.1f) * 0.018f) : 0.0f;
        float boatRoll3 = (animTime > 0.0f) ? (sinf(animTime * 1.2f + 1.5f) * radians(1.4f)) : 0.0f;
        float boatPitch3 = (animTime > 0.0f) ? (cosf(animTime * 1.3f + 0.8f) * radians(1.1f)) : 0.0f;

        // Backward compatibility aliases for boatX, boatZ, boatCourseYaw
        float boatX = boat2X;
        float boatZ = boat2Z;
        float boatCourseYaw = boat2Yaw;

        // ── Celestial Sun Position in Sky Dome ──────────────────
        const vec3 sunPosition(-14.0f, 26.0f, 16.0f);

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
            // Radiant Daytime (Bright sunlight cast by radiant Sun with Blinn-Phong shading)
            clearColor       = vec3(0.55f, 0.78f, 0.94f); // bright daylight blue sky
            fogCol           = clearColor;
            lightDir         = Sun::getLightDirection(sunPosition); // directional sunlight cast from Sun
            lightColor       = vec3(1.0f, 0.97f, 0.88f);  // warm golden incandescent sunlight
            ambientStrength  = 0.52f;                      // bright daytime ambient
            specularStrength = 0.35f;
            pointIntensity   = 0.0f;                      // daytime: lantern illumination off
            fogDens          = 0.0f;                       // clear daytime visibility
            noLightVal       = 0;                          // full Blinn-Phong lighting
        }
        else if (lightingMode == 2) {
            // Crisp Unlit 3D Facets (Daylight, 0 darkness, 0 shadows, 0 point lights - Lab Milestone Inspection)
            clearColor      = vec3(0.82f, 0.88f, 0.94f); // clean daylight sky
            fogCol          = clearColor;
            lightDir        = Sun::getLightDirection(sunPosition);
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
        vec3 mosqueLanternWorldPos = vec3(-33.0f, 2.28f, -30.92f);
        shader.setVec3("pointLight3Pos",        mosqueLanternWorldPos);
        shader.setVec3("pointLight3Color",      vec3(1.0f, 0.78f, 0.35f)); // warm brass lantern glow
        shader.setFloat("pointLight3Intensity", pointIntensity * lanternMultiplier * mosqueLanternFlicker * 1.15f);

        // Point Light 4: Kitchen Clay Cooking Stove (Matir Chula) Embers / Fire
        float stoveFlicker = (pointIntensity > 0.0f) ? (1.0f + 0.12f * sinf(animTime * 15.3f) * cosf(animTime * 23.7f) + 0.05f * sinf(animTime * 31.1f)) : 1.0f;
        vec3 stoveWorldPos = vec3(-3.27f, 0.25f, -4.19f);
        shader.setVec3("pointLight4Pos",        stoveWorldPos);
        shader.setVec3("pointLight4Color",      vec3(1.0f, 0.46f, 0.10f)); // roaring wood fire & embers
        shader.setFloat("pointLight4Intensity", pointIntensity * stoveFlicker * 1.10f);

        // Point Light 5: River Landing Ghat Mooring Post Lantern
        float ghatFlameFlicker = (pointIntensity > 0.0f && lanternMultiplier > 0.0f) ? (1.0f + 0.05f * sinf(animTime * 8.3f) * cosf(animTime * 13.7f)) : 1.0f;
        vec3 ghatLanternWorldPos = vec3(4.7f, 0.70f, 0.0f);
        shader.setVec3("pointLight5Pos",        ghatLanternWorldPos);
        shader.setVec3("pointLight5Color",      vec3(1.0f, 0.74f, 0.30f)); // warm golden dock lantern
        shader.setFloat("pointLight5Intensity", pointIntensity * lanternMultiplier * ghatFlameFlicker * 1.15f);

        // Point Light 6: Traditional Boat Lantern (hanging under Chhoi canopy of Boat 2)
        float boat2FlameFlicker = (pointIntensity > 0.0f && lanternMultiplier > 0.0f) ? (1.0f + 0.06f * sinf(animTime * 10.1f) * cosf(animTime * 16.3f)) : 1.0f;
        vec3 boat2LanternWorldPos = vec3(boat2X + sinf(boat2Yaw) * 0.35f, 0.10f + boatBob2 + 0.40f, boat2Z + cosf(boat2Yaw) * 0.35f);
        shader.setVec3("pointLight6Pos",        boat2LanternWorldPos);
        shader.setVec3("pointLight6Color",      vec3(1.0f, 0.68f, 0.22f)); // warm amber boat lantern
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
        } else {
            // Radiant daytime Sun (Surjo / সূর্য) casting golden daylight across rural landscape
            mat4 sunM = mat4::identity();
            sunM = translate(sunM, sunPosition);
            // Orient sun disc normal (+Z) to face toward village center (0, 0, 0)
            vec3 toVillage = -sunPosition;
            vec3 forward   = normalize(toVillage);
            vec3 worldUp(0.0f, 1.0f, 0.0f);
            vec3 right     = normalize(cross(worldUp, forward));
            vec3 actualUp  = cross(forward, right);
            mat4 rot       = mat4::identity();
            rot(0, 0) = right.x;    rot(0, 1) = actualUp.x;    rot(0, 2) = forward.x;
            rot(1, 0) = right.y;    rot(1, 1) = actualUp.y;    rot(1, 2) = forward.y;
            rot(2, 0) = right.z;    rot(2, 1) = actualUp.z;    rot(2, 2) = forward.z;
            sunM = sunM * rot;
            Sun::draw(shader, sunM, animTime);
        }

        // ─── 2. TERRAIN, COURTYARDS, ROADS, RIVER & LANDING GHAT ─
        if (showTerrain) {
            mat4 terrainM = mat4::identity();
            Terrain::draw(shader, terrainM);

            mat4 riverM = mat4::identity();
            riverM = translate(riverM, vec3(8.0f, 0.0f, 0.0f));
            River::draw(shader, riverM, animTime);
        }

        // ─── 3. TRADITIONAL VILLAGE MOSQUE (GRAMIN MASJID - NORTH-WEST CORNER SANCTUARY) ───
        mat4 mosqueM = mat4::identity();
        mosqueM = translate(mosqueM, vec3(-33.0f, 0.0f, -34.0f));
        Texture::bind(TEX_BRICK, 0);
        shader.setInt("uTextureType", (int)TEX_BRICK);
        Mosque::draw(shader, mosqueM);

        // ─── 4. VILLAGE HOUSES & HOMESTEADS (RICHLY DISTRIBUTED VILLAGE COMMUNITIES) ─
        Texture::bind(TEX_BAMBOO, 0);
        shader.setInt("uTextureType", (int)TEX_BAMBOO);

        // ═════════════════════════════════════════════════════════════
        // BARI 1: MODDHO BARI (CENTRAL HOMESTEAD & GATHERING UTHAN)
        // ═════════════════════════════════════════════════════════════
        // House 1: Central Chouchala Homestead (4-sloped hip roof, verandah & outdoor clay stove)
        mat4 house1 = mat4::identity();
        house1 = translate(house1, vec3(-8.5f, 0.0f, -3.5f));
        house1 = rotate(house1, radians(4.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house1, HOUSE_CHOUCHALA, true, animTime);

        // House 1B: Outdoor Thatched Kitchen Hut (Ranna Ghor behind House 1)
        mat4 kitchenM = mat4::identity();
        kitchenM = translate(kitchenM, vec3(-12.5f, 0.0f, -11.5f));
        kitchenM = rotate(kitchenM, radians(-8.0f), vec3(0.0f, 1.0f, 0.0f));
        kitchenM = scale(kitchenM, vec3(0.72f, 0.82f, 0.72f));
        House::draw(shader, kitchenM, HOUSE_DOCHALA, false);

        // House 1C: Moddho Bari Guest & Family Cottage (Boithokkhana)
        mat4 house1C = mat4::identity();
        house1C = translate(house1C, vec3(-17.5f, 0.0f, -3.5f));
        house1C = rotate(house1C, radians(-28.0f), vec3(0.0f, 1.0f, 0.0f));
        house1C = scale(house1C, vec3(0.85f, 0.88f, 0.85f));
        House::draw(shader, house1C, HOUSE_DOCHALA, false);

        // Granary 1: Traditional Elevated Paddy Granary (Dhaner Gola / ধানের গোলা)
        mat4 granary1 = mat4::identity();
        granary1 = translate(granary1, vec3(-12.8f, 0.0f, -1.8f));
        House::drawGranary(shader, granary1);

        // Cast-Iron Tubewell (Chapa Kol) with concrete washing apron shifted to clear house verandah
        mat4 tubewellM = mat4::identity();
        tubewellM = translate(tubewellM, vec3(-3.85f, 0.0f, -1.65f));
        tubewellM = rotate(tubewellM, radians(12.0f), vec3(0.0f, 1.0f, 0.0f));
        House::drawTubewell(shader, tubewellM, pumpHandleAngle, isCurrentlyPumping);

        // ═════════════════════════════════════════════════════════════
        // BARI 2: UTTAR BARI (NORTH FARMSTEAD & CATTLE HOMESTEAD)
        // ═════════════════════════════════════════════════════════════
        // House 2: North Farmstead Main House (Dochala 2-sloped curved pitched roof & bamboo verandah)
        mat4 house2 = mat4::identity();
        house2 = translate(house2, vec3(-23.5f, 0.0f, -12.5f));
        house2 = rotate(house2, radians(-18.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house2, HOUSE_DOCHALA, false);

        // House 2B: Farm Worker / Extended Family Dochala Cottage
        mat4 house2B = mat4::identity();
        house2B = translate(house2B, vec3(-19.0f, 0.0f, -20.5f));
        house2B = rotate(house2B, radians(32.0f), vec3(0.0f, 1.0f, 0.0f));
        house2B = scale(house2B, vec3(0.82f, 0.85f, 0.82f));
        House::draw(shader, house2B, HOUSE_DOCHALA, false);

        // Thatched Cow Shed (Gowal Ghor in Uttar Bari farmstead - shifted west to clear House 2)
        mat4 cowShedM = mat4::identity();
        cowShedM = translate(cowShedM, vec3(-30.5f, 0.0f, -10.5f));
        cowShedM = rotate(cowShedM, radians(8.0f), vec3(0.0f, 1.0f, 0.0f));
        House::drawCowShed(shader, cowShedM);

        // Standing Deshi Cow resting in the open Uttar Bari farmyard (shifted clear of house wall)
        mat4 cowStandingM = mat4::identity();
        cowStandingM = translate(cowStandingM, vec3(-28.2f, 0.0f, -8.0f));
        cowStandingM = rotate(cowStandingM, radians(65.0f), vec3(0.0f, 1.0f, 0.0f));
        House::drawCow(shader, cowStandingM, false);

        // Rice Straw Stacks (Khorer Paloi / খড়ের পালা with center bamboo pole)
        House::drawStrawStack(shader, mat4::identity(), vec3(-23.0f, 0.0f, -18.5f), 1.15f); // Uttar Bari main stack
        House::drawStrawStack(shader, mat4::identity(), vec3(-25.8f, 0.0f, -18.5f), 0.95f); // Uttar Bari secondary stack

        // Chicken Coop on stilts (Murgir Khopa)
        mat4 coopM = mat4::identity();
        coopM = translate(coopM, vec3(-15.5f, 0.0f, -16.5f));
        coopM = rotate(coopM, radians(32.0f), vec3(0.0f, 1.0f, 0.0f));
        House::drawChickenCoop(shader, coopM, isNight);

        // ═════════════════════════════════════════════════════════════
        // BARI 3: DOKKHIN BARI (SOUTH AGRICULTURAL HOMESTEAD & FIELDS)
        // ═════════════════════════════════════════════════════════════
        // House 3: Dokkhin Bari South Homestead (Chouchala House facing path & paddy fields)
        mat4 house3 = mat4::identity();
        house3 = translate(house3, vec3(-10.5f, 0.0f, 16.5f));
        house3 = rotate(house3, radians(172.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house3, HOUSE_CHOUCHALA, false);

        // House 3B: Farmer's Tool & Storage Dochala Outbuilding
        mat4 house3B = mat4::identity();
        house3B = translate(house3B, vec3(-18.5f, 0.0f, 16.5f));
        house3B = rotate(house3B, radians(105.0f), vec3(0.0f, 1.0f, 0.0f));
        house3B = scale(house3B, vec3(0.85f, 0.88f, 0.85f));
        House::draw(shader, house3B, HOUSE_DOCHALA, false);

        // House 3C: Southern Boundary Farmer's Cottage
        mat4 house3C = mat4::identity();
        house3C = translate(house3C, vec3(-8.5f, 0.0f, 33.5f));
        house3C = rotate(house3C, radians(-165.0f), vec3(0.0f, 1.0f, 0.0f));
        house3C = scale(house3C, vec3(0.80f, 0.85f, 0.80f));
        House::draw(shader, house3C, HOUSE_DOCHALA, false);

        // Vegetable Trellis (Lau / Kumra Macha / সবজির মাচা with hanging gourds)
        mat4 trellis1 = mat4::identity();
        trellis1 = translate(trellis1, vec3(-14.5f, 0.0f, 18.0f));
        trellis1 = rotate(trellis1, radians(5.0f), vec3(0.0f, 1.0f, 0.0f));
        House::drawVegetableTrellis(shader, trellis1);

        // Straw Stack in Dokkhin Bari
        House::drawStrawStack(shader, mat4::identity(), vec3(-14.5f, 0.0f, 14.5f), 1.10f);

        // ═════════════════════════════════════════════════════════════
        // BARI 4: POSHCHIM BARI (WEST MEADOW HOMESTEADS & GRANARY)
        // ═════════════════════════════════════════════════════════════
        // House 5: Paschim Bari West Homestead (Dochala style)
        mat4 house5 = mat4::identity();
        house5 = translate(house5, vec3(-25.5f, 0.0f, 5.0f));
        house5 = rotate(house5, radians(42.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house5, HOUSE_DOCHALA, false);

        // House 5B: Weaver / Village Artisan Homestead (Chouchala House)
        mat4 house5B = mat4::identity();
        house5B = translate(house5B, vec3(-31.5f, 0.0f, -3.5f));
        house5B = rotate(house5B, radians(15.0f), vec3(0.0f, 1.0f, 0.0f));
        house5B = scale(house5B, vec3(0.92f, 0.92f, 0.92f));
        House::draw(shader, house5B, HOUSE_CHOUCHALA, false);

        // House 5C: West Shaded Garden Cottage
        mat4 house5C = mat4::identity();
        house5C = translate(house5C, vec3(-26.5f, 0.0f, 13.0f));
        house5C = rotate(house5C, radians(-45.0f), vec3(0.0f, 1.0f, 0.0f));
        house5C = scale(house5C, vec3(0.85f, 0.88f, 0.85f));
        House::draw(shader, house5C, HOUSE_DOCHALA, false);

        // Granary 2: Second Elevated Paddy Granary (Dhaner Gola) in Western Homestead
        mat4 granary2 = mat4::identity();
        granary2 = translate(granary2, vec3(-28.0f, 0.0f, 9.0f));
        House::drawGranary(shader, granary2);

        // Straw Stack at Paschim Bari
        House::drawStrawStack(shader, mat4::identity(), vec3(-21.5f, 0.0f, 1.5f), 1.05f);

        // ═════════════════════════════════════════════════════════════
        // BARI 5: NODI-PAR / PURBOPARA (RIVERSIDE FISHERMAN & BOATMEN)
        // ═════════════════════════════════════════════════════════════
        // House 4: Riverside Fisherman Cottage (Dochala situated on solid ground beside northern riverbank path)
        mat4 house4 = mat4::identity();
        house4 = translate(house4, vec3(-9.0f, 0.0f, -21.5f));
        house4 = rotate(house4, radians(82.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house4, HOUSE_DOCHALA, false);

        // House 4B: Riverside Storage & Net Hut (Footbridge landing on East Bank)
        mat4 house4B = mat4::identity();
        house4B = translate(house4B, vec3(12.5f, 0.0f, -21.5f));
        house4B = rotate(house4B, radians(95.0f), vec3(0.0f, 1.0f, 0.0f));
        house4B = scale(house4B, vec3(0.72f, 0.78f, 0.72f));
        House::draw(shader, house4B, HOUSE_DOCHALA, false);

        // House 7: South Riverside Boatman's Cottage
        mat4 house7 = mat4::identity();
        house7 = translate(house7, vec3(-8.5f, 0.0f, 7.0f));
        house7 = rotate(house7, radians(65.0f), vec3(0.0f, 1.0f, 0.0f));
        house7 = scale(house7, vec3(0.88f, 0.90f, 0.88f));
        House::draw(shader, house7, HOUSE_DOCHALA, false);

        // Fishing Net Drying Racks (Jal Macha / মাছের জাল শুকানোর মাচা)
        mat4 netRack1 = mat4::identity();
        netRack1 = translate(netRack1, vec3(-6.2f, 0.0f, -19.5f));
        netRack1 = rotate(netRack1, radians(-15.0f), vec3(0.0f, 1.0f, 0.0f));
        House::drawNetRack(shader, netRack1);

        mat4 netRack2 = mat4::identity();
        netRack2 = translate(netRack2, vec3(-6.2f, 0.0f, 6.0f));
        netRack2 = rotate(netRack2, radians(18.0f), vec3(0.0f, 1.0f, 0.0f));
        House::drawNetRack(shader, netRack2);

        // Riverbank Duck House (Hash-er Ghor) on bamboo stilts
        mat4 duckHouseM = mat4::identity();
        duckHouseM = translate(duckHouseM, vec3(-5.2f, 0.0f, -1.5f));
        duckHouseM = rotate(duckHouseM, radians(-22.0f), vec3(0.0f, 1.0f, 0.0f));
        House::drawDuckHouse(shader, duckHouseM, isNight);

        // ═════════════════════════════════════════════════════════════
        // BARI 6: TANTI PARA (FAR WEST ARTISAN & WEAVER COLONY)
        // ═════════════════════════════════════════════════════════════
        // House 6A: Artisan Master Weaver Homestead (Dochala style)
        mat4 house6A = mat4::identity();
        house6A = translate(house6A, vec3(-36.5f, 0.0f, 6.5f));
        house6A = rotate(house6A, radians(25.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house6A, HOUSE_DOCHALA, false);

        // House 6B: Handloom & Storage Cottage
        mat4 house6B = mat4::identity();
        house6B = translate(house6B, vec3(-40.0f, 0.0f, 15.5f));
        house6B = rotate(house6B, radians(-30.0f), vec3(0.0f, 1.0f, 0.0f));
        house6B = scale(house6B, vec3(0.85f, 0.88f, 0.85f));
        House::draw(shader, house6B, HOUSE_DOCHALA, false);

        // House 6C: West Elder Homestead (Chouchala House)
        mat4 house6C = mat4::identity();
        house6C = translate(house6C, vec3(-39.5f, 0.0f, -4.5f));
        house6C = rotate(house6C, radians(78.0f), vec3(0.0f, 1.0f, 0.0f));
        house6C = scale(house6C, vec3(0.92f, 0.92f, 0.92f));
        House::draw(shader, house6C, HOUSE_CHOUCHALA, false);

        // House 6D: Village Pottery & Crafts Workshop
        mat4 house6D = mat4::identity();
        house6D = translate(house6D, vec3(-41.0f, 0.0f, -14.5f));
        house6D = rotate(house6D, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
        house6D = scale(house6D, vec3(0.80f, 0.85f, 0.80f));
        House::draw(shader, house6D, HOUSE_DOCHALA, false);

        // Straw Stack at Tanti Para
        House::drawStrawStack(shader, mat4::identity(), vec3(-36.5f, 0.0f, 1.0f), 1.05f);

        // ═════════════════════════════════════════════════════════════
        // BARI 7: UTTAR-PASCHIM BARI (NORTH-WEST MEADOW FARMSTEAD)
        // ═════════════════════════════════════════════════════════════
        // House 7A: North-West Farmhouse (Dochala style)
        mat4 house7A = mat4::identity();
        house7A = translate(house7A, vec3(-28.0f, 0.0f, -28.0f));
        house7A = rotate(house7A, radians(-15.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house7A, HOUSE_DOCHALA, false);

        // House 7B: Family Dochala Cottage
        mat4 house7B = mat4::identity();
        house7B = translate(house7B, vec3(-19.5f, 0.0f, -35.5f));
        house7B = rotate(house7B, radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
        house7B = scale(house7B, vec3(0.85f, 0.88f, 0.85f));
        House::draw(shader, house7B, HOUSE_DOCHALA, false);

        // House 7C: North Boundary Hay & Storehouse
        mat4 house7C = mat4::identity();
        house7C = translate(house7C, vec3(-31.5f, 0.0f, -20.5f));
        house7C = rotate(house7C, radians(75.0f), vec3(0.0f, 1.0f, 0.0f));
        house7C = scale(house7C, vec3(0.80f, 0.82f, 0.80f));
        House::draw(shader, house7C, HOUSE_DOCHALA, false);

        // Straw Stack at Uttar-Paschim Bari
        House::drawStrawStack(shader, mat4::identity(), vec3(-24.5f, 0.0f, -23.5f), 1.10f);

        // ═════════════════════════════════════════════════════════════
        // BARI 8: DOKKHIN-PASCHIM BARI (SOUTH AGRICULTURAL SETTLEMENT)
        // ═════════════════════════════════════════════════════════════
        // House 8A: Southern Agricultural Chouchala Homestead
        mat4 house8A = mat4::identity();
        house8A = translate(house8A, vec3(-26.5f, 0.0f, 34.5f));
        house8A = rotate(house8A, radians(165.0f), vec3(0.0f, 1.0f, 0.0f));
        house8A = scale(house8A, vec3(0.95f, 0.95f, 0.95f));
        House::draw(shader, house8A, HOUSE_CHOUCHALA, false);

        // House 8B: Field Worker Dochala Cottage
        mat4 house8B = mat4::identity();
        house8B = translate(house8B, vec3(-36.5f, 0.0f, 32.5f));
        house8B = rotate(house8B, radians(-40.0f), vec3(0.0f, 1.0f, 0.0f));
        house8B = scale(house8B, vec3(0.85f, 0.88f, 0.85f));
        House::draw(shader, house8B, HOUSE_DOCHALA, false);

        // House 8C: Southern Field Watch Hut (Khet-er Ghor)
        mat4 house8C = mat4::identity();
        house8C = translate(house8C, vec3(-17.5f, 0.0f, 35.5f));
        house8C = rotate(house8C, radians(12.0f), vec3(0.0f, 1.0f, 0.0f));
        house8C = scale(house8C, vec3(0.78f, 0.82f, 0.78f));
        House::draw(shader, house8C, HOUSE_DOCHALA, false);

        // Straw Stack & Vegetable Trellis in Southern settlement
        House::drawStrawStack(shader, mat4::identity(), vec3(-21.5f, 0.0f, 35.5f), 1.08f);
        mat4 trellis2 = mat4::identity();
        trellis2 = translate(trellis2, vec3(-31.5f, 0.0f, 33.5f));
        trellis2 = rotate(trellis2, radians(-10.0f), vec3(0.0f, 1.0f, 0.0f));
        House::drawVegetableTrellis(shader, trellis2);

        // ═════════════════════════════════════════════════════════════
        // BARI 9: UTTAR-NODI BARI (MOSQUE & NORTH RIVERSIDE COTTAGES)
        // ═════════════════════════════════════════════════════════════
        // House 9A: Imam & Mosque Caretaker Cottage
        mat4 house9A = mat4::identity();
        house9A = translate(house9A, vec3(-13.0f, 0.0f, -30.0f));
        house9A = rotate(house9A, radians(12.0f), vec3(0.0f, 1.0f, 0.0f));
        house9A = scale(house9A, vec3(0.88f, 0.90f, 0.88f));
        House::draw(shader, house9A, HOUSE_DOCHALA, false);

        // House 9B: North Riverside Fisherman Hut
        mat4 house9B = mat4::identity();
        house9B = translate(house9B, vec3(-10.5f, 0.0f, -38.5f));
        house9B = rotate(house9B, radians(85.0f), vec3(0.0f, 1.0f, 0.0f));
        house9B = scale(house9B, vec3(0.80f, 0.85f, 0.80f));
        House::draw(shader, house9B, HOUSE_DOCHALA, false);

        // ═════════════════════════════════════════════════════════════
        // BARI 10: PURBOPARA (EAST VILLAGE - TRANSLATED FAR INTO EASTERN MEADOW)
        // ═════════════════════════════════════════════════════════════
        // House E1: Central Purbopara Homestead (Chouchala 4-sloped terracotta hip roof)
        mat4 houseE1 = mat4::identity();
        houseE1 = translate(houseE1, vec3(33.5f, 0.0f, -2.5f));
        houseE1 = rotate(houseE1, radians(-85.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, houseE1, HOUSE_CHOUCHALA, false);

        // House E2: East Village Dochala Cottage
        mat4 houseE2 = mat4::identity();
        houseE2 = translate(houseE2, vec3(30.5f, 0.0f, 8.5f));
        houseE2 = rotate(houseE2, radians(-95.0f), vec3(0.0f, 1.0f, 0.0f));
        houseE2 = scale(houseE2, vec3(0.88f, 0.90f, 0.88f));
        House::draw(shader, houseE2, HOUSE_DOCHALA, false);

        // House E3: North Purbopara Homestead (Chouchala House)
        mat4 houseE3 = mat4::identity();
        houseE3 = translate(houseE3, vec3(32.5f, 0.0f, -19.5f));
        houseE3 = rotate(houseE3, radians(-75.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, houseE3, HOUSE_CHOUCHALA, false);

        // House E4: North East Farmer Cottage (Dochala)
        mat4 houseE4 = mat4::identity();
        houseE4 = translate(houseE4, vec3(40.5f, 0.0f, -12.5f));
        houseE4 = rotate(houseE4, radians(15.0f), vec3(0.0f, 1.0f, 0.0f));
        houseE4 = scale(houseE4, vec3(0.85f, 0.88f, 0.85f));
        House::draw(shader, houseE4, HOUSE_DOCHALA, false);

        // House E5: South Purbopara Homestead (Dochala)
        mat4 houseE5 = mat4::identity();
        houseE5 = translate(houseE5, vec3(35.5f, 0.0f, 15.5f));
        houseE5 = rotate(houseE5, radians(-110.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, houseE5, HOUSE_DOCHALA, false);

        // House E6: South East Orchard Cottage (Dochala) shifted onto solid courtyard ground
        mat4 houseE6 = mat4::identity();
        houseE6 = translate(houseE6, vec3(27.5f, 0.0f, 23.5f));
        houseE6 = rotate(houseE6, radians(-85.0f), vec3(0.0f, 1.0f, 0.0f));
        houseE6 = scale(houseE6, vec3(0.85f, 0.88f, 0.85f));
        House::draw(shader, houseE6, HOUSE_DOCHALA, false);

        // House E7: East Boundary Homestead (Chouchala)
        mat4 houseE7 = mat4::identity();
        houseE7 = translate(houseE7, vec3(42.5f, 0.0f, -3.5f));
        houseE7 = rotate(houseE7, radians(15.0f), vec3(0.0f, 1.0f, 0.0f));
        houseE7 = scale(houseE7, vec3(0.92f, 0.92f, 0.92f));
        House::draw(shader, houseE7, HOUSE_CHOUCHALA, false);

        // House E8: Far North Purbopara Cottage (Dochala)
        mat4 houseE8 = mat4::identity();
        houseE8 = translate(houseE8, vec3(34.5f, 0.0f, -31.5f));
        houseE8 = rotate(houseE8, radians(-80.0f), vec3(0.0f, 1.0f, 0.0f));
        houseE8 = scale(houseE8, vec3(0.85f, 0.88f, 0.85f));
        House::draw(shader, houseE8, HOUSE_DOCHALA, false);

        // Purbopara Rice Straw Stacks (Khorer Paloi moved far into the eastern village)
        House::drawStrawStack(shader, mat4::identity(), vec3(37.5f, 0.0f, 3.5f), 1.15f);
        House::drawStrawStack(shader, mat4::identity(), vec3(36.5f, 0.0f, -25.5f), 1.05f);
        House::drawStrawStack(shader, mat4::identity(), vec3(39.5f, 0.0f, 12.0f), 1.10f);

        // Traditional Rural Bangladeshi Bullock Cart (Gorur Gari / গরুর গাড়ি) Driven on Grameen Rasta
        mat4 bullockCartM = mat4::identity();
        bullockCartM = translate(bullockCartM, vec3(g_cartX, 0.012f, g_cartZ));
        bullockCartM = rotate(bullockCartM, radians(g_cartHeading), vec3(0.0f, 1.0f, 0.0f));
        House::drawBullockCart(shader, bullockCartM, g_cartWheelRot, g_cartWalkPhase);

        // Terracotta Clay Water Pitchers (Matir Kolshi) distributed across homesteads
        House::drawKolshi(shader, mat4::identity(), vec3(-3.35f, 0.0f, -1.35f), 0.95f);  // Tubewell apron
        House::drawKolshi(shader, mat4::identity(), vec3(-17.5f, 0.0f, -12.5f), 0.90f); // Dochala House 2 entrance
        House::drawKolshi(shader, mat4::identity(), vec3(-7.0f, 0.0f, -9.6f), 0.88f);  // Kitchen Hut
        House::drawKolshi(shader, mat4::identity(), vec3(4.8f, 0.12f, 1.8f), 0.92f);   // River Landing Ghat
        House::drawKolshi(shader, mat4::identity(), vec3(-7.8f, 0.0f, 16.5f), 0.85f);  // Dokkhin Bari House 3
        House::drawKolshi(shader, mat4::identity(), vec3(-21.5f, 0.0f, 3.2f), 0.88f);  // Paschim Bari House 5
        House::drawKolshi(shader, mat4::identity(), vec3(1.8f, 0.0f, 7.8f), 0.82f);    // Riverside House 7

        // Parametric Cubic Bézier Terracotta Surahi Vases
        mat4 vaseM1 = mat4::identity();
        vaseM1 = translate(vaseM1, vec3(-20.2f, 0.08f, 5.2f)); // Paschim Bari
        vaseM1 = scale(vaseM1, vec3(0.72f, 0.72f, 0.72f));
        Texture::bind(TEX_BRICK, 0);
        shader.setInt("uTextureType", (int)TEX_BRICK);
        CurvedObject::drawBezierVase(shader, vaseM1, vec3(0.76f, 0.42f, 0.24f));

        mat4 vaseM2 = mat4::identity();
        vaseM2 = translate(vaseM2, vec3(-7.2f, 0.08f, 16.0f)); // Dokkhin Bari
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
        // All coconut trees removed from river channel and placed on authentic dry village ground.
        mat4 palm3 = mat4::identity();
        palm3 = translate(palm3, vec3(-9.5f, 0.0f, -13.0f));
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

        mat4 palm8 = mat4::identity();
        palm8 = translate(palm8, vec3(-27.5f, 0.0f, 6.5f));
        palm8 = rotate(palm8, radians(-25.0f), vec3(0.0f, 1.0f, 0.0f));
        palm8 = rotate(palm8, palmSway2, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, palm8, TREE_PALM);

        // Full Plane Coconut Palms (Distributed across Far-West, North, South, and East Bank Village)
        struct TreePlacement { float x, z, rotDeg; int swayAxis; }; // swayAxis: 0=sway1 Z, 1=sway2 X
        static const TreePlacement fullPalms[] = {
            // East Bank Riverfront & Purbopara
            { 16.5f, -12.0f, -15.0f, 0 },
            { 17.2f,   2.0f,  20.0f, 1 },
            { 16.8f,  14.0f, -25.0f, 0 },
            { 17.5f, -28.0f,  30.0f, 1 },
            { 16.0f,  32.0f, -10.0f, 0 },
            // Roadside Palms Lining the Nearly Straight Grameen Rasta (X = 23.8m)
            { 23.8f, -36.0f,  15.0f, 0 },
            { 23.8f, -14.0f, -25.0f, 1 },
            { 23.8f,   8.0f,  35.0f, 0 },
            { 23.8f,  24.0f, -15.0f, 1 },
            { 23.8f,  38.0f,  20.0f, 0 },
            { 36.5f, -10.5f,  45.0f, 1 },
            { 41.5f,   3.0f, -35.0f, 0 },
            { 37.0f,  16.5f,  15.0f, 1 },
            { 41.0f, -22.0f, -20.0f, 0 },
            { 40.5f,  30.0f,  25.0f, 1 },
            // Far-West Artisan Colony (Tanti Para)
            { -39.0f,  10.0f, -18.0f, 0 },
            { -35.0f,  -1.0f,  32.0f, 1 },
            { -31.0f,  15.0f, -22.0f, 0 },
            // North-West Farmstead & Mosque Precinct
            { -38.5f, -38.0f,  15.0f, 1 },
            { -27.0f, -30.0f, -25.0f, 0 },
            { -22.0f, -36.0f, -30.0f, 0 },
            // South Agricultural Expanse (Shifted out of rice field onto solid southern ground)
            { -26.0f,  32.5f,  40.0f, 1 },
            { -16.0f,  38.0f, -15.0f, 0 },
            { -30.0f,  39.0f,  20.0f, 1 }
        };
        for (const auto& tp : fullPalms) {
            mat4 m = mat4::identity();
            m = translate(m, vec3(tp.x, 0.0f, tp.z));
            m = rotate(m, radians(tp.rotDeg), vec3(0.0f, 1.0f, 0.0f));
            if (tp.swayAxis == 0) m = rotate(m, palmSway1, vec3(0.0f, 0.0f, 1.0f));
            else                  m = rotate(m, palmSway2, vec3(1.0f, 0.0f, 0.0f));
            Tree::draw(shader, m, TREE_PALM);
        }

        // Banana Trees (Kola Gach with paddle leaves, fruit bunch & purple heart)
        mat4 banana1 = mat4::identity();
        banana1 = translate(banana1, vec3(-17.5f, 0.0f, -11.5f)); // Uttar Bari yard
        banana1 = rotate(banana1, bananaSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, banana1, TREE_BANANA);

        mat4 banana2 = mat4::identity();
        banana2 = translate(banana2, vec3(-22.0f, 0.0f, 7.5f)); // Behind Paschim Bari
        banana2 = rotate(banana2, radians(55.0f), vec3(0.0f, 1.0f, 0.0f));
        banana2 = rotate(banana2, -bananaSway, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, banana2, TREE_BANANA);

        mat4 banana3 = mat4::identity();
        banana3 = translate(banana3, vec3(-11.5f, 0.0f, -14.8f)); // Behind Kitchen Hut on solid ground
        banana3 = rotate(banana3, radians(-25.0f), vec3(0.0f, 1.0f, 0.0f));
        banana3 = rotate(banana3, bananaSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, banana3, TREE_BANANA);

        mat4 banana4 = mat4::identity();
        banana4 = translate(banana4, vec3(-7.5f, 0.0f, 16.5f)); // Near South Homestead
        banana4 = rotate(banana4, radians(80.0f), vec3(0.0f, 1.0f, 0.0f));
        banana4 = rotate(banana4, -bananaSway, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, banana4, TREE_BANANA);

        mat4 banana5 = mat4::identity();
        banana5 = translate(banana5, vec3(-8.8f, 0.0f, -18.5f)); // Shading Riverside Cottage garden on solid west terrace
        banana5 = rotate(banana5, radians(-60.0f), vec3(0.0f, 1.0f, 0.0f));
        banana5 = rotate(banana5, bananaSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, banana5, TREE_BANANA);

        mat4 banana6 = mat4::identity();
        banana6 = translate(banana6, vec3(-15.5f, 0.0f, -5.5f)); // Beside House 1C
        banana6 = rotate(banana6, radians(40.0f), vec3(0.0f, 1.0f, 0.0f));
        banana6 = rotate(banana6, -bananaSway, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, banana6, TREE_BANANA);

        mat4 banana7 = mat4::identity();
        banana7 = translate(banana7, vec3(-15.5f, 0.0f, 13.5f)); // Beside House 3B
        banana7 = rotate(banana7, radians(-35.0f), vec3(0.0f, 1.0f, 0.0f));
        banana7 = rotate(banana7, bananaSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, banana7, TREE_BANANA);

        // Foreground Banana Tree (framing the courtyard garden on solid ground)
        mat4 bananaFg = mat4::identity();
        bananaFg = translate(bananaFg, vec3(-6.5f, 0.0f, 3.5f));
        bananaFg = rotate(bananaFg, radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
        bananaFg = rotate(bananaFg, bananaSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, bananaFg, TREE_BANANA);

        // Full Plane Banana Trees
        struct BananaPlacement { float x, z, rotDeg; bool invertSway; };
        static const BananaPlacement fullBananas[] = {
            // Far West Tanti Para
            { -38.5f,   8.0f, -25.0f, false },
            { -34.0f,  14.5f,  60.0f, true  },
            { -31.0f,   1.5f, -40.0f, false },
            // North-West Farmstead & Mosque Flank
            { -26.0f, -36.0f,  35.0f, true  },
            { -23.0f, -32.0f, -20.0f, false },
            // South-West Farmstead
            { -26.0f,  35.5f,  75.0f, true  },
            { -20.0f,  32.0f, -30.0f, false },
            // Mosque Hamlet
            {  -8.0f, -31.5f,  45.0f, true  },
            // Purbopara (East Village - Translated Far into Eastern Meadow)
            {  36.5f,  -4.0f, -35.0f, false },
            {  41.0f,  -1.0f,  50.0f, true  },
            {  37.5f,  11.0f, -15.0f, false },
            {  41.5f,   7.0f,  40.0f, true  },
            {  37.0f, -26.5f, -45.0f, false },
            {  41.0f,  19.5f,  30.0f, true  }
        };
        for (const auto& bp : fullBananas) {
            mat4 m = mat4::identity();
            m = translate(m, vec3(bp.x, 0.0f, bp.z));
            m = rotate(m, radians(bp.rotDeg), vec3(0.0f, 1.0f, 0.0f));
            if (bp.invertSway) m = rotate(m, -bananaSway, vec3(1.0f, 0.0f, 0.0f));
            else               m = rotate(m,  bananaSway, vec3(0.0f, 0.0f, 1.0f));
            Tree::draw(shader, m, TREE_BANANA);
        }

        // Branching Banyan / Mango Trees (Bot / Aam Gach with spreading leafy canopy)
        mat4 mangoTree1 = mat4::identity();
        mangoTree1 = translate(mangoTree1, vec3(-23.5f, 0.0f, -16.5f)); // Sheltering Uttar Bari
        mangoTree1 = rotate(mangoTree1, treeSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, mangoTree1, TREE_GENERAL);

        mat4 mangoTree2 = mat4::identity();
        mangoTree2 = translate(mangoTree2, vec3(-24.5f, 0.0f, 1.5f)); // Shading Paschim Bari
        mangoTree2 = rotate(mangoTree2, radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
        mangoTree2 = rotate(mangoTree2, -treeSway, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, mangoTree2, TREE_GENERAL);

        mat4 mangoTree3 = mat4::identity();
        mangoTree3 = translate(mangoTree3, vec3(-8.5f, 0.0f, 9.0f)); // Shading western terrace on solid ground
        mangoTree3 = rotate(mangoTree3, radians(-45.0f), vec3(0.0f, 1.0f, 0.0f));
        mangoTree3 = rotate(mangoTree3, treeSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, mangoTree3, TREE_GENERAL);

        mat4 mangoTree4 = mat4::identity();
        mangoTree4 = translate(mangoTree4, vec3(-7.5f, 0.0f, 22.5f)); // Shading Dokkhin Bari
        mangoTree4 = rotate(mangoTree4, radians(60.0f), vec3(0.0f, 1.0f, 0.0f));
        mangoTree4 = rotate(mangoTree4, -treeSway, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, mangoTree4, TREE_GENERAL);

        // Full Plane Spreading Mango & Banyan Trees
        struct MangoPlacement { float x, z, rotDeg; bool invertSway; };
        static const MangoPlacement fullMangoes[] = {
            // Far West Tanti Para Gathering Tree
            { -39.5f,   1.5f, -30.0f, false },
            // North-West Orchard Corner (Sheltering Bari 7)
            { -20.0f, -21.0f,  45.0f, true  },
            // Dokkhin-Paschim Field Grove (Shifted out of rice field onto solid western meadow bank ground)
            { -33.5f,  25.5f, -50.0f, false },
            // Far South-West Boundary
            { -27.5f,  44.0f,  60.0f, true  },
            // Mosque North Rear Shade Tree
            { -33.0f, -41.0f, -15.0f, false },
            // East Village Center Shade Tree
            {  36.5f,   0.0f,  25.0f, true  },
            // East Village Northern Grove
            {  39.0f, -16.0f, -40.0f, false },
            // East Village Southern Meadow
            {  38.0f,  32.0f,  35.0f, true  }
        };
        for (const auto& mp : fullMangoes) {
            mat4 m = mat4::identity();
            m = translate(m, vec3(mp.x, 0.0f, mp.z));
            m = rotate(m, radians(mp.rotDeg), vec3(0.0f, 1.0f, 0.0f));
            if (mp.invertSway) m = rotate(m, -treeSway, vec3(1.0f, 0.0f, 0.0f));
            else              m = rotate(m,  treeSway, vec3(0.0f, 0.0f, 1.0f));
            Tree::draw(shader, m, TREE_GENERAL);
        }

        // Bamboo Groves (Bansher Jhar with segmented culms & annular joints)
        mat4 bambooGrove1 = mat4::identity();
        bambooGrove1 = translate(bambooGrove1, vec3(-18.0f, 0.0f, -8.0f));
        bambooGrove1 = rotate(bambooGrove1, radians(30.0f), vec3(0.0f, 1.0f, 0.0f));
        bambooGrove1 = rotate(bambooGrove1, bambooSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, bambooGrove1, TREE_BAMBOO);

        mat4 bambooGrove2 = mat4::identity();
        bambooGrove2 = translate(bambooGrove2, vec3(-12.5f, 0.0f, -20.5f));
        bambooGrove2 = rotate(bambooGrove2, radians(-15.0f), vec3(0.0f, 1.0f, 0.0f));
        bambooGrove2 = rotate(bambooGrove2, -bambooSway, vec3(1.0f, 0.0f, 0.0f));
        Tree::draw(shader, bambooGrove2, TREE_BAMBOO);

        mat4 bambooGrove3 = mat4::identity();
        bambooGrove3 = translate(bambooGrove3, vec3(-21.5f, 0.0f, 18.0f));
        bambooGrove3 = rotate(bambooGrove3, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
        bambooGrove3 = rotate(bambooGrove3, bambooSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, bambooGrove3, TREE_BAMBOO);

        mat4 bambooGrove4 = mat4::identity();
        bambooGrove4 = translate(bambooGrove4, vec3(-8.5f, 0.0f, -24.5f)); // Flanking House 4 on the solid river terrace
        bambooGrove4 = rotate(bambooGrove4, radians(-30.0f), vec3(0.0f, 1.0f, 0.0f));
        bambooGrove4 = rotate(bambooGrove4, bambooSway, vec3(0.0f, 0.0f, 1.0f));
        Tree::draw(shader, bambooGrove4, TREE_BAMBOO);

        // Full Plane Dense Bamboo Groves (Borders & Windbreaks)
        struct BambooPlacement { float x, z, rotDeg; bool invertSway; };
        static const BambooPlacement fullBamboos[] = {
            // Far West Border Windbreaks
            { -42.0f,   5.0f,  25.0f, false },
            { -41.0f,  -8.0f, -20.0f, true  },
            // North-West Border Windbreak
            { -40.0f, -36.0f,  35.0f, false },
            { -34.0f, -42.0f, -15.0f, true  },
            // South-West Border
            { -32.0f,  34.0f,  45.0f, false },
            { -25.0f,  42.0f, -30.0f, true  },
            // Far North Riverbank (on solid ground away from water)
            {  -9.5f, -38.0f,  20.0f, false },
            {  15.0f, -41.0f, -25.0f, true  },
            // East Bank Windbreaks (Purbopara Outer Boundary)
            {  44.0f,  -8.0f,  30.0f, false },
            {  45.0f,  10.0f, -20.0f, true  },
            {  43.5f, -24.0f,  40.0f, false },
            {  44.5f,  28.0f, -15.0f, true  }
        };
        for (const auto& bp : fullBamboos) {
            mat4 m = mat4::identity();
            m = translate(m, vec3(bp.x, 0.0f, bp.z));
            m = rotate(m, radians(bp.rotDeg), vec3(0.0f, 1.0f, 0.0f));
            if (bp.invertSway) m = rotate(m, -bambooSway, vec3(1.0f, 0.0f, 0.0f));
            else              m = rotate(m,  bambooSway, vec3(0.0f, 0.0f, 1.0f));
            Tree::draw(shader, m, TREE_BAMBOO);
        }

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
 
        // Person 6: Devout Village Elder / Worshipper walking serenely around the Mosque (Masjid Prangon)
        // A reverent elder (Musalli / Namazi) completing a calm perimeter walk around the mosque.
        // Dressed in clean white Punjabi, deep green checkered Lungi, white cotton prayer cap (Tupi),
        // and red cotton Gamcha draped over one shoulder.
        {
            struct PathPoint { float x, z; };
            static const PathPoint s_mosqueLoop[12] = {
                { -33.0f, -28.3f }, // 0: Front entrance walkway across grand veranda stairs
                { -30.2f, -29.3f }, // 1: Southeast corner approach along courtyard
                { -27.4f, -31.1f }, // 2: Approaching Ozukhana (ablution area) along courtyard
                { -27.1f, -33.5f }, // 3: Beside Ozukhana washing bench & water cistern
                { -27.4f, -36.1f }, // 4: Northeast corner flanking the soaring Azaan minaret
                { -29.8f, -38.7f }, // 5: Northeast turn behind mosque
                { -33.0f, -39.0f }, // 6: North rear walkway behind Mehrab / Qibla projection
                { -36.2f, -38.7f }, // 7: Northwest corner turn
                { -37.7f, -36.3f }, // 8: West plinth courtyard pathway
                { -37.7f, -33.5f }, // 9: Western plinth pathway center
                { -37.4f, -30.7f }, // 10: Southwest corner approach
                { -35.6f, -28.7f }  // 11: Southwest approach back to the front entrance road
            };
            const int N = 12;

            // Loop parameter: 1 complete circuit every ~37.5 seconds (peaceful meditative walking pace ~0.95 m/s)
            float loopU = fmodf(animTime * 0.32f, (float)N);
            if (loopU < 0.0f) loopU += (float)N;
            int idx = (int)loopU;
            float t = loopU - (float)idx;

            auto getPoint = [&](int i) -> PathPoint {
                int wrapped = (i % N + N) % N;
                return s_mosqueLoop[wrapped];
            };

            PathPoint p0 = getPoint(idx - 1);
            PathPoint p1 = getPoint(idx);
            PathPoint p2 = getPoint(idx + 1);
            PathPoint p3 = getPoint(idx + 2);

            // Centripetal Catmull-Rom spline evaluation
            float t2 = t * t;
            float t3 = t2 * t;

            float curX = 0.5f * ((2.0f * p1.x) +
                                 (-p0.x + p2.x) * t +
                                 (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 +
                                 (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);

            float curZ = 0.5f * ((2.0f * p1.z) +
                                 (-p0.z + p2.z) * t +
                                 (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t2 +
                                 (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t3);

            // Velocity tangent derivative for exact continuous forward heading
            float velX = 0.5f * ((-p0.x + p2.x) +
                                 2.0f * (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t +
                                 3.0f * (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t2);

            float velZ = 0.5f * ((-p0.z + p2.z) +
                                 2.0f * (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t +
                                 3.0f * (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t2);

            float walkerYaw = atan2f(velX, velZ);

            // Natural walking stride cadence (~1.5 steps per second)
            float strideFreq = 4.6f;
            float strideAngle = (animTime > 0.0f) ? (sinf(animTime * strideFreq) * radians(26.0f)) : 0.0f;
            float walkBob = (animTime > 0.0f) ? (fabsf(sinf(animTime * strideFreq)) * 0.022f) : 0.0f;
            float torsoTwist = (animTime > 0.0f) ? (sinf(animTime * strideFreq) * radians(2.2f)) : 0.0f;

            PersonParams walker;
            walker.skinColor     = vec3(0.54f, 0.37f, 0.23f);   // warm Bengali skin tone
            walker.shirtColor    = vec3(0.95f, 0.96f, 0.95f);   // clean white cotton Punjabi / Kurta
            walker.pantsColor    = vec3(0.18f, 0.38f, 0.28f);   // deep green checkered Lungi
            walker.isElder       = true;                        // revered village elder with white beard
            walker.hasTupi       = true;                        // traditional white cotton prayer cap (Tupi / টুপি)
            walker.hasGamcha     = true;                        // red cotton Gamcha draped over shoulder
            walker.gamchaColor   = vec3(0.82f, 0.22f, 0.16f);
            walker.leftLegAngle  = strideAngle;
            walker.rightLegAngle = -strideAngle;
            walker.leftArmAngle  = -strideAngle * 0.72f;
            walker.rightArmAngle = strideAngle * 0.72f;

            mat4 walkerM = mat4::identity();
            walkerM = translate(walkerM, vec3(curX, walkBob, curZ));
            walkerM = rotate(walkerM, walkerYaw, vec3(0.0f, 1.0f, 0.0f));
            walkerM = rotate(walkerM, torsoTwist, vec3(0.0f, 1.0f, 0.0f));
            walkerM = scale(walkerM, vec3(1.02f)); // dignified, full adult height

            Texture::bind(TEX_FABRIC, 0);
            shader.setInt("uTextureType", (int)TEX_FABRIC);
            Person::draw(shader, walkerM, walker);
        }

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
                // 1. House 1: Main Central Homestead (Moddho Bari Chouchala + verandah & steps)
                { -10.8f,  -6.2f,  -5.8f,  -1.2f },
                // 2. House 2: North Bari Dochala House (Uttar Bari)
                { -25.8f, -21.2f, -14.8f, -10.2f },
                // 3. House 5: West Bari Dochala House (Paschim Bari)
                { -27.8f, -23.2f,   2.8f,   7.2f },
                // 4. Kitchen Hut (Outdoor Thatched Kitchen)
                { -14.2f, -10.8f, -13.2f,  -9.8f },
                // 5. Cow Shed (Gowal Ghor in Uttar Bari)
                { -29.8f, -25.2f, -12.5f,  -8.5f },
                // 6. Outdoor Cooking Stove (Matir Chula) & Bamboo Windbreak Fence
                {  -4.2f,  -1.2f,  -4.8f,  -2.4f },
                // 7. Chicken Coop on Stilts (Murgir Khopa in North Bari Yard)
                { -16.5f, -14.5f, -17.5f, -15.5f },
                // 8. Tubewell Concrete Pad (Chapa Kol)
                {  -4.7f,  -3.1f,  -2.5f,  -0.8f },
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

            if (isNight) {
                // ── AT NIGHT: ALL HENS AND DUCKS MUST BE INSIDE HENS/DUCK HOUSE ──
                // 1. All 5 Village Hens (Deshi Murgi) roosting safely inside the Chicken Coop (Murgir Khopa)
                const struct HenRoost {
                    vec3  localPos;
                    float yaw;
                    float scaleVal;
                } s_henRoosts[5] = {
                    { vec3(-0.24f, 0.36f + 0.04f, -0.16f),  radians( 18.0f), 0.85f },
                    { vec3( 0.00f, 0.36f + 0.04f, -0.18f),  radians( -6.0f), 0.82f },
                    { vec3( 0.24f, 0.36f + 0.04f, -0.16f),  radians(-22.0f), 0.86f },
                    { vec3(-0.13f, 0.36f + 0.16f,  0.06f),  radians( 12.0f), 0.80f },
                    { vec3( 0.13f, 0.36f + 0.16f,  0.06f),  radians(-15.0f), 0.82f }
                };
                for (int h = 0; h < 5; ++h) {
                    float breathe = (animTime > 0.0f) ? (sinf(animTime * 2.0f + (float)h * 1.2f) * 0.003f) : 0.0f;
                    mat4 henM = coopM;
                    henM = translate(henM, s_henRoosts[h].localPos + vec3(0.0f, breathe, 0.0f));
                    henM = rotate(henM, s_henRoosts[h].yaw, vec3(0.0f, 1.0f, 0.0f));
                    henM = scale(henM, vec3(s_henRoosts[h].scaleVal));
                    Hen::draw(shader, henM);
                }

                // 2. All 6 Ducks (Pati Hash) nestled together safely inside the Duck House (Hash-er Ghor)
                const struct DuckRoost {
                    vec3  localPos;
                    float yaw;
                    float scaleVal;
                } s_duckRoosts[6] = {
                    { vec3(-0.28f, 0.34f + 0.03f, -0.15f),  radians( 20.0f), 0.82f },
                    { vec3( 0.00f, 0.34f + 0.03f, -0.18f),  radians( -4.0f), 0.85f },
                    { vec3( 0.28f, 0.34f + 0.03f, -0.15f),  radians(-24.0f), 0.80f },
                    { vec3(-0.20f, 0.34f + 0.03f,  0.10f),  radians( 14.0f), 0.78f },
                    { vec3( 0.00f, 0.34f + 0.03f,  0.12f),  radians(  0.0f), 0.82f },
                    { vec3( 0.20f, 0.34f + 0.03f,  0.10f),  radians(-16.0f), 0.79f }
                };
                for (int d = 0; d < 6; ++d) {
                    float breathe = (animTime > 0.0f) ? (sinf(animTime * 1.8f + (float)d * 1.1f) * 0.0025f) : 0.0f;
                    mat4 duckM = duckHouseM;
                    duckM = translate(duckM, s_duckRoosts[d].localPos + vec3(0.0f, breathe, 0.0f));
                    duckM = rotate(duckM, s_duckRoosts[d].yaw, vec3(0.0f, 1.0f, 0.0f));
                    duckM = scale(duckM, vec3(s_duckRoosts[d].scaleVal));
                    Duck::draw(shader, duckM);
                }
            } else {
                // ── BY DAY: HENS ROAM THE OPEN COURTYARDS & DUCKS ENJOY THE RIVER ──
                // Flock of 5 Free-Range Village Hens (Deshi Murgi)
                // Trajectories placed in authentic open-yard zones across distributed homesteads
                struct HenPath {
                    vec3  basePos;
                    float radiusX;
                    float radiusZ;
                    float speed;
                    float phase;
                    float scaleVal;
                };
                static const HenPath s_henPaths[5] = {
                    { vec3(-5.5f, 0.0f, -0.6f), 0.75f, 0.55f, 0.75f, 0.0f, 1.00f }, // Central courtyard (open earth)
                    { vec3(-17.2f, 0.0f, -15.5f), 0.75f, 0.65f, 0.65f, 1.8f, 0.92f }, // Uttar Bari farmyard near chicken coop & bamboo grove
                    { vec3(-6.2f, 0.0f,  1.2f), 0.55f, 0.50f, 0.80f, 3.4f, 0.95f }, // Moddho Bari west yard open earth
                    { vec3(-2.2f, 0.0f,  2.8f), 0.50f, 0.45f, 0.70f, 4.8f, 0.88f }, // Foreground courtyard inside fence
                    { vec3(-20.5f, 0.0f,  2.5f), 0.70f, 0.55f, 0.60f, 2.2f, 0.90f } // Paschim Bari open yard
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

                // River Obstacle & Boat Collision Avoidance for Swimming Ducks
                // Completely prevents ducks from ever entering or intersecting Moored Boat 1,
                // Cruising Boat 2, or the River Landing Ghat platform.
                auto resolveDuckCollisions = [&](float& dx, float& dz, float duckRadius) {
                    // 1. Boat 1: Red-Sail Boat (North)
                    {
                        float relX = dx - boat1X;
                        float relZ = dz - boat1Z;
                        float cosY = cosf(-boat1Yaw);
                        float sinY = sinf(-boat1Yaw);
                        float lx = relX * cosY - relZ * sinY;
                        float lz = relX * sinY + relZ * cosY;
                        float halfW = 0.65f + duckRadius;
                        float halfL = 2.60f + duckRadius;
                        if (fabsf(lx) < halfW && fabsf(lz) < halfL) {
                            lx = (lx >= 0.0f) ? halfW : -halfW;
                            dx = boat1X + lx * cosf(boat1Yaw) - lz * sinf(boat1Yaw);
                            dz = boat1Z + lx * sinf(boat1Yaw) + lz * cosf(boat1Yaw);
                        }
                    }

                    // 2. Static Obstacle: River Landing Ghat
                    {
                        float ghatMinX = 4.6f - duckRadius;
                        float ghatMaxX = 6.2f + duckRadius;
                        float ghatMinZ = 0.0f - duckRadius;
                        float ghatMaxZ = 2.4f + duckRadius;
                        if (dx > ghatMinX && dx < ghatMaxX && dz > ghatMinZ && dz < ghatMaxZ) {
                            float dLeft   = dx - ghatMinX;
                            float dRight  = ghatMaxX - dx;
                            float dBack   = dz - ghatMinZ;
                            float dFront  = ghatMaxZ - dz;
                            float minD = dLeft;
                            if (dRight < minD) minD = dRight;
                            if (dBack  < minD) minD = dBack;
                            if (dFront < minD) minD = dFront;

                            if (minD == dLeft)        dx = ghatMinX;
                            else if (minD == dRight)  dx = ghatMaxX;
                            else if (minD == dBack)   dz = ghatMinZ;
                            else                      dz = ghatMaxZ;
                        }
                    }

                    // 3. Boat 2: Round Golden-Straw Chhoi Boat with Rowing Majhi (Center)
                    {
                        float relX = dx - boat2X;
                        float relZ = dz - boat2Z;
                        float cosY = cosf(-boat2Yaw);
                        float sinY = sinf(-boat2Yaw);
                        float lx = relX * cosY - relZ * sinY;
                        float lz = relX * sinY + relZ * cosY;
                        float halfW = 0.72f + duckRadius;
                        float halfL = 2.70f + duckRadius;
                        if (fabsf(lx) < halfW && fabsf(lz) < halfL) {
                            lx = (lx >= 0.0f) ? halfW : -halfW;
                            dx = boat2X + lx * cosf(boat2Yaw) - lz * sinf(boat2Yaw);
                            dz = boat2Z + lx * sinf(boat2Yaw) + lz * cosf(boat2Yaw);
                        }
                    }

                    // 4. Boat 3: White Sailboat (South)
                    {
                        float relX = dx - boat3X;
                        float relZ = dz - boat3Z;
                        float cosY = cosf(-boat3Yaw);
                        float sinY = sinf(-boat3Yaw);
                        float lx = relX * cosY - relZ * sinY;
                        float lz = relX * sinY + relZ * cosY;
                        float halfW = 0.65f + duckRadius;
                        float halfL = 2.60f + duckRadius;
                        if (fabsf(lx) < halfW && fabsf(lz) < halfL) {
                            lx = (lx >= 0.0f) ? halfW : -halfW;
                            dx = boat3X + lx * cosf(boat3Yaw) - lz * sinf(boat3Yaw);
                            dz = boat3Z + lx * sinf(boat3Yaw) + lz * cosf(boat3Yaw);
                        }
                    }

                    // 5. Fishing Dingi Boat (Eastern Shallows, Z ~ 8.5m)
                    {
                        float fz = 8.5f;
                        float fx = 8.0f + (1.8f * sinf(fz * 0.08f + 0.4f) + 0.6f * cosf(fz * 0.04f)) + 1.8f;
                        float relX = dx - fx;
                        float relZ = dz - fz;
                        float cosY = cosf(radians(35.0f));
                        float sinY = sinf(radians(35.0f));
                        float lx = relX * cosY - relZ * sinY;
                        float lz = relX * sinY + relZ * cosY;
                        float halfW = 0.50f + duckRadius;
                        float halfL = 1.90f + duckRadius;
                        if (fabsf(lx) < halfW && fabsf(lz) < halfL) {
                            lx = (lx >= 0.0f) ? halfW : -halfW;
                            dx = fx + lx * cosf(radians(-35.0f)) - lz * sinf(radians(-35.0f));
                            dz = fz + lx * sinf(radians(-35.0f)) + lz * cosf(radians(-35.0f));
                        }
                    }
                };

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
                    { vec3(6.2f, 0.0f,  6.2f), 0.65f, 0.95f, 0.45f, 0.0f, 1.00f }, // North lily pad cove
                    { vec3(6.6f, 0.0f,  9.2f), 0.70f, 1.10f, 0.40f, 1.8f, 0.88f }, // Far North riverbank reeds
                    { vec3(6.2f, 0.0f, -5.5f), 0.60f, 0.95f, 0.48f, 3.2f, 0.95f }, // South riverbank cove
                    { vec3(13.6f, 0.0f, -1.5f), 0.85f, 1.35f, 0.42f, 4.7f, 0.92f }  // Far East river mirror
                };

                for (int d = 0; d < 4; ++d) {
                    const auto& dp = s_duckPaths[d];
                    float duckTheta = animTime * dp.speed + dp.phase;
                    float rawDx = dp.basePos.x + sinf(duckTheta) * dp.swimRadiusX;
                    float rawDz = dp.basePos.z + cosf(duckTheta) * dp.swimRadiusZ;

                    float dx = rawDx;
                    float dz = rawDz;
                    resolveDuckCollisions(dx, dz, 0.22f);

                    float nextTheta = duckTheta + 0.08f;
                    float nextDx = dp.basePos.x + sinf(nextTheta) * dp.swimRadiusX;
                    float nextDz = dp.basePos.z + cosf(nextTheta) * dp.swimRadiusZ;
                    resolveDuckCollisions(nextDx, nextDz, 0.22f);

                    float vdx = nextDx - dx;
                    float vdz = nextDz - dz;
                    float duckYaw = (fabsf(vdx) > 1e-4f || fabsf(vdz) > 1e-4f) ?
                                    atan2f(vdx, vdz) :
                                    atan2f(cosf(duckTheta) * dp.swimRadiusX, -sinf(duckTheta) * dp.swimRadiusZ);

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
        }

        // ─── 8. TRADITIONAL DINGI BOATS MATCHING FOLK ARTWORK ───
        Texture::bind(TEX_WOOD, 0);
        shader.setInt("uTextureType", (int)TEX_WOOD);

        // Boat 1 (Left / North upstream): Red-Sail Boat with Standing Majhi
        mat4 boatM1 = mat4::identity();
        boatM1 = translate(boatM1, vec3(boat1X, 0.10f + boatBob1, boat1Z));
        boatM1 = rotate(boatM1, boat1Yaw, vec3(0.0f, 1.0f, 0.0f));
        boatM1 = rotate(boatM1, boatRoll1, vec3(0.0f, 0.0f, 1.0f));
        boatM1 = rotate(boatM1, boatPitch1, vec3(1.0f, 0.0f, 0.0f));
        Boat::draw(shader, boatM1, Boat::BOAT_STYLE_RED_SAIL, animTime, 0.0f, 0);

        // Boat 2 (Center): Round Golden-Straw Chhoi Dome Boat with Seated Rowing Majhi, Boitha Oar & 2 Passengers
        mat4 boatM2 = mat4::identity();
        boatM2 = translate(boatM2, vec3(boat2X, 0.10f + boatBob2, boat2Z));
        boatM2 = rotate(boatM2, boat2Yaw, vec3(0.0f, 1.0f, 0.0f));
        boatM2 = rotate(boatM2, boatRoll2, vec3(0.0f, 0.0f, 1.0f));
        boatM2 = rotate(boatM2, boatPitch2, vec3(1.0f, 0.0f, 0.0f));
        Boat::draw(shader, boatM2, Boat::BOAT_STYLE_ROUND_CHHOI, animTime, oarRowAnim, 2);

        // Dynamic stern wake ripples when boat is in motion
        if (fabsf(g_boatSpeed) > 0.10f || g_boatStepDistRemaining > 0.01f) {
            vec3 wakeColor(0.85f, 0.94f, 0.98f);
            for (int w = 1; w <= 3; ++w) {
                float wakeDist = (float)w * 0.90f;
                float wakeExpand = 0.65f + (float)w * 0.40f;
                mat4 wakeM = boatM2;
                wakeM = translate(wakeM, vec3(0.0f, -0.075f, -1.8f - wakeDist));
                wakeM = scale(wakeM, vec3(wakeExpand, 1.0f, 0.12f));
                Primitives::drawPlane(shader, wakeM, wakeColor);
            }
        }

        // Boat 3 (Right / South downstream): Crisp White Sailboat with Tall Bamboo Mast
        mat4 boatM3 = mat4::identity();
        boatM3 = translate(boatM3, vec3(boat3X, 0.10f + boatBob3, boat3Z));
        boatM3 = rotate(boatM3, boat3Yaw, vec3(0.0f, 1.0f, 0.0f));
        boatM3 = rotate(boatM3, boatRoll3, vec3(0.0f, 0.0f, 1.0f));
        boatM3 = rotate(boatM3, boatPitch3, vec3(1.0f, 0.0f, 0.0f));
        Boat::draw(shader, boatM3, Boat::BOAT_STYLE_WHITE_SAIL, animTime, 0.0f, 0);

        // ─── 8.5 TRADITIONAL RIVER FISHERMAN HUNTING FISH (জেলে ও মাছ শিকার) ───
        // Located in the tranquil eastern river shallows at Z = 8.5m, X ≈ 11.95m
        // Slender wooden fishing dingi, standing athletic fisherman casting circular
        // net (Khepla Jal) with weighted sinkers, leaping silver river fish,
        // bamboo Khalui basket with catch, Polo plunge trap, Teta spear, and Hariken.
        {
            float fisherZ = 8.5f;
            float fisherX = 8.0f + (1.8f * sinf(fisherZ * 0.08f + 0.4f) + 0.6f * cosf(fisherZ * 0.04f)) + 1.8f;
            mat4 fisherM = mat4::identity();
            fisherM = translate(fisherM, vec3(fisherX, 0.08f, fisherZ));
            fisherM = rotate(fisherM, radians(-35.0f), vec3(0.0f, 1.0f, 0.0f));
            Fisherman::draw(shader, fisherM, animTime);
        }

        // ── Clean Up Bound VAO, Swap Buffers & Poll Events ─────
        Primitives::resetVAOState();

        static int s_shotFrame = 0;
        if (g_shotMode) {
            s_shotFrame++;
            if (s_shotFrame >= 4) {
                int fbW = 0, fbH = 0;
                glfwGetFramebufferSize(window, &fbW, &fbH);
                const char* shotFile = g_shotCow ? "screenshot_shifted_cowshed.bmp" : (g_shotFisher ? "screenshot_fisherman_boat.bmp" : (g_shotRiver ? "screenshot_river.bmp" : (g_shotTrees ? "screenshot_shifted_trees.bmp" : (g_shotPump ? "screenshot_shifted_tubewell.bmp" : (g_shotOldMosque ? "screenshot_old_cleared.bmp" : (g_shotMosque ? "screenshot_shifted_mosque.bmp" : (g_shotBari8 ? "screenshot_bari8.bmp" : (g_shotField ? "screenshot_shifted_house.bmp" : (g_shotDay ? "screenshot_day_sun.bmp" : "screenshot_house.bmp")))))))));
                saveBMP(shotFile, fbW, fbH);
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
        }

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