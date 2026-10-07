// RayTracer.h — Real-Time Whitted Ray Tracing Engine for OpenGL 3.3 Core Profile.
// Features:
// 1. Ray Generation (Primary Camera Rays from arbitrary view/projection).
// 2. Analytic Ray-Sphere, Ray-Plane, and Ray-Box Intersections.
// 3. Shadow Rays: Hard ray-traced shadows from Directional & Point light sources.
// 4. Specular Reflection Rays: Multi-bounce mirror reflections (water mirror & polished spheres).
// 5. Emissive light sources (Moon, Lantern flame) and Blinn-Phong local illumination.
#pragma once

#include <glad/glad.h>
#include "Camera.h"
#include "Shader.h"

namespace RayTracer {
    // Initialize ray tracer GPU buffers and compile GLSL ray tracing shader
    void init();

    // Release GPU buffers and shaders
    void cleanup();

    // Render full-screen real-time ray-traced view
    void render(int screenWidth, int screenHeight, const Camera& camera, float animTime, int lightingMode);

    // Check if ray tracer is successfully initialized
    bool isInitialized();
}
