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

// Lighting Mode: 0 = Moonlit Night (Default), 1 = Golden Sunset, 2 = Daytime
static int lightingMode = 0;

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
        const char* modeNames[] = { "Moonlit Night (Default)", "Golden Dusk / Sunset", "Crisp Tropical Day" };
        std::cout << "Lighting Mode: " << modeNames[lightingMode] << std::endl;
    }
    // Camera Presets
    else if (key == GLFW_KEY_1) {
        // Courtyard Gathering closeup
        camera.target   = vec3(-3.5f, 0.9f, 0.8f);
        camera.yaw      = radians(32.0f);
        camera.pitch    = radians(16.0f);
        camera.distance = 10.0f;
        camera.updatePosition();
        std::cout << "Camera View 1: Courtyard Gathering Closeup\n";
    }
    else if (key == GLFW_KEY_2) {
        // River Shore & Moored Boat
        camera.target   = vec3(7.2f, 0.8f, 1.2f);
        camera.yaw      = radians(-28.0f);
        camera.pitch    = radians(18.0f);
        camera.distance = 12.0f;
        camera.updatePosition();
        std::cout << "Camera View 2: River Shore & Moored Boat\n";
    }
    else if (key == GLFW_KEY_3) {
        // Full Village Overview
        camera.target   = vec3(0.0f, 1.5f, 0.0f);
        camera.yaw      = radians(28.0f);
        camera.pitch    = radians(24.0f);
        camera.distance = 32.0f;
        camera.updatePosition();
        std::cout << "Camera View 3: Full Village Overview\n";
    }
    else if (key == GLFW_KEY_4) {
        // Homestead & Houses
        camera.target   = vec3(-6.5f, 1.6f, -7.5f);
        camera.yaw      = radians(55.0f);
        camera.pitch    = radians(20.0f);
        camera.distance = 16.0f;
        camera.updatePosition();
        std::cout << "Camera View 4: Rural Homestead & Houses\n";
    }
    else if (key == GLFW_KEY_5) {
        // Lush Paddy Fields (Dhan Khet)
        camera.target   = vec3(-11.5f, 0.6f, 15.0f);
        camera.yaw      = radians(45.0f);
        camera.pitch    = radians(22.0f);
        camera.distance = 18.0f;
        camera.updatePosition();
        std::cout << "Camera View 5: Lush Paddy Fields (Dhan Khet)\n";
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
    std::cout << "CONTROLS:\n";
    std::cout << "  Left-Drag Mouse : Orbit Camera\n";
    std::cout << "  Scroll Wheel    : Zoom In / Out\n";
    std::cout << "  W / S / A / D   : Move Camera (Forward / Backward / Left / Right)\n";
    std::cout << "  Key 'L'         : Cycle Lighting (Night -> Sunset -> Day)\n";
    std::cout << "  Key '1'         : View 1 - Courtyard Gathering\n";
    std::cout << "  Key '2'         : View 2 - River Shore & Moored Boat\n";
    std::cout << "  Key '3'         : View 3 - Full Village Overview\n";
    std::cout << "  Key '4'         : View 4 - Rural Homestead\n";
    std::cout << "  Key '5'         : View 5 - Lush Paddy Fields (Dhan Khet)\n";
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

        // ── Lighting Mode Setup ─────────────────────────────────
        vec3 clearColor;
        vec3 lightDir;
        vec3 lightColor;
        float ambientStrength;
        float specularStrength = 0.35f;
        float shininess        = 32.0f;
        vec3  pointColor       (1.0f, 0.78f, 0.32f); // warm golden kerosene glow
        float pointIntensity   = 0.0f;
        vec3  fogCol;
        float fogDens          = 0.014f;

        if (lightingMode == 0) {
            // Mode 0: Serene Moonlit Night (Deep, moody, authentic village night)
            clearColor      = vec3(0.04f, 0.06f, 0.14f); // deep midnight indigo
            fogCol          = clearColor;
            lightDir        = normalize(vec3(0.35f, -0.85f, -0.40f)); // cool moonlight
            lightColor      = vec3(0.50f, 0.62f, 0.85f);              // silvery lunar light
            ambientStrength = 0.28f;                                  // deep, quiet nocturnal shadows
            pointIntensity  = 2.40f;                                  // warm golden radiant courtyard lantern
            fogDens         = 0.014f;                                 // atmospheric midnight depth
        }
        else if (lightingMode == 1) {
            // Mode 1: Golden Sunset / Twilight (Godhuli Bela)
            clearColor      = vec3(0.36f, 0.18f, 0.24f); // crimson/amber dusk
            fogCol          = clearColor;
            lightDir        = normalize(vec3(-0.85f, -0.30f, -0.38f)); // low setting sun
            lightColor      = vec3(1.00f, 0.68f, 0.38f);              // warm golden sunlight
            ambientStrength = 0.40f;
            pointIntensity  = 1.50f;
            fogDens         = 0.008f;
        }
        else {
            // Mode 2: Crisp Tropical Day
            clearColor      = vec3(0.48f, 0.72f, 0.88f); // bright sky blue
            fogCol          = vec3(0.55f, 0.76f, 0.90f);
            lightDir        = normalize(vec3(-0.35f, -0.90f, -0.35f)); // overhead sun
            lightColor      = vec3(1.00f, 0.98f, 0.92f);              // warm white sun
            ambientStrength = 0.48f;
            pointIntensity  = 0.0f;  // lantern flame less dominant in bright sun
            fogDens         = 0.006f;
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

        // Point Light 1: Courtyard Hurricane Lantern (Hariken — little light source)
        float flameFlicker = 1.0f + 0.05f * sinf(time * 11.3f) * cosf(time * 17.7f);
        shader.setVec3("pointLightPos",       vec3(lanternPos.x, 0.425f, lanternPos.z));
        shader.setVec3("pointLightColor",     pointColor);
        shader.setFloat("pointLightIntensity", pointIntensity * flameFlicker);

        // Point Light 2: Boat Hurricane Lantern (Hariken hanging at front of Chhoi)
        float boatBobLight = sinf(time * 1.6f) * 0.018f;
        float boatFlameFlicker = 1.0f + 0.06f * sinf(time * 9.7f) * cosf(time * 14.3f);
        vec3 boatLanternWorldPos = vec3(7.2f, 0.11f + boatBobLight + 0.42f, 1.2f + 1.08f);
        shader.setVec3("pointLight2Pos",       boatLanternWorldPos);
        shader.setVec3("pointLight2Color",     pointColor);
        shader.setFloat("pointLight2Intensity", pointIntensity * boatFlameFlicker * 0.88f);

        // Atmospheric Distance Fog
        shader.setVec3("fogColor",  fogCol);
        shader.setFloat("fogDensity", fogDens);

        // ─── 1. NIGHT SKY & CELESTIAL BODIES ────────────────────
        if (lightingMode != 2) {
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

        // ─── 2. TERRAIN & COURTYARD (UTHAN) ─────────────────────
        mat4 terrainM = mat4::identity();
        Terrain::draw(shader, terrainM);

        // ─── 3. MEANDERING RIVER ────────────────────────────────
        mat4 riverM = mat4::identity();
        riverM = translate(riverM, vec3(8.0f, 0.0f, 0.0f));
        River::draw(shader, riverM, time);

        // ─── 4. VILLAGE HOUSES (DISTRIBUTED WITH BREATHING ROOM) ─
        // House 1: Main residential house (Chouchala 4-sloped hip roof)
        mat4 house1 = mat4::identity();
        house1 = translate(house1, vec3(-8.5f, 0.0f, -6.0f));
        house1 = rotate(house1, radians(6.0f), vec3(0.0f, 1.0f, 0.0f));
        House::draw(shader, house1, HOUSE_CHOUCHALA);

        // House 2: Side cottage / kitchen (Dochala 2-sloped pitched gable roof with outdoor clay stove)
        mat4 house2 = mat4::identity();
        house2 = translate(house2, vec3(-3.0f, 0.0f, -11.5f));
        house2 = rotate(house2, radians(22.0f), vec3(0.0f, 1.0f, 0.0f));
        house2 = scale(house2, vec3(0.85f, 0.88f, 0.85f));
        House::draw(shader, house2, HOUSE_DOCHALA);

        // ─── 5. VEGETATION (TREES DISTRIBUTED ACROSS MAP) ────────
        // Coconut Palm 1: Village riverbank north
        mat4 palm1 = mat4::identity();
        palm1 = translate(palm1, vec3(4.2f, 0.0f, -8.5f));
        palm1 = rotate(palm1, radians(-12.0f), vec3(0.0f, 1.0f, 0.0f));
        Tree::draw(shader, palm1, TREE_PALM);

        // Coconut Palm 2: Village riverbank south
        mat4 palm2 = mat4::identity();
        palm2 = translate(palm2, vec3(4.5f, 0.0f, 9.5f));
        palm2 = rotate(palm2, radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
        Tree::draw(shader, palm2, TREE_PALM);

        // Coconut Palm 3: Southwest courtyard boundary
        mat4 palm3 = mat4::identity();
        palm3 = translate(palm3, vec3(-12.5f, 0.0f, 4.5f));
        palm3 = rotate(palm3, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
        Tree::draw(shader, palm3, TREE_PALM);

        // Coconut Palm 4: Rear homestead garden
        mat4 palm4 = mat4::identity();
        palm4 = translate(palm4, vec3(-2.0f, 0.0f, -17.0f));
        palm4 = rotate(palm4, radians(-25.0f), vec3(0.0f, 1.0f, 0.0f));
        Tree::draw(shader, palm4, TREE_PALM);

        // Coconut Palm 5: Far riverbank north (across the river!)
        mat4 palm5 = mat4::identity();
        palm5 = translate(palm5, vec3(15.5f, 0.0f, -7.0f));
        palm5 = rotate(palm5, radians(20.0f), vec3(0.0f, 1.0f, 0.0f));
        Tree::draw(shader, palm5, TREE_PALM);

        // Coconut Palm 6: Far riverbank center (across the river!)
        mat4 palm6 = mat4::identity();
        palm6 = translate(palm6, vec3(16.8f, 0.0f, 1.5f));
        palm6 = rotate(palm6, radians(-30.0f), vec3(0.0f, 1.0f, 0.0f));
        Tree::draw(shader, palm6, TREE_PALM);

        // Coconut Palm 7: Far riverbank south (across the river!)
        mat4 palm7 = mat4::identity();
        palm7 = translate(palm7, vec3(16.0f, 0.0f, 9.0f));
        palm7 = rotate(palm7, radians(15.0f), vec3(0.0f, 1.0f, 0.0f));
        Tree::draw(shader, palm7, TREE_PALM);

        // Banana Trees: Kitchen garden grove
        mat4 banana1 = mat4::identity();
        banana1 = translate(banana1, vec3(-13.5f, 0.0f, -4.5f));
        Tree::draw(shader, banana1, TREE_BANANA);

        mat4 banana2 = mat4::identity();
        banana2 = translate(banana2, vec3(-12.8f, 0.0f, -7.5f));
        banana2 = rotate(banana2, radians(55.0f), vec3(0.0f, 1.0f, 0.0f));
        banana2 = scale(banana2, vec3(0.85f, 0.85f, 0.85f));
        Tree::draw(shader, banana2, TREE_BANANA);

        // Majestic branching Banyan / Mango Tree (homestead backdrop)
        mat4 mangoTree = mat4::identity();
        mangoTree = translate(mangoTree, vec3(-16.5f, 0.0f, -10.5f));
        Tree::draw(shader, mangoTree, TREE_GENERAL);

        // Second leafy Banyan / Mango Tree on the far bank of the river
        mat4 farTree = mat4::identity();
        farTree = translate(farTree, vec3(18.5f, 0.0f, 3.0f));
        farTree = scale(farTree, vec3(1.1f, 1.1f, 1.1f));
        Tree::draw(shader, farTree, TREE_GENERAL);

        // Foreground lush canopy framing the courtyard from the south
        mat4 southTree = mat4::identity();
        southTree = translate(southTree, vec3(-1.0f, 0.0f, 16.0f));
        southTree = scale(southTree, vec3(1.30f, 1.30f, 1.30f));
        Tree::draw(shader, southTree, TREE_GENERAL);

        // North leafy tree framing the river from the top
        mat4 northTree = mat4::identity();
        northTree = translate(northTree, vec3(1.0f, 0.0f, -17.5f));
        northTree = scale(northTree, vec3(1.25f, 1.25f, 1.25f));
        Tree::draw(shader, northTree, TREE_GENERAL);

        // Traditional Bamboo Grove (Bansher Jhar) behind homestead (far away from river!)
        mat4 bambooGrove = mat4::identity();
        bambooGrove = translate(bambooGrove, vec3(-19.0f, 0.0f, -14.0f));
        bambooGrove = rotate(bambooGrove, radians(30.0f), vec3(0.0f, 1.0f, 0.0f));
        Tree::draw(shader, bambooGrove, TREE_BAMBOO);

        // ─── 6. VILLAGE GATHERING (UTHAN) ───────────────────────
        // Traditional woven Charpai (bed)
        mat4 charpaiM = mat4::identity();
        charpaiM = translate(charpaiM, vec3(-3.8f, 0.0f, 0.8f));
        charpaiM = rotate(charpaiM, radians(-12.0f), vec3(0.0f, 1.0f, 0.0f));
        Charpai::draw(shader, charpaiM);

        // Seated Elder on the Charpai edge ("sit the human in the charpai properly")
        PersonParams elder;
        elder.skinColor   = vec3(0.52f, 0.35f, 0.22f);
        elder.shirtColor  = vec3(0.92f, 0.90f, 0.85f); // white cotton kurta
        elder.pantsColor  = vec3(0.55f, 0.14f, 0.08f); // maroon lungi
        elder.seated      = true;
        elder.isElder     = true;                      // dignified white hair and beard
        elder.hasGamcha   = true;                      // red gamcha over shoulder
        elder.gamchaColor = vec3(0.80f, 0.20f, 0.14f);
        elder.hasFan      = true;                      // holds traditional handmade fan (Haat Pakha) in hand!
        elder.fanSway     = sinf(time * 2.6f) * radians(8.0f); // gentle, lifelike fanning oscillation

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

        // Standing Villager 1 (elder chatting)
        PersonParams vp1;
        vp1.skinColor     = vec3(0.48f, 0.32f, 0.20f);
        vp1.shirtColor    = vec3(0.20f, 0.40f, 0.65f); // blue shirt
        vp1.pantsColor    = vec3(0.55f, 0.48f, 0.32f); // checkered lungi
        vp1.hasGamcha     = true;                      // gamcha on shoulder
        vp1.gamchaColor   = vec3(0.85f, 0.75f, 0.20f); // yellowish gamcha
        vp1.rightArmAngle = radians(18.0f);            // right arm gesturing politely forward
        mat4 p1M = mat4::identity();
        p1M = translate(p1M, vec3(-5.2f, 0.0f, 0.2f));
        p1M = rotate(p1M, radians(45.0f), vec3(0.0f, 1.0f, 0.0f)); // faces charpai
        Person::draw(shader, p1M, vp1);

        // Standing Villager 2 (listening)
        PersonParams vp2;
        vp2.skinColor     = vec3(0.52f, 0.36f, 0.22f);
        vp2.shirtColor    = vec3(0.78f, 0.72f, 0.58f); // cream panjabi
        vp2.pantsColor    = vec3(0.18f, 0.30f, 0.45f); // dark blue lungi
        mat4 p2M = mat4::identity();
        p2M = translate(p2M, vec3(-3.8f, 0.0f, 2.6f));
        p2M = rotate(p2M, radians(-30.0f), vec3(0.0f, 1.0f, 0.0f));
        Person::draw(shader, p2M, vp2);

        // Child sitting cross-legged on the swept yard reading
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

        // ─── 7. ANIMALS (HENS & DUCKS) ──────────────────────────
        // Hens wandering across the spacious courtyard
        mat4 hen1 = mat4::identity();
        hen1 = translate(hen1, vec3(-6.5f, 0.0f, -1.8f));
        hen1 = rotate(hen1, radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
        Hen::draw(shader, hen1);

        mat4 hen2 = mat4::identity();
        hen2 = translate(hen2, vec3(-5.0f, 0.0f, 3.2f));
        hen2 = rotate(hen2, radians(-80.0f), vec3(0.0f, 1.0f, 0.0f));
        Hen::draw(shader, hen2);

        mat4 hen3 = mat4::identity();
        hen3 = translate(hen3, vec3(-1.5f, 0.0f, -4.8f));
        hen3 = rotate(hen3, radians(125.0f), vec3(0.0f, 1.0f, 0.0f));
        Hen::draw(shader, hen3);

        // Ducks swimming gracefully on the river water
        float duckBob1 = sinf(time * 2.5f) * 0.02f;
        mat4 duck1 = mat4::identity();
        duck1 = translate(duck1, vec3(7.5f, 0.02f + duckBob1, -3.8f));
        duck1 = rotate(duck1, radians(25.0f), vec3(0.0f, 1.0f, 0.0f));
        Duck::draw(shader, duck1);

        float duckBob2 = cosf(time * 2.2f) * 0.02f;
        mat4 duck2 = mat4::identity();
        duck2 = translate(duck2, vec3(8.6f, 0.02f + duckBob2, -1.8f));
        duck2 = rotate(duck2, radians(-40.0f), vec3(0.0f, 1.0f, 0.0f));
        Duck::draw(shader, duck2);

        float duckBob3 = sinf(time * 2.0f + 1.0f) * 0.02f;
        mat4 duck3 = mat4::identity();
        duck3 = translate(duck3, vec3(9.2f, 0.02f + duckBob3, 5.5f));
        duck3 = rotate(duck3, radians(15.0f), vec3(0.0f, 1.0f, 0.0f));
        Duck::draw(shader, duck3);

        // ─── 8. TRADITIONAL DINGI NOUKA & MAJHI ──────────────────
        // Boat resting right at the natural sandy riverbank (ghat removed)
        // Elevated to 0.11m so floorboards (0.16m) stay safely above the river water surface (0.02m)
        float boatBob  = sinf(time * 1.6f) * 0.018f;
        float boatRoll = sinf(time * 1.4f) * radians(1.2f);

        mat4 boatM = mat4::identity();
        boatM = translate(boatM, vec3(7.2f, 0.11f + boatBob, 1.2f));
        boatM = rotate(boatM, radians(6.0f), vec3(0.0f, 1.0f, 0.0f));
        boatM = rotate(boatM, boatRoll, vec3(0.0f, 0.0f, 1.0f));
        Boat::draw(shader, boatM);

        // Boatman (Majhi) seated at the stern seat (local Y = 0.16f)
        float oarSway = sinf(time * 1.5f) * radians(8.0f);
        mat4 majhiM = boatM;
        majhiM = translate(majhiM, vec3(0.0f, 0.16f, -1.5f));
        Boatman::draw(shader, majhiM, oarSway);

        // Second smaller Dingi Nouka moored further upstream by the reeds
        float boatBob2 = cosf(time * 1.5f + 0.5f) * 0.018f;
        mat4 boat2 = mat4::identity();
        boat2 = translate(boat2, vec3(9.8f, 0.11f + boatBob2, -13.5f));
        boat2 = rotate(boat2, radians(-15.0f), vec3(0.0f, 1.0f, 0.0f));
        boat2 = scale(boat2, vec3(0.75f, 0.75f, 0.75f));
        Boat::draw(shader, boat2);

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