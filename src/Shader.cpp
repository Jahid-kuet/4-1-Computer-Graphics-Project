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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos     = vec3(model * vec4(aPos, 1.0));
    Normal      = mat3(transpose(inverse(model))) * aNormal;
    ObjPos      = aPos;
    TexCoord    = vec2(aPos.x + aPos.z * 0.5, aPos.y);
    gl_Position = projection * view * vec4(FragPos, 1.0);
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

uniform vec3  objectColor;
uniform vec3  lightDir;        // direction FROM light source
uniform vec3  lightColor;
uniform vec3  viewPos;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;
uniform float emissive;        // 1.0 = fully emissive, 0.0 = normal Phong

// Procedural and GPU Texture Mapping controls
uniform sampler2D uTexture;
uniform int       uUseTexture;    // 0 = Off (Solid Color), 1 = Procedural GLSL Detailing, 2 = Sample GPU Texture Map
uniform int       uTextureType;   // 0 = Wood, 1 = Brick, 2 = Bamboo, 3 = Fabric

// Directional and Point Light master toggles
uniform int   dirLightEnabled;    // 1 = Dir Light ON, 0 = Dir Light OFF (Pure Point Lights inspection)
uniform int   pointLightsEnabled;  // 1 = Point Lights ON, 0 = Point Lights OFF (Pure Dir Light inspection)

// Point light 1 (Courtyard Hurricane Lantern)
uniform vec3  pointLightPos;
uniform vec3  pointLightColor;
uniform float pointLightIntensity;

// Point light 2 (Moored Boat Hurricane Lantern)
uniform vec3  pointLight2Pos;
uniform vec3  pointLight2Color;
uniform float pointLight2Intensity;

// Point light 3 (Mosque Entrance Portal Lantern)
uniform vec3  pointLight3Pos;
uniform vec3  pointLight3Color;
uniform float pointLight3Intensity;

// Point light 4 (Kitchen Clay Stove Fire / Embers)
uniform vec3  pointLight4Pos;
uniform vec3  pointLight4Color;
uniform float pointLight4Intensity;

// Point light 5 (River Landing Ghat Mooring Post Lantern)
uniform vec3  pointLight5Pos;
uniform vec3  pointLight5Color;
uniform float pointLight5Intensity;

// Point light 6 (Cruising Dingi Nouka Dynamic Lantern)
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

// Procedural GLSL surface detail generator (calculates real-time grain per fragment)
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
        if (isMortar) {
            return vec3(0.72, 0.70, 0.65);
        } else {
            float noise = hash21(floor(vec2(bCoord.x + xOff, bCoord.y))) * 0.22;
            return baseCol * (0.89 + noise);
        }
    }
    else if (texType == 2) // Bamboo weave & split fibers (Chatai / Bansh)
    {
        vec2 bUv = pUV * 6.5;
        int uB = int(floor(bUv.x + bUv.y));
        int vB = int(floor(bUv.x - bUv.y + 100.0));
        bool weft = ((uB + vB) % 2 == 0);
        float split = fract(bUv.x + bUv.y);
        float fiber = 0.84 + 0.16 * sin(split * 3.14159);
        return baseCol * (weft ? 1.08 : 0.84) * fiber;
    }
    else if (texType == 3) // Bengali checked lungi / gamcha plaid
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

// Intelligent UV generation for sampling GPU texture map
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

// ── Directional Light Calculation (Parallel rays from Moon / Sun) ─────
vec3 CalcDirLight(vec3 normal, vec3 viewDir)
{
    if (dirLightEnabled == 0) return vec3(0.0);
    vec3 lightD = normalize(-lightDir);
    
    // Ambient component
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse component (Lambertian)
    float diff = max(dot(normal, lightD), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular component (Blinn-Phong)
    vec3 halfDir = normalize(lightD + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
    
    return (ambient + diffuse + specular);
}

// ── Positional Point Light Calculation (1 / (Kc + Kl*d + Kq*d^2) Attenuation) ──
vec3 CalcPointLight(vec3 pPos, vec3 pColor, float pIntensity, float maxRadius, float constAtt, float linAtt, float quadAtt, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    if (pointLightsEnabled == 0 || pIntensity <= 0.001) return vec3(0.0);
    vec3 pVec = pPos - fragPos;
    float pDist = length(pVec);
    if (pDist >= maxRadius) return vec3(0.0);

    vec3 pDir = normalize(pVec);
    // Diffuse with soft organic wrap
    float pDiff = max(dot(normal, pDir), 0.0);
    pDiff = pDiff * 0.72 + 0.28 * max(normal.y, 0.0);

    // Specular (Blinn-Phong)
    vec3 pHalf = normalize(pDir + viewDir);
    float pSpec = pow(max(dot(normal, pHalf), 0.0), shininess);

    // Standard attenuation formula with smooth distance cutoff
    float pDistEff = max(pDist, 0.8);
    float win = clamp(1.0 - (pDist / maxRadius) * (pDist / maxRadius), 0.0, 1.0);
    win = win * win;
    float att = win / (constAtt + linAtt * pDistEff + quadAtt * pDistEff * pDistEff);

    return (pDiff * pColor + specularStrength * pSpec * pColor) * pIntensity * att;
}

void main()
{
    // Emissive bypass (moon, stars, fireflies, lantern flame, glowing windows)
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
        baseColor = objectColor * texSamp.rgb * 1.35;
    }

    // Object display mode (no lighting yet - for milestone grading)
    if (noLighting == 1)
    {
        // Gentle directional facet contrast (82% ambient + 18% directional) so 3D surfaces are clear
        vec3 norm = normalize(Normal);
        float shade = 0.82 + 0.18 * max(dot(norm, normalize(vec3(0.35, 0.90, 0.40))), 0.0);
        FragColor = vec4(baseColor * shade, 1.0);
        return;
    }
    else if (noLighting == 2)
    {
        // Pure flat color without any shading
        FragColor = vec4(baseColor, 1.0);
        return;
    }

    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // 1. Directional Light (Moon / Sun)
    vec3 totalLight = CalcDirLight(norm, viewDir);

    // 2. All 6 Positional Point Lights
    // Point Light 1: Courtyard Hurricane Lantern (Hariken on stool)
    totalLight += CalcPointLight(pointLightPos, pointLightColor, pointLightIntensity, 24.0, 1.0, 0.15, 0.025, norm, FragPos, viewDir);

    // Point Light 2: Moored Boat Hurricane Lantern (Hanging under Chhoi)
    totalLight += CalcPointLight(pointLight2Pos, pointLight2Color, pointLight2Intensity, 18.0, 1.0, 0.18, 0.030, norm, FragPos, viewDir);

    // Point Light 3: Mosque Entrance Portal Lantern (Mehrab Arched Lamp)
    totalLight += CalcPointLight(pointLight3Pos, pointLight3Color, pointLight3Intensity, 20.0, 1.0, 0.18, 0.030, norm, FragPos, viewDir);

    // Point Light 4: Kitchen Clay Cooking Stove Embers (Matir Chula Wood Fire)
    totalLight += CalcPointLight(pointLight4Pos, pointLight4Color, pointLight4Intensity, 15.0, 1.0, 0.22, 0.040, norm, FragPos, viewDir);

    // Point Light 5: River Landing Ghat Mooring Post Lantern
    totalLight += CalcPointLight(pointLight5Pos, pointLight5Color, pointLight5Intensity, 18.0, 1.0, 0.18, 0.030, norm, FragPos, viewDir);

    // Point Light 6: Cruising Dingi Nouka Dynamic Lantern (navigates along river in real time!)
    totalLight += CalcPointLight(pointLight6Pos, pointLight6Color, pointLight6Intensity, 18.0, 1.0, 0.18, 0.030, norm, FragPos, viewDir);

    vec3 result = totalLight * baseColor;

    // Atmospheric Distance & Height-Dependent Low Nocturnal Mist
    if (fogDensity > 0.0001)
    {
        float distToCam = length(viewPos - FragPos);
        float distFog = 1.0 - exp(-distToCam * fogDensity);
        // Low ground height mist factor: denser near river basin and ground, clearing higher up
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
}

void Shader::use() const { glUseProgram(ID); }

// ═══════════════════════════════════════════════════════════════
// Uniform setters
// ═══════════════════════════════════════════════════════════════
void Shader::setBool(const char* name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name), (int)value);
}
void Shader::setInt(const char* name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name), value);
}
void Shader::setFloat(const char* name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name), value);
}
void Shader::setVec3(const char* name, const math::vec3& v) const {
    glUniform3f(glGetUniformLocation(ID, name), v.x, v.y, v.z);
}
void Shader::setVec3(const char* name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}
void Shader::setMat4(const char* name, const math::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, mat.value_ptr());
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
