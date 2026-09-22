// main.cpp — Village Gathering by the River: A Bangladeshi Rural Night Scene in 3D
// OpenGL 3.3 Core Profile Computer Graphics Project
// Interactive orbit camera (left-drag = rotate, scroll = zoom, ESC = exit)
// Camera Presets: '1' = Courtyard Gathering, '2' = River & Boat, '3' = Full Overview, '4' = Homestead
// Lighting Toggle: 'L' = Cycle between Moonlit Night (Default), Golden Dusk, and Crisp Day

#include <iostream>
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

using namespace math;

// ─── Globals ─────────────────────────────────────────────────────────
static Camera  camera;
static bool    dragging   = false;
static double  lastMouseX = 0.0, lastMouseY = 0.0;
static int     winWidth   = 1280, winHeight = 720;

// Display / Milestone Mode:
//   0 = No Lighting (Crisp Unlit 3D Facets - Milestone Default)
//   1 = No Lighting (Pure Flat Object Color)
//   2 = Full Moonlit Night Scene Preview
static int  lightingMode = 0;
static bool showTerrain  = true; // Key 'T' toggles terrain/river visibility

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

static void keyCallback(GLFWwindow* window, int key, int, int action, int)
{
    if (action != GLFW_PRESS) return;

    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }
    // Lighting Mode Toggle
    else if (key == GLFW_KEY_L) {
        lightingMode = (lightingMode + 1) % 3;
        const char* modeNames[] = {
            "No Lighting (Crisp Unlit 3D Facets - Milestone Default)",
            "No Lighting (Pure Flat Object Color)",
            "Full Moonlit Night Scene Preview"
        };
        std::cout << "Display Mode: " << modeNames[lightingMode] << std::endl;
    }
    // Terrain / Ground Visibility Toggle
    else if (key == GLFW_KEY_T) {
        showTerrain = !showTerrain;
        std::cout << "Terrain / Ground: " << (showTerrain ? "VISIBLE" : "HIDDEN (Freestanding Objects Only)") << std::endl;
    }
    // Camera Presets for Object Inspection
    else if (key == GLFW_KEY_1) {
        // 1: Village House & Clay Cooking Stove
        camera.target   = vec3(-8.5f, 1.4f, -6.0f);
        camera.yaw      = radians(45.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 9.5f;
        camera.updatePosition();
        std::cout << "View 1: Village House (Chouchala 4-Sloped Roof & Outdoor Clay Stove)\n";
    }
    else if (key == GLFW_KEY_2) {
        // 2: Dingi Nouka with Hanging Hariken Lantern
        camera.target   = vec3(7.2f, 0.6f, 1.2f);
        camera.yaw      = radians(-28.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 7.5f;
        camera.updatePosition();
        std::cout << "View 2: Traditional Dingi Nouka (Curved Hull, Pointed Bow/Stern, Chhoi & Hariken)\n";
    }
    else if (key == GLFW_KEY_3) {
        // 3: Charpai, Seated Elder & Handmade Fan
        camera.target   = vec3(-3.8f, 0.7f, 0.8f);
        camera.yaw      = radians(42.0f);
        camera.pitch    = radians(14.0f);
        camera.distance = 5.2f;
        camera.updatePosition();
        std::cout << "View 3: Charpai, Seated Elder & Handmade Palm-Leaf Fan (Haat Pakha)\n";
    }
    else if (key == GLFW_KEY_4) {
        // 4: Child Reading Book
        camera.target   = vec3(-1.8f, 0.45f, 2.0f);
        camera.yaw      = radians(10.0f);
        camera.pitch    = radians(14.0f);
        camera.distance = 3.5f;
        camera.updatePosition();
        std::cout << "View 4: Child Reading Book on Courtyard\n";
    }
    else if (key == GLFW_KEY_5) {
        // 5: Rural Trees (1 Coconut Palm, 1 Banana, 1 Mango, 1 Bamboo)
        camera.target   = vec3(4.2f, 2.5f, -8.5f);
        camera.yaw      = radians(-15.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 15.0f;
        camera.updatePosition();
        std::cout << "View 5: Rural Trees (1 Coconut Palm, 1 Banana, 1 Mango, 1 Bamboo)\n";
    }
    else if (key == GLFW_KEY_6) {
        // 6: Animals (1 Hen & 1 Duck)
        camera.target   = vec3(-4.5f, 0.4f, 0.4f);
        camera.yaw      = radians(25.0f);
        camera.pitch    = radians(14.0f);
        camera.distance = 7.5f;
        camera.updatePosition();
        std::cout << "View 6: Village Animals (1 Hen & 1 River Duck)\n";
    }
    else if (key == GLFW_KEY_7) {
        // 7: Rural Rice Tree (Dhan Gachh), Grass & Water Grass (Kashbon)
        camera.target   = vec3(-8.5f, 0.4f, 8.0f);
        camera.yaw      = radians(40.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 6.0f;
        camera.updatePosition();
        std::cout << "View 7: Rural Rice Tree (Dhan Gachh), Meadow Grass & Water Grass (Kashbon)\n";
    }
    else if (key == GLFW_KEY_8) {
        // 8: Full Scene Overview
        camera.target   = vec3(-1.0f, 1.0f, 0.0f);
        camera.yaw      = radians(30.0f);
        camera.pitch    = radians(28.0f);
        camera.distance = 28.0f;
        camera.updatePosition();
        std::cout << "View 8: Full Village Scene Overview\n";
    }
}

static void glfwErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error [" << error << "]: " << description << std::endl;
}

// ─── Main ────────────────────────────────────────────────────────────
int main()
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
    std::cout << "  BANGLADESHI RURAL VILLAGE 3D SCENE - OBJECT MILESTONE\n";
    std::cout << "  (Objects Only | No Motion | No Lighting)\n";
    std::cout << "========================================================\n";
    std::cout << "CONTROLS:\n";
    std::cout << "  Left-Drag Mouse : Orbit Camera Around Object\n";
    std::cout << "  Scroll Wheel    : Zoom In / Out\n";
    std::cout << "  W / S / A / D   : Move Camera Freely\n";
    std::cout << "  Key '1'         : Inspect House (Chouchala & Clay Cooking Stove)\n";
    std::cout << "  Key '2'         : Inspect Boat (Dingi Nouka with Hanging Hariken)\n";
    std::cout << "  Key '3'         : Inspect Charpai, Seated Elder & Handmade Fan\n";
    std::cout << "  Key '4'         : Inspect Child Reading Book\n";
    std::cout << "  Key '5'         : Inspect Trees (1 Palm, 1 Banana, 1 Mango, 1 Bamboo)\n";
    std::cout << "  Key '6'         : Inspect Animals (1 Hen & 1 Duck)\n";
    std::cout << "  Key '7'         : Inspect Rice Tree (Dhan Gachh), Grass & Water Grass\n";
    std::cout << "  Key '8'         : Full Scene Overview\n";
    std::cout << "  Key 'T'         : Toggle Terrain/Ground Visibility\n";
    std::cout << "  Key 'L'         : Toggle Lighting (0: Unlit 3D | 1: Flat | 2: Night)\n";
    std::cout << "  ESC             : Exit\n";
    std::cout << "========================================================\n";

    // ── OpenGL State ────────────────────────────────────────────
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // ── Shader ──────────────────────────────────────────────────
    Shader shader;
    shader.init(Shader::phongVertexSrc, Shader::phongFragmentSrc);

    // ── Primitives ──────────────────────────────────────────────
    Primitives::init();

    // ── Camera Default Setup (Atmospheric framed village clearing) ──
    camera.target   = vec3(-2.0f, 0.8f, 0.5f);
    camera.yaw      = radians(32.0f);
    camera.pitch    = radians(38.0f);
    camera.distance = 25.0f;
    camera.updatePosition();

    // Lantern position in courtyard (acts as point light source)
    vec3 lanternPos(-2.6f, 0.40f, 0.6f);

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
            camera.processKeyboardMovement(camFwd, camRgt, dt);
        }

        // ── Motion Control (No motion within objects for milestone grading) ──
        float animTime = (lightingMode == 2) ? time : 0.0f;

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
            // Milestone Default: Crisp Unlit 3D Facets (Daylight, 0 darkness, 0 shadows, 0 point lights)
            clearColor      = vec3(0.82f, 0.88f, 0.94f); // clean daylight sky
            fogCol          = clearColor;
            lightDir        = normalize(vec3(0.35f, -0.90f, -0.40f));
            lightColor      = vec3(1.0f, 1.0f, 1.0f);
            ambientStrength = 1.0f;
            noLightVal      = 1; // gentle facet shading for crisp 3D recognition
        }
        else if (lightingMode == 1) {
            // Milestone Mode: Pure Flat Object Color (completely unshaded)
            clearColor      = vec3(0.88f, 0.90f, 0.92f); // neutral light studio backdrop
            fogCol          = clearColor;
            lightDir        = normalize(vec3(0.0f, -1.0f, 0.0f));
            lightColor      = vec3(1.0f, 1.0f, 1.0f);
            ambientStrength = 1.0f;
            noLightVal      = 2; // pure flat unshaded objectColor
        }
        else {
            // Full Moonlit Night Scene Preview
            clearColor       = vec3(0.04f, 0.06f, 0.14f); // deep midnight indigo
            fogCol           = clearColor;
            lightDir         = normalize(vec3(0.35f, -0.85f, -0.40f)); // cool moonlight
            lightColor       = vec3(0.50f, 0.62f, 0.85f);              // silvery lunar light
            ambientStrength  = 0.28f;
            specularStrength = 0.35f;
            pointIntensity   = 2.40f;
            fogDens          = 0.014f;
            noLightVal       = 0; // full Phong lighting with lanterns & fog
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

        // Point Light 1: Courtyard Hurricane Lantern
        float flameFlicker = (lightingMode == 2) ? (1.0f + 0.05f * sinf(time * 11.3f) * cosf(time * 17.7f)) : 1.0f;
        shader.setVec3("pointLightPos",       vec3(lanternPos.x, 0.425f, lanternPos.z));
        shader.setVec3("pointLightColor",     pointColor);
        shader.setFloat("pointLightIntensity", pointIntensity * flameFlicker);

        // Point Light 2: Boat Hurricane Lantern
        float boatBobLight = (lightingMode == 2) ? (sinf(time * 1.6f) * 0.018f) : 0.0f;
        float boatFlameFlicker = (lightingMode == 2) ? (1.0f + 0.06f * sinf(time * 9.7f) * cosf(time * 14.3f)) : 1.0f;
        vec3 boatLanternWorldPos = vec3(7.2f, 0.11f + boatBobLight + 0.42f, 1.2f + 1.08f);
        shader.setVec3("pointLight2Pos",       boatLanternWorldPos);
        shader.setVec3("pointLight2Color",     pointColor);
        shader.setFloat("pointLight2Intensity", pointIntensity * boatFlameFlicker * 0.88f);

        // Atmospheric Distance Fog
        shader.setVec3("fogColor",  fogCol);
        shader.setFloat("fogDensity", fogDens);

        // ─── 1. NIGHT SKY & CELESTIAL BODIES (Only in Night Preview Mode) ─
        if (lightingMode == 2) {
            // Stars in the sky dome
            Stars::draw(shader);

            // Full Moon with glowing corona
            mat4 moonM = mat4::identity();
            moonM = translate(moonM, vec3(16.0f, 22.0f, -24.0f));
            moonM = scale(moonM, vec3(1.5f, 1.5f, 1.5f));
            Moon::draw(shader, moonM);

            // Fireflies blinking across village & riverbank
            Fireflies::draw(shader, time);
        }

        // ─── 2. TERRAIN & COURTYARD (Toggled by Key 'T') ─────────
        if (showTerrain) {
            mat4 terrainM = mat4::identity();
            Terrain::draw(shader, terrainM);

            mat4 riverM = mat4::identity();
            riverM = translate(riverM, vec3(8.0f, 0.0f, 0.0f));
            River::draw(shader, riverM, animTime);
        }

        // ─── 4. VILLAGE HOUSE (1 TRADITIONAL BENGALI HOMESTEAD) ──
        // Single residential house (Chouchala 4-sloped hip roof, verandah & outdoor clay cooking stove)
        mat4 house1 = mat4::identity();
        house1 = translate(house1, vec3(-8.5f, 0.0f, -6.0f));
        house1 = rotate(house1, radians(6.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house1, HOUSE_CHOUCHALA);

        // ─── 5. VEGETATION (1 OF EACH TREE TYPE FOR MILESTONE INSPECTION) ──
        // 1. Coconut Palm (curved trunk, crown with cascading leaflets)
        mat4 palm1 = mat4::identity();
        palm1 = translate(palm1, vec3(4.2f, 0.0f, -8.5f));
        palm1 = rotate(palm1, radians(-12.0f), vec3(0.0f, 1.0f, 0.0f));
        Tree::draw(shader, palm1, TREE_PALM);

        // 2. Banana Tree (curved stem, broad paddle leaves)
        mat4 banana1 = mat4::identity();
        banana1 = translate(banana1, vec3(-13.5f, 0.0f, -4.5f));
        Tree::draw(shader, banana1, TREE_BANANA);

        // 3. Majestic Branching Banyan / Mango Tree (heavy branching trunk, dense canopy)
        mat4 mangoTree = mat4::identity();
        mangoTree = translate(mangoTree, vec3(-16.5f, 0.0f, -10.5f));
        Tree::draw(shader, mangoTree, TREE_GENERAL);

        // 4. Traditional Bamboo Grove (Bansher Jhar) behind homestead (far away from river!)
        mat4 bambooGrove = mat4::identity();
        bambooGrove = translate(bambooGrove, vec3(-19.0f, 0.0f, -14.0f));
        bambooGrove = rotate(bambooGrove, radians(30.0f), vec3(0.0f, 1.0f, 0.0f));
        Tree::draw(shader, bambooGrove, TREE_BAMBOO);

        // ─── 6. VILLAGE GATHERING (UTHAN) — EXACTLY 2 PEOPLE: OLD MAN & CHILD ─
        // Traditional woven Charpai (bed)
        mat4 charpaiM = mat4::identity();
        charpaiM = translate(charpaiM, vec3(-3.8f, 0.0f, 0.8f));
        charpaiM = rotate(charpaiM, radians(-12.0f), vec3(0.0f, 1.0f, 0.0f));
        Charpai::draw(shader, charpaiM);

        // Person 1: Seated Elder on the Charpai edge holding handmade fan (Haat Pakha)
        PersonParams elder;
        elder.skinColor   = vec3(0.52f, 0.35f, 0.22f);
        elder.shirtColor  = vec3(0.92f, 0.90f, 0.85f); // white cotton kurta
        elder.pantsColor  = vec3(0.55f, 0.14f, 0.08f); // maroon lungi
        elder.seated      = true;
        elder.isElder     = true;                      // dignified white hair and beard
        elder.hasGamcha   = true;                      // red gamcha over shoulder
        elder.gamchaColor = vec3(0.80f, 0.20f, 0.14f);
        elder.hasFan      = true;                      // holds traditional handmade fan (Haat Pakha) in hand!
        elder.fanSway     = (animTime > 0.0f) ? (sinf(animTime * 2.6f) * radians(8.0f)) : 0.0f; // static rest pose in inspection mode

        mat4 elderM = charpaiM;
        // Pelvis directly on mattress surface (Y = 0.50f), seated along edge facing courtyard (+Z / lantern)
        elderM = translate(elderM, vec3(0.20f, 0.50f, 0.0f));
        elderM = rotate(elderM, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
        Person::draw(shader, elderM, elder);

        // Low wooden stool with glowing Hurricane Lantern (Hariken — little light source)
        mat4 stoolM = mat4::identity();
        stoolM = translate(stoolM, vec3(lanternPos.x, 0.12f, lanternPos.z));
        stoolM = scale(stoolM, vec3(0.38f, 0.24f, 0.38f));
        Primitives::drawCube(shader, stoolM, vec3(0.35f, 0.22f, 0.10f));

        mat4 lanternM = mat4::identity();
        lanternM = translate(lanternM, vec3(lanternPos.x, 0.24f, lanternPos.z));
        Charpai::drawLantern(shader, lanternM);

        // Person 2: Child sitting cross-legged on the swept yard reading book
        PersonParams child;
        child.skinColor   = vec3(0.52f, 0.36f, 0.22f);
        child.shirtColor  = vec3(0.88f, 0.45f, 0.15f); // saffron/orange shirt
        child.pantsColor  = vec3(0.25f, 0.38f, 0.20f); // green shorts
        child.crossLegged = true;
        mat4 childM = mat4::identity();
        childM = translate(childM, vec3(-1.8f, 0.0f, 2.0f));
        childM = rotate(childM, radians(-70.0f), vec3(0.0f, 1.0f, 0.0f));
        childM = scale(childM, vec3(0.68f, 0.68f, 0.68f));
        Person::draw(shader, childM, child);

        // Book held by child
        mat4 bookM = childM;
        bookM = translate(bookM, vec3(0.0f, 0.24f, 0.28f));
        bookM = rotate(bookM, radians(25.0f), vec3(1.0f, 0.0f, 0.0f));
        bookM = scale(bookM, vec3(0.35f, 0.035f, 0.26f));
        Primitives::drawCube(shader, bookM, vec3(0.85f, 0.85f, 0.80f));

        // ─── 7. ANIMALS (1 HEN & 1 DUCK FOR MILESTONE INSPECTION) ──
        // Village Hen (pecking ground, wings, tail, comb)
        mat4 hen1 = mat4::identity();
        hen1 = translate(hen1, vec3(-5.2f, 0.0f, 0.8f));
        hen1 = rotate(hen1, radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
        Hen::draw(shader, hen1);

        // Duck on the river water
        float duckBob1 = (animTime > 0.0f) ? (sinf(animTime * 2.5f) * 0.02f) : 0.0f;
        mat4 duck1 = mat4::identity();
        duck1 = translate(duck1, vec3(7.5f, 0.02f + duckBob1, -1.8f));
        duck1 = rotate(duck1, radians(25.0f), vec3(0.0f, 1.0f, 0.0f));
        Duck::draw(shader, duck1);

        // ─── 8. TRADITIONAL DINGI NOUKA (1 MOORED BOAT WITH HARIKEN) ──
        // Boat resting right at the natural sandy riverbank
        float boatBob  = (animTime > 0.0f) ? (sinf(animTime * 1.6f) * 0.018f) : 0.0f;
        float boatRoll = (animTime > 0.0f) ? (sinf(animTime * 1.4f) * radians(1.2f)) : 0.0f;

        mat4 boatM = mat4::identity();
        boatM = translate(boatM, vec3(7.2f, 0.11f + boatBob, 1.2f));
        boatM = rotate(boatM, radians(6.0f), vec3(0.0f, 1.0f, 0.0f));
        boatM = rotate(boatM, boatRoll, vec3(0.0f, 0.0f, 1.0f));
        Boat::draw(shader, boatM);

        // ── Swap Buffers & Poll Events ──────────────────────────
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ── Cleanup ─────────────────────────────────────────────────
    Primitives::cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}