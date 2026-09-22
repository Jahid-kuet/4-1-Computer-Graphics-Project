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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos     = vec3(model * vec4(aPos, 1.0));
    Normal      = mat3(transpose(inverse(model))) * aNormal;
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

uniform vec3  objectColor;
uniform vec3  lightDir;        // direction FROM light source
uniform vec3  lightColor;
uniform vec3  viewPos;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;
uniform float emissive;        // 1.0 = fully emissive, 0.0 = normal Phong

// Point light 1 (Courtyard Hurricane Lantern)
uniform vec3  pointLightPos;
uniform vec3  pointLightColor;
uniform float pointLightIntensity;

// Point light 2 (Boat Hurricane Lantern)
uniform vec3  pointLight2Pos;
uniform vec3  pointLight2Color;
uniform float pointLight2Intensity;

// Distance fog for atmospheric depth
uniform vec3  fogColor;
uniform float fogDensity;

void main()
{
    // Emissive bypass (moon, stars, fireflies, lantern flame)
    if (emissive > 0.5)
    {
        FragColor = vec4(objectColor, 1.0);
        return;
    }

    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Ambient (cool silvery moonlight in night mode)
    vec3 ambient = ambientStrength * lightColor;

    // Directional Diffuse (Moon / Sun)
    vec3  lightD  = normalize(-lightDir);
    float diff    = max(dot(norm, lightD), 0.0);
    vec3  diffuse = diff * lightColor;

    // Directional Specular (Blinn-Phong)
    vec3  halfDir  = normalize(lightD + viewDir);
    float spec     = pow(max(dot(norm, halfDir), 0.0), shininess);
    vec3  specular = specularStrength * spec * lightColor;

    // Point Light (Courtyard Hurricane Lantern — soft, warm golden glow)
    if (pointLightIntensity > 0.01)
    {
        vec3  pVec = pointLightPos - FragPos;
        float pDist = length(pVec);
        float maxRadius = 24.0;
        if (pDist < maxRadius)
        {
            vec3  pDir = normalize(pVec);
            float pDiff = max(dot(norm, pDir), 0.0);
            // Soft wrap for realistic lantern light dispersal across courtyard
            pDiff = pDiff * 0.70 + 0.30 * max(norm.y, 0.0);

            float pDistEff = max(pDist, 1.2);
            float win = clamp(1.0 - (pDist / maxRadius) * (pDist / maxRadius), 0.0, 1.0);
            win = win * win; // smooth falloff curve
            float att = win / (1.0 + 0.15 * pDistEff + 0.025 * pDistEff * pDistEff);

            vec3  pHalf = normalize(pDir + viewDir);
            float pSpec = pow(max(dot(norm, pHalf), 0.0), shininess);
            vec3  pContrib = (pDiff * pointLightColor + specularStrength * pSpec * pointLightColor) * pointLightIntensity * att;
            diffuse += pContrib;
        }
    }

    // Point Light 2 (Boat Hurricane Lantern — warm golden glow over boat and river)
    if (pointLight2Intensity > 0.01)
    {
        vec3  pVec = pointLight2Pos - FragPos;
        float pDist = length(pVec);
        float maxRadius = 18.0;
        if (pDist < maxRadius)
        {
            vec3  pDir = normalize(pVec);
            float pDiff = max(dot(norm, pDir), 0.0);
            pDiff = pDiff * 0.70 + 0.30 * max(norm.y, 0.0);

            float pDistEff = max(pDist, 1.2);
            float win = clamp(1.0 - (pDist / maxRadius) * (pDist / maxRadius), 0.0, 1.0);
            win = win * win;
            float att = win / (1.0 + 0.18 * pDistEff + 0.03 * pDistEff * pDistEff);

            vec3  pHalf = normalize(pDir + viewDir);
            float pSpec = pow(max(dot(norm, pHalf), 0.0), shininess);
            vec3  pContrib = (pDiff * pointLight2Color + specularStrength * pSpec * pointLight2Color) * pointLight2Intensity * att;
            diffuse += pContrib;
        }
    }

    vec3 result = (ambient + diffuse + specular) * objectColor;

    // Atmospheric Distance Fog
    if (fogDensity > 0.0001)
    {
        float distToCam = length(viewPos - FragPos);
        float fogFactor = 1.0 - exp(-distToCam * fogDensity);
        fogFactor = clamp(fogFactor, 0.0, 0.90);
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
