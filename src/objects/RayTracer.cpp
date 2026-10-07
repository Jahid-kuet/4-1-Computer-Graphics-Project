// RayTracer.cpp — Real-Time Whitted Ray Tracing Implementation in GLSL 330 Core.
#include "objects/RayTracer.h"
#include <iostream>

namespace RayTracer {

static GLuint s_quadVAO = 0;
static GLuint s_quadVBO = 0;
static GLuint s_rayProgram = 0;
static bool   s_initialized = false;

// ═══════════════════════════════════════════════════════════════
// Full-Screen Quad Vertex Shader (GLSL 330 Core)
// ═══════════════════════════════════════════════════════════════
static const char* s_quadVertSrc = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
out vec2 TexCoord;

void main() {
    TexCoord = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

// ═══════════════════════════════════════════════════════════════
// Whitted-Style Ray Tracing Fragment Shader (GLSL 330 Core)
// Computes:
// 1. Primary Camera Ray generation
// 2. Analytic Ray-Sphere, Ray-Plane, and Ray-Box Intersections
// 3. Shadow Rays for Directional Sun/Moonlight & Point Lantern Light
// 4. Recursive/Iterative Specular Reflection Rays (Mirror Reflections)
// 5. Blinn-Phong Ambient, Diffuse, and Specular Shading
// ═══════════════════════════════════════════════════════════════
static const char* s_rayFragSrc = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform vec3  uCamPos;
uniform vec3  uCamTarget;
uniform vec3  uCamUp;
uniform float uFov;
uniform vec2  uResolution;
uniform float uTime;
uniform int   uLightingMode; // 0 = Moonlit Night, 1 = Daylight Sun

// ── Ray & Material Data Structures ──
struct Ray {
    vec3 origin;
    vec3 dir;
};

struct Material {
    vec3  albedo;
    float specular;
    float shininess;
    float reflectivity;
    float emissive;
};

struct HitInfo {
    float    t;
    vec3     pos;
    vec3     normal;
    Material mat;
    int      hitObj;
};

const float INF = 1e8;
const float EPS = 0.0015;

// ── Analytic Geometric Intersections ──

// 1. Ray - Sphere Intersection: |O + t*D - C|^2 = R^2
bool IntersectSphere(Ray r, vec3 center, float radius, Material mat, int objId, inout HitInfo bestHit) {
    vec3 oc = r.origin - center;
    float b = dot(oc, r.dir);
    float c = dot(oc, oc) - radius * radius;
    float disc = b * b - c;
    if (disc < 0.0) return false;

    float sqrtD = sqrt(disc);
    float t1 = -b - sqrtD;
    float t2 = -b + sqrtD;
    float t = (t1 > EPS) ? t1 : ((t2 > EPS) ? t2 : -1.0);
    if (t > EPS && t < bestHit.t) {
        bestHit.t = t;
        bestHit.pos = r.origin + t * r.dir;
        bestHit.normal = normalize(bestHit.pos - center);
        bestHit.mat = mat;
        bestHit.hitObj = objId;
        return true;
    }
    return false;
}

// 2. Ray - Horizontal Plane Intersection at y = height
bool IntersectWaterPlane(Ray r, float height, inout HitInfo bestHit) {
    if (abs(r.dir.y) < 1e-5) return false;
    float t = (height - r.origin.y) / r.dir.y;
    if (t > EPS && t < bestHit.t) {
        vec3 hitPos = r.origin + t * r.dir;
        // Restrict plane to river region
        if (abs(hitPos.x) < 45.0 && abs(hitPos.z) < 45.0) {
            bestHit.t = t;
            bestHit.pos = hitPos;

            // Procedural gentle water ripples for perturbed surface normal
            float w1 = sin(hitPos.x * 2.5 + uTime * 2.2);
            float w2 = cos(hitPos.z * 3.0 + uTime * 1.8);
            vec3 pNorm = normalize(vec3(w1 * 0.035, 1.0, w2 * 0.035));
            bestHit.normal = (r.dir.y < 0.0) ? pNorm : -pNorm;

            // Water material: dark river mirror with high reflectivity
            Material mat;
            if (uLightingMode == 0) {
                mat.albedo = vec3(0.02, 0.06, 0.12); // Midnight deep river
            } else {
                mat.albedo = vec3(0.12, 0.38, 0.52); // Daylight river
            }
            mat.specular     = 0.95;
            mat.shininess    = 128.0;
            mat.reflectivity = 0.55;
            mat.emissive     = 0.0;
            bestHit.mat = mat;
            bestHit.hitObj = 100;
            return true;
        }
    }
    return false;
}

// 3. Ray - Axis-Aligned Bounding Box (Slab Method)
bool IntersectBox(Ray r, vec3 boxMin, vec3 boxMax, Material mat, int objId, inout HitInfo bestHit) {
    vec3 invD = 1.0 / r.dir;
    vec3 t0 = (boxMin - r.origin) * invD;
    vec3 t1 = (boxMax - r.origin) * invD;
    vec3 tmin = min(t0, t1);
    vec3 tmax = max(t0, t1);

    float nearT = max(max(tmin.x, tmin.y), tmin.z);
    float farT  = min(min(tmax.x, tmax.y), tmax.z);

    if (nearT > farT || farT < EPS) return false;
    float t = (nearT > EPS) ? nearT : farT;
    if (t > EPS && t < bestHit.t) {
        bestHit.t = t;
        bestHit.pos = r.origin + t * r.dir;

        // Determine surface normal on hit face
        vec3 c = (boxMin + boxMax) * 0.5;
        vec3 p = bestHit.pos - c;
        vec3 d = abs(boxMin - boxMax) * 0.5;
        vec3 norm = vec3(0.0);
        float bias = 1.001;
        if (abs(p.x) > d.x * (1.0 - 0.002)) norm = vec3(sign(p.x), 0.0, 0.0);
        else if (abs(p.y) > d.y * (1.0 - 0.002)) norm = vec3(0.0, sign(p.y), 0.0);
        else if (abs(p.z) > d.z * (1.0 - 0.002)) norm = vec3(0.0, 0.0, sign(p.z));
        else norm = vec3(0.0, 1.0, 0.0);

        bestHit.normal = norm;
        bestHit.mat = mat;
        bestHit.hitObj = objId;
        return true;
    }
    return false;
}

// ── Complete Scene Intersection ──
bool IntersectScene(Ray r, inout HitInfo bestHit) {
    bestHit.t = INF;
    bestHit.hitObj = -1;

    // 1. Water Plane (River mirror)
    IntersectWaterPlane(r, 0.0, bestHit);

    // 2. The Celestial Moon (Emissive source)
    Material moonMat;
    moonMat.albedo       = vec3(0.96, 0.98, 1.00);
    moonMat.specular     = 0.0;
    moonMat.shininess    = 1.0;
    moonMat.reflectivity = 0.0;
    moonMat.emissive     = 1.0;
    IntersectSphere(r, vec3(18.0, 14.0, -28.0), 3.2, moonMat, 1, bestHit);

    // 3. Terracotta Clay Pot / Sphere (Matir Kolshi)
    Material terraMat;
    terraMat.albedo       = vec3(0.78, 0.38, 0.18);
    terraMat.specular     = 0.35;
    terraMat.shininess    = 32.0;
    terraMat.reflectivity = 0.05;
    terraMat.emissive     = 0.0;
    IntersectSphere(r, vec3(0.0, 1.5, 0.0), 1.5, terraMat, 2, bestHit);

    // 4. Polished Brass / Metal Sphere (Pittol / Kasha mirror ball with high reflection)
    Material brassMat;
    brassMat.albedo       = vec3(0.92, 0.82, 0.45);
    brassMat.specular     = 0.95;
    brassMat.shininess    = 128.0;
    brassMat.reflectivity = 0.78; // Sharp mirror reflections of scene
    brassMat.emissive     = 0.0;
    IntersectSphere(r, vec3(3.6, 1.2, 2.8), 1.2, brassMat, 3, bestHit);

    // 5. Emerald Jadeite Glass Sphere
    Material emeraldMat;
    emeraldMat.albedo       = vec3(0.18, 0.68, 0.32);
    emeraldMat.specular     = 0.85;
    emeraldMat.shininess    = 64.0;
    emeraldMat.reflectivity = 0.35;
    emeraldMat.emissive     = 0.0;
    IntersectSphere(r, vec3(-3.2, 1.0, 2.2), 1.0, emeraldMat, 4, bestHit);

    // 6. Traditional Wood Cottage Base (Plinth / Dochala Box)
    Material woodMat;
    woodMat.albedo       = vec3(0.42, 0.28, 0.15);
    woodMat.specular     = 0.20;
    woodMat.shininess    = 16.0;
    woodMat.reflectivity = 0.02;
    woodMat.emissive     = 0.0;
    IntersectBox(r, vec3(-8.5, 0.0, -4.5), vec3(-5.5, 2.4, -1.5), woodMat, 5, bestHit);

    // 7. Hurricane Lantern Flame (Point Light Source Sphere)
    float lanternBob = sin(uTime * 2.4) * 0.12;
    vec3 lanternPos = vec3(-1.4, 1.6 + lanternBob, 1.2);
    Material lanternMat;
    lanternMat.albedo       = vec3(1.0, 0.78, 0.35);
    lanternMat.specular     = 0.0;
    lanternMat.shininess    = 1.0;
    lanternMat.reflectivity = 0.0;
    lanternMat.emissive     = 1.0;
    IntersectSphere(r, lanternPos, 0.24, lanternMat, 6, bestHit);

    return (bestHit.t < INF);
}

// ── Shadow Ray Check: Returns true if ray to light is obstructed ──
bool InShadow(vec3 p, vec3 lightDir, float maxDist) {
    Ray shadowRay;
    shadowRay.origin = p + lightDir * (EPS * 3.0);
    shadowRay.dir = lightDir;
    HitInfo sHit;
    if (IntersectScene(shadowRay, sHit)) {
        if (sHit.t < maxDist && sHit.mat.emissive < 0.5) {
            return true;
        }
    }
    return false;
}

// ── Sky Dome / Environmental Backdrop ──
vec3 GetSkyColor(vec3 dir) {
    if (uLightingMode == 0) {
        // Deep nocturnal indigo sky with celestial star sparkles
        float t = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);
        vec3 nightHorizon = vec3(0.03, 0.05, 0.12);
        vec3 nightZenith  = vec3(0.01, 0.02, 0.05);
        vec3 sky = mix(nightHorizon, nightZenith, t);
        // Star sparkle shimmer
        float starSeed = sin(dot(floor(dir * 120.0), vec3(12.9898, 78.233, 45.164))) * 43758.5453;
        float star = step(0.996, fract(starSeed));
        return sky + vec3(star * 0.75);
    } else {
        // Radiant golden-blue daylight sky
        float t = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);
        return mix(vec3(0.72, 0.84, 0.94), vec3(0.24, 0.52, 0.88), t);
    }
}

// ── Whitted Ray Tracing Main Function ──
vec3 TraceRay(Ray r) {
    vec3 accumulatedColor = vec3(0.0);
    vec3 throughput = vec3(1.0);

    // Light source setup
    vec3 dirLightDir;
    vec3 dirLightColor;
    if (uLightingMode == 0) {
        dirLightDir   = normalize(vec3(-0.45, 0.65, -0.60)); // Moonbeam
        dirLightColor = vec3(0.65, 0.78, 0.95);
    } else {
        dirLightDir   = normalize(vec3(0.40, 0.85, 0.35));  // Sunlight
        dirLightColor = vec3(1.00, 0.95, 0.85);
    }

    float lanternBob = sin(uTime * 2.4) * 0.12;
    vec3 ptLightPos = vec3(-1.4, 1.6 + lanternBob, 1.2);
    vec3 ptLightCol = vec3(1.0, 0.75, 0.32);

    // Multi-bounce ray tracing loop (up to 3 specular reflection bounces)
    for (int bounce = 0; bounce < 3; ++bounce) {
        HitInfo hit;
        if (!IntersectScene(r, hit)) {
            accumulatedColor += throughput * GetSkyColor(r.dir);
            break;
        }

        // Emissive light source hit directly
        if (hit.mat.emissive > 0.5) {
            accumulatedColor += throughput * hit.mat.albedo * 1.5;
            break;
        }

        vec3 N = hit.normal;
        vec3 V = -r.dir;

        // Ambient lighting term
        vec3 ambient = (uLightingMode == 0 ? 0.08 : 0.22) * hit.mat.albedo;

        // ── 1. Directional Light (Sun / Moon) with Hard Shadow Ray ──
        vec3 dirDiffuse = vec3(0.0);
        vec3 dirSpec = vec3(0.0);
        if (!InShadow(hit.pos, dirLightDir, INF)) {
            float NdotL = max(dot(N, dirLightDir), 0.0);
            dirDiffuse = dirLightColor * hit.mat.albedo * NdotL;

            vec3 H = normalize(dirLightDir + V);
            float NdotH = max(dot(N, H), 0.0);
            dirSpec = dirLightColor * hit.mat.specular * pow(NdotH, hit.mat.shininess);
        }

        // ── 2. Local Positional Point Light (Hurricane Lantern) with Shadow Ray ──
        vec3 ptDiffuse = vec3(0.0);
        vec3 ptSpec = vec3(0.0);
        vec3 toPt = ptLightPos - hit.pos;
        float ptDist = length(toPt);
        if (ptDist > 0.01) {
            vec3 ptDir = normalize(toPt);
            if (!InShadow(hit.pos, ptDir, ptDist)) {
                float att = 1.0 / (1.0 + 0.15 * ptDist + 0.035 * ptDist * ptDist);
                float NdotL = max(dot(N, ptDir), 0.0);
                ptDiffuse = ptLightCol * hit.mat.albedo * NdotL * att * 1.8;

                vec3 H = normalize(ptDir + V);
                float NdotH = max(dot(N, H), 0.0);
                ptSpec = ptLightCol * hit.mat.specular * pow(NdotH, hit.mat.shininess) * att * 1.8;
            }
        }

        vec3 directLighting = ambient + dirDiffuse + dirSpec + ptDiffuse + ptSpec;

        // Accumulate direct lighting weighted by current ray throughput and material non-reflective portion
        float refl = hit.mat.reflectivity;
        accumulatedColor += throughput * directLighting * (1.0 - refl);

        // If surface has no reflection, terminate ray
        if (refl <= 0.01) {
            break;
        }

        // Compute Reflection Ray for next bounce: R = reflect(D, N)
        throughput *= hit.mat.albedo * refl;
        r.origin = hit.pos + N * (EPS * 3.0);
        r.dir = normalize(reflect(r.dir, N));
    }

    return accumulatedColor;
}

void main() {
    // Normalized Screen Coordinates [-1, 1] adjusted for Aspect Ratio
    vec2 uv = (gl_FragCoord.xy - 0.5 * uResolution) / uResolution.y;

    // Camera Basis Vectors (Forward, Right, Up)
    vec3 forward = normalize(uCamTarget - uCamPos);
    vec3 right   = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));
    vec3 up      = cross(right, forward);

    // Primary Ray from Camera through pixel
    float fovFactor = tan(radians(uFov) * 0.5);
    vec3 rayDir = normalize(forward + (uv.x * right + uv.y * up) * fovFactor);

    Ray primaryRay;
    primaryRay.origin = uCamPos;
    primaryRay.dir = rayDir;

    // Execute Whitted Ray Tracing
    vec3 col = TraceRay(primaryRay);

    // Gamma tone mapping
    col = pow(col, vec3(1.0 / 1.15));

    FragColor = vec4(col, 1.0);
}
)";

void init() {
    if (s_initialized) return;

    // Compile Ray Tracing Shader Program
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &s_quadVertSrc, nullptr);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &s_rayFragSrc, nullptr);
    glCompileShader(frag);

    GLint success;
    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetShaderInfoLog(frag, 1024, nullptr, log);
        std::cerr << "RayTracer Shader Error: " << log << std::endl;
    }

    s_rayProgram = glCreateProgram();
    glAttachShader(s_rayProgram, vert);
    glAttachShader(s_rayProgram, frag);
    glLinkProgram(s_rayProgram);

    glDeleteShader(vert);
    glDeleteShader(frag);

    // Fullscreen Quad VAO/VBO
    static const float quadVertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,
    };

    glGenVertexArrays(1, &s_quadVAO);
    glGenBuffers(1, &s_quadVBO);

    glBindVertexArray(s_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, s_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    s_initialized = true;
    std::cout << ">> [RAY TRACING ENGINE] Whitted Ray Tracer initialized successfully (Primary, Shadow & Reflection Rays ready).\n";
}

void cleanup() {
    if (s_quadVAO) { glDeleteVertexArrays(1, &s_quadVAO); s_quadVAO = 0; }
    if (s_quadVBO) { glDeleteBuffers(1, &s_quadVBO); s_quadVBO = 0; }
    if (s_rayProgram) { glDeleteProgram(s_rayProgram); s_rayProgram = 0; }
    s_initialized = false;
}

void render(int screenWidth, int screenHeight, const Camera& camera, float animTime, int lightingMode) {
    if (!s_initialized) init();

    glDisable(GL_DEPTH_TEST);
    glUseProgram(s_rayProgram);

    glUniform3f(glGetUniformLocation(s_rayProgram, "uCamPos"), camera.position.x, camera.position.y, camera.position.z);
    glUniform3f(glGetUniformLocation(s_rayProgram, "uCamTarget"), camera.target.x, camera.target.y, camera.target.z);
    glUniform3f(glGetUniformLocation(s_rayProgram, "uCamUp"), 0.0f, 1.0f, 0.0f);
    float fovDeg = camera.fov * (180.0f / 3.14159265f);
    glUniform1f(glGetUniformLocation(s_rayProgram, "uFov"), fovDeg);
    glUniform2f(glGetUniformLocation(s_rayProgram, "uResolution"), (float)screenWidth, (float)screenHeight);
    glUniform1f(glGetUniformLocation(s_rayProgram, "uTime"), animTime);
    glUniform1i(glGetUniformLocation(s_rayProgram, "uLightingMode"), (lightingMode == 0) ? 0 : 1);

    glBindVertexArray(s_quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glUseProgram(0);
    glEnable(GL_DEPTH_TEST);
}

bool isInitialized() {
    return s_initialized;
}

} // namespace RayTracer
