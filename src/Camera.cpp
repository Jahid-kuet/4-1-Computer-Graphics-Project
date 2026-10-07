// Camera.cpp — Orbit camera implementation.

#include "Camera.h"
#include <cmath>
#include <algorithm>

using namespace math;

Camera::Camera()
    : target(0.0f, 2.0f, 0.0f)
    , yaw(0.0f)
    , pitch(radians(28.0f))
    , distance(22.0f)
    // LAB TOPIC 1: PROJECTION & DEPTH/SCALE
    // Sensible Perspective FOV = 55 degrees (0.9599 rad) provides realistic human perspective
    // and strong depth foreshortening (far objects shrink noticeably compared to near objects).
    , fov(radians(55.0f))
    , aspectRatio(1280.0f / 720.0f)
    , nearPlane(0.15f)
    , farPlane(350.0f)
    , position(0.0f)
{
    updatePosition();
}

void Camera::updatePosition()
{
    // Spherical to Cartesian coordinate transformation (Y-up convention)
    // Eye position is computed on a sphere of radius 'distance' around 'target':
    position.x = target.x + distance * cosf(pitch) * sinf(yaw);
    position.y = target.y + distance * sinf(pitch);
    position.z = target.z + distance * cosf(pitch) * cosf(yaw);
}

// ═══════════════════════════════════════════════════════════════════
// LAB TOPIC 1: THE COMPLETE 3D GRAPHICS TRANSFORMATION PIPELINE
// 1. OBJECT (LOCAL) SPACE:
//    Vertices are defined relative to the model's own local origin.
// 2. WORLD SPACE (MODEL MATRIX):
//    P_world = ModelMatrix * P_local
//    Hierarchical composition using translate, rotate, and scale.
// 3. VIEW / CAMERA SPACE (VIEW MATRIX via lookAt):
//    P_view = ViewMatrix * P_world
//    Translates and reorients world coordinates so the camera is at the origin (0,0,0)
//    pointing down the negative Z-axis (-Z).
// 4. CLIP SPACE (HOMOGENEOUS via perspective projection):
//    P_clip = ProjectionMatrix * P_view = (x_c, y_c, z_c, w_c)^T
//    *** CLIPPING HAPPENS HERE IN HARDWARE ***
//    Any primitive vertex outside the viewing frustum:
//      -w_c <= x_c <= w_c (Left / Right clipping planes)
//      -w_c <= y_c <= w_c (Bottom / Top clipping planes)
//      -w_c <= z_c <= w_c (Near / Far clipping planes)
//    is clipped against the 6 frustum planes before rasterization.
// 5. NORMALIZED DEVICE COORDINATES (NDC via Perspective Division):
//    P_ndc = (x_c / w_c, y_c / w_c, z_c / w_c)^T in range [-1.0, 1.0]^3.
//    Because w_c = -z_view, dividing by w_c causes objects farther from the camera
//    to become progressively smaller (depth foreshortening).
// 6. VIEWPORT / SCREEN SPACE:
//    Maps NDC [-1, 1] to window pixel coordinates [0, width] x [0, height].
// ═══════════════════════════════════════════════════════════════════
mat4 Camera::getViewMatrix() const
{
    return lookAt(position, target, vec3(0.0f, 1.0f, 0.0f));
}

mat4 Camera::getProjectionMatrix() const
{
    return perspective(fov, aspectRatio, nearPlane, farPlane);
}

void Camera::processMouseDrag(float dx, float dy, float sensitivity)
{
    yaw   += dx * sensitivity;
    pitch += dy * sensitivity;

    // Clamp pitch to avoid gimbal lock at poles
    const float maxPitch = radians(89.0f);
    const float minPitch = radians(-10.0f);
    if (pitch > maxPitch) pitch = maxPitch;
    if (pitch < minPitch) pitch = minPitch;

    updatePosition();
}

void Camera::processScroll(float yoffset)
{
    // LAB TOPIC 1 (PROBLEM 1 FIX): TRUE CAMERA DOLLY ZOOM
    // Scrolling changes the camera's physical distance (dolly) from the target in world space.
    // It does NOT change the FOV or rescale objects, preserving natural perspective foreshortening.
    distance -= yoffset * 1.5f;
    if (distance < 1.5f)  distance = 1.5f;
    if (distance > 70.0f) distance = 70.0f;

    updatePosition();
}

void Camera::processKeyboardMovement(float forward, float right, float upDown, float dt, float speed)
{
    // Horizontal direction vectors in world XZ plane aligned with camera yaw
    vec3 fwd(-sinf(yaw), 0.0f, -cosf(yaw));
    vec3 rgt(cosf(yaw), 0.0f, -sinf(yaw));

    float velocity = speed * dt;
    target.x += (fwd.x * forward + rgt.x * right) * velocity;
    target.y += upDown * velocity;
    target.z += (fwd.z * forward + rgt.z * right) * velocity;

    // Boundary limits to keep camera target within the 95x95m landscape
    if (target.x < -44.0f) target.x = -44.0f;
    if (target.x >  44.0f) target.x =  44.0f;
    if (target.y <   0.4f) target.y =   0.4f;
    if (target.y >  28.0f) target.y =  28.0f;
    if (target.z < -44.0f) target.z = -44.0f;
    if (target.z >  44.0f) target.z =  44.0f;

    updatePosition();
}
