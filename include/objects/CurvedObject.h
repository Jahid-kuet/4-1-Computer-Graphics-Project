// CurvedObject.h — Procedural Curved 3D Objects for OpenGL 3.3 Core Profile.
// Features:
// 1. Parametric Cubic Bézier Surface of Revolution (Traditional Terracotta Vase / Matir Surahi)
// 2. Traditional Curved Arched Bamboo Footbridge (Bansher Saako / বাঁশের সাঁকো)
#pragma once

#include "Shader.h"
#include "mathutil.h"

namespace CurvedObject {
    // Generate curved mesh GPU buffers (VAO/VBO/EBO) for Bézier surface. Call once at startup.
    void init();

    // Release GPU buffers at application exit.
    void cleanup();

    // 1. Draw smooth Cubic Bézier Surface of Revolution (Terracotta Vase / Surahi)
    // Modeled by revolving a cubic Bézier profile curve 360 degrees around the Y-axis.
    void drawBezierVase(Shader& shader, const math::mat4& model, const math::vec3& color = math::vec3(0.72f, 0.38f, 0.22f));

    // 2. Draw traditional Curved Arched Bamboo Footbridge (Bansher Saako / বাঁশের সাঁকো)
    // Spans across river inlet with arched bamboo walkway, curved cross-slats,
    // curved double handrails, and cross-braced diagonal bamboo stilts.
    void drawBambooBridge(Shader& shader, const math::mat4& model);
}
