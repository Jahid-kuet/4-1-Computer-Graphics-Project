// Shader.cpp — Shader compilation, linking, and built-in Phong GLSL sources.

#include "Shader.h"
#include <glad/glad.h>
#include <iostream>

// ═══════════════════════════════════════════════════════════════
// Built-in Phong Shader — vertex
// ═══════════════════════════════════════════════════════════════
const char* Shader::phongVertexSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 ObjPos;
out vec2 TexCoord;
out vec3 GouraudDiff;
out vec3 GouraudSpec;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// ═══════════════════════════════════════════════════════════════════
// LAB REQUIREMENT: GOURAUD SHADING (PER-VERTEX LIGHTING EVALUATION)
// 0 = Phong Shading (Per-Fragment Lighting)
// 1 = Gouraud Shading (Per-Vertex Lighting, Linear Raster Interpolation)
// ═══════════════════════════════════════════════════════════════════
uniform int   uShadingModel;

uniform vec3  lightDir;
uniform vec3  lightColor;
uniform vec3  viewPos;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

uniform int   dirLightEnabled;
uniform int   pointLightsEnabled;
uniform int   hardLightMode;

// 6 Positional Point Lights
uniform vec3  pointLightPos;
uniform vec3  pointLightColor;
uniform float pointLightIntensity;

uniform vec3  pointLight2Pos;
uniform vec3  pointLight2Color;
uniform float pointLight2Intensity;

uniform vec3  pointLight3Pos;
uniform vec3  pointLight3Color;
uniform float pointLight3Intensity;

uniform vec3  pointLight4Pos;
uniform vec3  pointLight4Color;
uniform float pointLight4Intensity;

uniform vec3  pointLight5Pos;
uniform vec3  pointLight5Color;
uniform float pointLight5Intensity;

uniform vec3  pointLight6Pos;
uniform vec3  pointLight6Color;
uniform float pointLight6Intensity;

void CalcGouraudPointLight(vec3 pPos, vec3 pColor, float pIntensity, float maxRadius,
                           float constAtt, float linAtt, float quadAtt,
                           vec3 norm, vec3 vPos, vec3 vDir,
                           inout vec3 diffSum, inout vec3 specSum)
{
    if (pointLightsEnabled == 0 || pIntensity <= 0.001) return;
    vec3 pVec = pPos - vPos;
    float pDist = length(pVec);
    if (pDist >= maxRadius) return;

    vec3 pDir = normalize(pVec);
    float pDiff = max(dot(norm, pDir), 0.0);
    if (hardLightMode == 0) {
        pDiff = pDiff * 0.72 + 0.28 * max(norm.y, 0.0);
    }

    vec3 pHalf = normalize(pDir + vDir);
    float pShininess = (hardLightMode == 1) ? (shininess * 1.8) : shininess;
    float pSpec = pow(max(dot(norm, pHalf), 0.0), pShininess);

    float pDistEff = max(pDist, 0.8);
    float win = clamp(1.0 - (pDist / maxRadius) * (pDist / maxRadius), 0.0, 1.0);
    win = win * win;
    float att = win / (constAtt + linAtt * pDistEff + quadAtt * pDistEff * pDistEff);

    diffSum  += pDiff * pColor * pIntensity * att;
    specSum  += specularStrength * pSpec * pColor * pIntensity * att;
}

// ═══════════════════════════════════════════════════════════════════
// LAB TOPIC 1 & 5: COORDINATE PIPELINE & NORMAL TRANSFORMATION
// ═══════════════════════════════════════════════════════════════════
void main()
{
    FragPos     = vec3(model * vec4(aPos, 1.0));
    Normal      = mat3(transpose(inverse(model))) * aNormal;
    ObjPos      = aPos;
    TexCoord    = vec2(aPos.x + aPos.z * 0.5, aPos.y);
    gl_Position = projection * view * vec4(FragPos, 1.0);

    // ── Gouraud Per-Vertex Lighting Evaluation ──
    if (uShadingModel == 1)
    {
        vec3 norm    = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);

        vec3 dirAmbient  = vec3(0.0);
        vec3 dirDiffuse  = vec3(0.0);
        vec3 dirSpecular = vec3(0.0);

        if (dirLightEnabled != 0) {
            vec3 lightD = normalize(-lightDir);
            if (hardLightMode == 1) {
                dirAmbient = (ambientStrength * 0.45) * lightColor;
                float diff = max(dot(norm, lightD), 0.0);
                dirDiffuse = pow(diff, 1.35) * lightColor;
                vec3 halfDir = normalize(lightD + viewDir);
                float spec = pow(max(dot(norm, halfDir), 0.0), shininess * 2.0);
                dirSpecular = (specularStrength * 1.5) * spec * lightColor;
            } else {
                dirAmbient = (ambientStrength * 1.15) * lightColor;
                float diff = max(dot(norm, lightD), 0.0);
                float softDiff = diff * 0.75 + 0.25 * max(norm.y, 0.0);
                dirDiffuse = softDiff * lightColor;
                vec3 halfDir = normalize(lightD + viewDir);
                float spec = pow(max(dot(norm, halfDir), 0.0), shininess);
                dirSpecular = specularStrength * spec * lightColor;
            }
        }

        vec3 ptDiff = vec3(0.0);
        vec3 ptSpec = vec3(0.0);
        CalcGouraudPointLight(pointLightPos,  pointLightColor,  pointLightIntensity,  24.0, 1.0, 0.15, 0.025, norm, FragPos, viewDir, ptDiff, ptSpec);
        CalcGouraudPointLight(pointLight2Pos, pointLight2Color, pointLight2Intensity, 18.0, 1.0, 0.18, 0.030, norm, FragPos, viewDir, ptDiff, ptSpec);
        CalcGouraudPointLight(pointLight3Pos, pointLight3Color, pointLight3Intensity, 20.0, 1.0, 0.18, 0.030, norm, FragPos, viewDir, ptDiff, ptSpec);
        CalcGouraudPointLight(pointLight4Pos, pointLight4Color, pointLight4Intensity, 15.0, 1.0, 0.22, 0.040, norm, FragPos, viewDir, ptDiff, ptSpec);
        CalcGouraudPointLight(pointLight5Pos, pointLight5Color, pointLight5Intensity, 18.0, 1.0, 0.18, 0.030, norm, FragPos, viewDir, ptDiff, ptSpec);
        CalcGouraudPointLight(pointLight6Pos, pointLight6Color, pointLight6Intensity, 18.0, 1.0, 0.18, 0.030, norm, FragPos, viewDir, ptDiff, ptSpec);

        GouraudDiff = dirAmbient + dirDiffuse + ptDiff;
        GouraudSpec = dirSpecular + ptSpec;
    }
    else
    {
        GouraudDiff = vec3(0.0);
        GouraudSpec = vec3(0.0);
    }
}
)";

// ═══════════════════════════════════════════════════════════════
// Built-in Phong Shader — fragment
// ═══════════════════════════════════════════════════════════════
const char* Shader::phongFragmentSrc = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 ObjPos;
in vec2 TexCoord;
in vec3 GouraudDiff;
in vec3 GouraudSpec;

uniform int   uShadingModel;   // 0 = Phong Shading (Per-Fragment), 1 = Gouraud Shading (Per-Vertex)
uniform vec3  objectColor;
uniform vec3  lightDir;        // direction FROM light source
uniform vec3  lightColor;
uniform vec3  viewPos;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;
uniform float emissive;        // 1.0 = fully emissive (light inside object), 0.0 = normal Phong

// Procedural and GPU Texture Mapping controls
uniform sampler2D uTexture;
uniform int       uUseTexture;    // 0 = Off (Solid Color), 1 = Procedural GLSL Detailing, 2 = Sample GPU Texture Map
uniform int       uTextureType;   // 0 = Wood, 1 = Brick, 2 = Bamboo, 3 = Fabric

// Directional and Point Light master toggles
uniform int   dirLightEnabled;    // 1 = Dir Light ON, 0 = Dir Light OFF
uniform int   pointLightsEnabled;  // 1 = Point Lights ON, 0 = Point Lights OFF

// LAB TOPIC 3: HARD LIGHT VS SOFT LIGHT TOGGLE
// 0 = Soft Light (high ambient, wide wrap diffuse, gentle specular, multiple soft lights)
// 1 = Hard Light (low ambient, sharp terminator, focused specular, harsh directional contrast)
uniform int   hardLightMode;

// 6 Positional Point Lights (Courtyard, Moored Boat, Mosque, Stove, Ghat, Cruising Boat)
uniform vec3  pointLightPos;
uniform vec3  pointLightColor;
uniform float pointLightIntensity;

uniform vec3  pointLight2Pos;
uniform vec3  pointLight2Color;
uniform float pointLight2Intensity;

uniform vec3  pointLight3Pos;
uniform vec3  pointLight3Color;
uniform float pointLight3Intensity;

uniform vec3  pointLight4Pos;
uniform vec3  pointLight4Color;
uniform float pointLight4Intensity;

uniform vec3  pointLight5Pos;
uniform vec3  pointLight5Color;
uniform float pointLight5Intensity;

uniform vec3  pointLight6Pos;
uniform vec3  pointLight6Color;
uniform float pointLight6Intensity;

// Distance fog for atmospheric depth
uniform vec3  fogColor;
uniform float fogDensity;

// Lighting bypass for object modeling inspection
uniform int   noLighting; // 1 = Crisp unlit 3D facets, 2 = Pure flat color, 0 = Full Phong lighting

// Procedural hash for micro-grain detailing
float hash21(vec2 p) {
    p = fract(p * vec2(234.34, 435.345));
    p += dot(p, p + 34.23);
    return fract(p.x * p.y);
}

// Procedural GLSL surface detail generator
vec3 CalcProceduralDetail(vec3 baseCol, vec3 objP, vec3 worldP, vec3 norm, int texType)
{
    vec3 an = abs(norm);
    vec2 pUV = (an.y > an.x && an.y > an.z) ? worldP.xz : ((an.x > an.z) ? worldP.zy : worldP.xy);

    if (texType == 0) // Wood grain & tree growth rings
    {
        float grain = sin(worldP.y * 36.0 + sin(worldP.x * 14.0 + worldP.z * 14.0) * 2.2);
        grain = 0.5 + 0.5 * grain;
        float fine = sin(worldP.y * 130.0) * 0.12;
        return baseCol * (0.82 + 0.32 * grain + fine);
    }
    else if (texType == 1) // Clay brick & mortar joints
    {
        vec2 bCoord = pUV * 4.5;
        int row = int(floor(bCoord.y));
        float xOff = (row % 2 != 0) ? 0.5 : 0.0;
        vec2 cell = fract(vec2(bCoord.x + xOff, bCoord.y));
        bool isMortar = (cell.x < 0.07 || cell.y < 0.10);
        float bioNoise = sin(worldP.x * 2.8 + sin(worldP.y * 3.4) * 1.8) * cos(worldP.z * 2.6 + worldP.y * 1.5);
        bioNoise = 0.5 + 0.5 * bioNoise;
        float heightDamp = clamp(1.0 - (worldP.y - 0.2) / 3.8, 0.0, 1.0);
        float mossFactor = clamp((bioNoise * 0.70 + heightDamp * 0.60 - 0.35) * 1.8, 0.0, 1.0);

        vec3 brickColor = baseCol * (0.89 + hash21(floor(vec2(bCoord.x + xOff, bCoord.y))) * 0.22);
        vec3 mortarColor = vec3(0.70, 0.68, 0.62);

        if (isMortar) {
            vec3 algaeMortar = mix(mortarColor, vec3(0.22, 0.36, 0.16), clamp(heightDamp * 0.85 + bioNoise * 0.30, 0.0, 1.0));
            return algaeMortar;
        } else {
            vec3 mossColor = vec3(0.22, 0.36, 0.15);
            vec3 slimeColor = vec3(0.11, 0.18, 0.08);
            vec3 bioCol = mix(mossColor, slimeColor, bioNoise);
            return mix(brickColor, bioCol, mossFactor * 0.75);
        }
    }
    else if (texType == 2) // Bamboo weave
    {
        vec2 bUv = pUV * 6.5;
        int uB = int(floor(bUv.x + bUv.y));
        int vB = int(floor(bUv.x - bUv.y + 100.0));
        bool weft = ((uB + vB) % 2 == 0);
        float split = fract(bUv.x + bUv.y);
        float fiber = 0.84 + 0.16 * sin(split * 3.14159);
        return baseCol * (weft ? 1.08 : 0.84) * fiber;
    }
    else if (texType == 3) // Lungi / gamcha plaid
    {
        vec2 fCoord = fract(pUV * 8.0);
        bool xS = (fCoord.x < 0.22);
        bool yS = (fCoord.y < 0.22);
        if (xS && yS) return baseCol * 0.45;
        if (xS || yS) return baseCol * 0.78;
        return baseCol * 1.15;
    }
    return baseCol;
}

vec2 GetTexCoords(vec3 objP, vec3 worldP, vec3 norm, int texType)
{
    vec3 an = abs(norm);
    if (texType == 1 && an.y < 0.85 && abs(objP.y) > 0.01) {
        float ang = atan(objP.z, objP.x) / 6.2831853 + 0.5;
        return vec2(ang * 4.0, objP.y * 3.0);
    }
    if (an.y > an.x && an.y > an.z) {
        return worldP.xz * 1.25;
    } else if (an.x > an.z) {
        return worldP.zy * 1.25;
    } else {
        return worldP.xy * 1.25;
    }
}

// ═══════════════════════════════════════════════════════════════════
// LAB TOPIC 2 & 3: PHONG LIGHTING (AMBIENT, DIFFUSE, SPECULAR) & HARD/SOFT LIGHT
// ═══════════════════════════════════════════════════════════════════
void CalcDirLightComponents(vec3 normal, vec3 viewDir, out vec3 ambientOut, out vec3 diffuseOut, out vec3 specularOut)
{
    ambientOut  = vec3(0.0);
    diffuseOut  = vec3(0.0);
    specularOut = vec3(0.0);

    if (dirLightEnabled == 0) return;
    vec3 lightD = normalize(-lightDir);

    if (hardLightMode == 1) {
        // ── HARD LIGHT MODE ──
        // Low ambient, sharp Lambertian cosine cutoff (hard terminator), high specular concentration
        float hardAmbientStrength = ambientStrength * 0.45;
        ambientOut = hardAmbientStrength * lightColor;

        float diff = max(dot(normal, lightD), 0.0);
        // Sharp step-down terminator
        float hardDiff = pow(diff, 1.35);
        diffuseOut = hardDiff * lightColor;

        vec3 halfDir = normalize(lightD + viewDir);
        float spec = pow(max(dot(normal, halfDir), 0.0), shininess * 2.0);
        specularOut = (specularStrength * 1.5) * spec * lightColor;
    } else {
        // ── SOFT LIGHT MODE (Default) ──
        // High ambient floor, soft organic wrap-around diffuse, gentle specular
        float softAmbientStrength = ambientStrength * 1.15;
        ambientOut = softAmbientStrength * lightColor;

        float diff = max(dot(normal, lightD), 0.0);
        // Wrap-around diffuse softens shadows
        float softDiff = diff * 0.75 + 0.25 * max(normal.y, 0.0);
        diffuseOut = softDiff * lightColor;

        vec3 halfDir = normalize(lightD + viewDir);
        float spec = pow(max(dot(normal, halfDir), 0.0), shininess);
        specularOut = specularStrength * spec * lightColor;
    }
}

// ═══════════════════════════════════════════════════════════════════
// LAB TOPIC 4: "LIGHT INSIDE OBJECT" VS "OBJECT INSIDE LIGHT"
// Attenuation = 1 / (Kc + Kl*d + Kq*d^2) applied to all external objects
// ═══════════════════════════════════════════════════════════════════
void CalcPointLightComponents(vec3 pPos, vec3 pColor, float pIntensity, float maxRadius,
                              float constAtt, float linAtt, float quadAtt,
                              vec3 normal, vec3 fragPos, vec3 viewDir,
                              inout vec3 diffuseSum, inout vec3 specularSum)
{
    if (pointLightsEnabled == 0 || pIntensity <= 0.001) return;
    vec3 pVec = pPos - fragPos;
    float pDist = length(pVec);
    if (pDist >= maxRadius) return;

    vec3 pDir = normalize(pVec);

    // Diffuse component
    float pDiff = max(dot(normal, pDir), 0.0);
    if (hardLightMode == 0) {
        pDiff = pDiff * 0.72 + 0.28 * max(normal.y, 0.0); // soft wrap
    }

    // Specular component (Blinn-Phong)
    vec3 pHalf = normalize(pDir + viewDir);
    float pShininess = (hardLightMode == 1) ? (shininess * 1.8) : shininess;
    float pSpec = pow(max(dot(normal, pHalf), 0.0), pShininess);

    // Inverse-square physical attenuation formula
    float pDistEff = max(pDist, 0.8);
    float win = clamp(1.0 - (pDist / maxRadius) * (pDist / maxRadius), 0.0, 1.0);
    win = win * win;
    float att = win / (constAtt + linAtt * pDistEff + quadAtt * pDistEff * pDistEff);

    diffuseSum  += pDiff * pColor * pIntensity * att;
    specularSum += specularStrength * pSpec * pColor * pIntensity * att;
}

void main()
{
    // ═══════════════════════════════════════════════════════════════
    // LAB TOPIC 4: "LIGHT INSIDE OBJECT"
    // An emissive source (Lantern flame, Full Moon, glowing Firefly) has
    // emissive > 0.5. It emits radiant light from INSIDE itself, bypassing
    // external lighting calculations and rendering at pure glowing color.
    // ═══════════════════════════════════════════════════════════════
    if (emissive > 0.5)
    {
        FragColor = vec4(objectColor, 1.0);
        return;
    }

    vec3 baseColor = objectColor;
    if (uUseTexture == 1)
    {
        baseColor = CalcProceduralDetail(objectColor, ObjPos, FragPos, normalize(Normal), uTextureType);
    }
    else if (uUseTexture == 2)
    {
        vec2 uv = GetTexCoords(ObjPos, FragPos, normalize(Normal), uTextureType);
        vec4 texSamp = texture(uTexture, uv);
        if (uTextureType == 1) {
            baseColor = objectColor * (texSamp.rgb * 1.55);
        } else {
            baseColor = objectColor * texSamp.rgb * 1.35;
        }
    }

    // Unshaded inspection modes
    if (noLighting == 1)
    {
        vec3 norm = normalize(Normal);
        float shade = 0.82 + 0.18 * max(dot(norm, normalize(vec3(0.35, 0.90, 0.40))), 0.0);
        FragColor = vec4(baseColor * shade, 1.0);
        return;
    }
    else if (noLighting == 2)
    {
        FragColor = vec4(baseColor, 1.0);
        return;
    }

    // ═══════════════════════════════════════════════════════════════
    // LAB TOPIC: GOURAUD SHADING (PER-VERTEX LIGHTING INTERPOLATION)
    // Ambient, diffuse, and specular terms are evaluated at vertices in the
    // vertex shader and linearly interpolated across primitive fragments by GPU hardware.
    // The fragment shader directly applies interpolated lighting to base color,
    // bypassing expensive per-fragment normalizations, light vectors, and pow() calls!
    // ═══════════════════════════════════════════════════════════════
    if (uShadingModel == 1)
    {
        vec3 result = GouraudDiff * baseColor + GouraudSpec + emissive * objectColor;
        if (fogDensity > 0.0001)
        {
            float distToCam = length(viewPos - FragPos);
            float distFog = 1.0 - exp(-distToCam * fogDensity);
            float heightFactor = clamp(1.0 - FragPos.y * 0.25, 0.0, 1.0);
            float fogFactor = clamp(distFog * (0.65 + 0.35 * heightFactor), 0.0, 0.88);
            result = mix(result, fogColor, fogFactor);
        }
        FragColor = vec4(result, 1.0);
        return;
    }

    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // ═══════════════════════════════════════════════════════════════
    // LAB TOPIC 2: PER-FRAGMENT BLINN-PHONG LIGHTING EQUATION
    // I = I_ambient + I_diffuse + I_specular + I_emissive
    // Written separately as individual terms:
    // ═══════════════════════════════════════════════════════════════
    vec3 dirAmbient, dirDiffuse, dirSpecular;
    CalcDirLightComponents(norm, viewDir, dirAmbient, dirDiffuse, dirSpecular);

    vec3 pointDiffuseSum  = vec3(0.0);
    vec3 pointSpecularSum = vec3(0.0);

    // 6 Positional Point Lights (Outdoor Courtyard, Moored Boat, Mosque, Kitchen Stove, Ghat, Cruising Boat)
    CalcPointLightComponents(pointLightPos,  pointLightColor,  pointLightIntensity,  24.0, 1.0, 0.15, 0.025, norm, FragPos, viewDir, pointDiffuseSum, pointSpecularSum);
    CalcPointLightComponents(pointLight2Pos, pointLight2Color, pointLight2Intensity, 18.0, 1.0, 0.18, 0.030, norm, FragPos, viewDir, pointDiffuseSum, pointSpecularSum);
    CalcPointLightComponents(pointLight3Pos, pointLight3Color, pointLight3Intensity, 20.0, 1.0, 0.18, 0.030, norm, FragPos, viewDir, pointDiffuseSum, pointSpecularSum);
    CalcPointLightComponents(pointLight4Pos, pointLight4Color, pointLight4Intensity, 15.0, 1.0, 0.22, 0.040, norm, FragPos, viewDir, pointDiffuseSum, pointSpecularSum);
    CalcPointLightComponents(pointLight5Pos, pointLight5Color, pointLight5Intensity, 18.0, 1.0, 0.18, 0.030, norm, FragPos, viewDir, pointDiffuseSum, pointSpecularSum);
    CalcPointLightComponents(pointLight6Pos, pointLight6Color, pointLight6Intensity, 18.0, 1.0, 0.18, 0.030, norm, FragPos, viewDir, pointDiffuseSum, pointSpecularSum);

    // Separate Phong terms explicitly combined
    vec3 ambientTerm  = dirAmbient * baseColor;
    vec3 diffuseTerm  = (dirDiffuse + pointDiffuseSum) * baseColor;
    vec3 specularTerm = (dirSpecular + pointSpecularSum);
    vec3 emissiveTerm = emissive * objectColor;

    vec3 result = ambientTerm + diffuseTerm + specularTerm + emissiveTerm;

    // Atmospheric Distance Fog
    if (fogDensity > 0.0001)
    {
        float distToCam = length(viewPos - FragPos);
        float distFog = 1.0 - exp(-distToCam * fogDensity);
        float heightFactor = clamp(1.0 - FragPos.y * 0.25, 0.0, 1.0);
        float fogFactor = clamp(distFog * (0.65 + 0.35 * heightFactor), 0.0, 0.88);
        result = mix(result, fogColor, fogFactor);
    }

    FragColor = vec4(result, 1.0);
}
)";

// ═══════════════════════════════════════════════════════════════
// Compile & link
// ═══════════════════════════════════════════════════════════════
void Shader::init(const char* vertexSrc, const char* fragmentSrc)
{
    // Vertex shader
    unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertexSrc, nullptr);
    glCompileShader(vert);
    checkCompileErrors(vert, "VERTEX");

    // Fragment shader
    unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragmentSrc, nullptr);
    glCompileShader(frag);
    checkCompileErrors(frag, "FRAGMENT");

    // Link program
    ID = glCreateProgram();
    glAttachShader(ID, vert);
    glAttachShader(ID, frag);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vert);
    glDeleteShader(frag);

    // Pre-cache high-frequency uniforms for zero-overhead hot draw calls
    m_uniformLocations.clear();
    locModel = glGetUniformLocation(ID, "model");
    locObjectColor = glGetUniformLocation(ID, "objectColor");
    m_uniformLocations.emplace("model", locModel);
    m_uniformLocations.emplace("objectColor", locObjectColor);
    m_uniformLocations.emplace("uShadingModel", glGetUniformLocation(ID, "uShadingModel"));
}

void Shader::use() const { glUseProgram(ID); }

int Shader::getUniformLocation(const char* name) const {
    auto it = m_uniformLocations.find(std::string_view(name));
    if (it != m_uniformLocations.end()) {
        return it->second;
    }
    int loc = glGetUniformLocation(ID, name);
    m_uniformLocations.emplace(name, loc);
    return loc;
}

// ═══════════════════════════════════════════════════════════════
// Ultra-fast direct uniform setters (bypasses all string lookups)
// ═══════════════════════════════════════════════════════════════
void Shader::setFastModel(const math::mat4& mat) const {
    if (locModel >= 0) {
        glUniformMatrix4fv(locModel, 1, GL_FALSE, mat.value_ptr());
    }
}

void Shader::setFastColor(const math::vec3& v) const {
    if (locObjectColor >= 0) {
        glUniform3f(locObjectColor, v.x, v.y, v.z);
    }
}

void Shader::setFastColor(float x, float y, float z) const {
    if (locObjectColor >= 0) {
        glUniform3f(locObjectColor, x, y, z);
    }
}

// ═══════════════════════════════════════════════════════════════
// Uniform getters & setters (accelerated by uniform location cache)
// ═══════════════════════════════════════════════════════════════
int Shader::getInt(const char* name) const {
    GLint val = 0;
    int loc = getUniformLocation(name);
    if (loc >= 0) glGetUniformiv(ID, loc, &val);
    return (int)val;
}

void Shader::setBool(const char* name, bool value) const {
    int loc = getUniformLocation(name);
    if (loc >= 0) glUniform1i(loc, (int)value);
}
void Shader::setInt(const char* name, int value) const {
    int loc = getUniformLocation(name);
    if (loc >= 0) glUniform1i(loc, value);
}
void Shader::setFloat(const char* name, float value) const {
    int loc = getUniformLocation(name);
    if (loc >= 0) glUniform1f(loc, value);
}
void Shader::setVec3(const char* name, const math::vec3& v) const {
    int loc = getUniformLocation(name);
    if (loc >= 0) glUniform3f(loc, v.x, v.y, v.z);
}
void Shader::setVec3(const char* name, float x, float y, float z) const {
    int loc = getUniformLocation(name);
    if (loc >= 0) glUniform3f(loc, x, y, z);
}
void Shader::setMat4(const char* name, const math::mat4& mat) const {
    int loc = getUniformLocation(name);
    if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, mat.value_ptr());
}

// ═══════════════════════════════════════════════════════════════
// Error checking
// ═══════════════════════════════════════════════════════════════
void Shader::checkCompileErrors(unsigned int shader, const char* type) const
{
    int  success;
    char infoLog[1024];

    if (type[0] != 'P') // VERTEX or FRAGMENT
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "SHADER COMPILE ERROR (" << type << "):\n" << infoLog << "\n";
        }
    }
    else // PROGRAM
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "SHADER LINK ERROR:\n" << infoLog << "\n";
        }
    }
}
