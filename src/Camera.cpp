// Camera.cpp — Orbit camera implementation.

#include "Camera.h"
#include <cmath>
#include <algorithm>

using namespace math;

Camera::Camera()
    : target(0.0f, 2.0f, 0.0f)
    , yaw(0.0f)
    , pitch(radians(30.0f))
    , distance(25.0f)
    , fov(radians(45.0f))
    , aspectRatio(1280.0f / 720.0f)
    , nearPlane(0.1f)
    , farPlane(200.0f)
    , position(0.0f)
{
    updatePosition();
}

void Camera::updatePosition()
{
    // Spherical to Cartesian (Y-up)
    position.x = target.x + distance * cosf(pitch) * sinf(yaw);
    position.y = target.y + distance * sinf(pitch);
    position.z = target.z + distance * cosf(pitch) * cosf(yaw);
}

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

    // Clamp pitch to avoid gimbal lock
    const float maxPitch = radians(89.0f);
    const float minPitch = radians(-10.0f);
    if (pitch > maxPitch) pitch = maxPitch;
    if (pitch < minPitch) pitch = minPitch;

    updatePosition();
}

void Camera::processScroll(float yoffset)
{
    distance -= yoffset * 1.5f;
    if (distance < 3.0f)  distance = 3.0f;
    if (distance > 80.0f) distance = 80.0f;

    updatePosition();
}

void Camera::processKeyboardMovement(float forward, float right, float dt, float speed)
{
    // Horizontal direction vectors in world XZ plane
    vec3 fwd(-sinf(yaw), 0.0f, -cosf(yaw));
    vec3 rgt(cosf(yaw), 0.0f, -sinf(yaw));

    float velocity = speed * dt;
    target.x += (fwd.x * forward + rgt.x * right) * velocity;
    target.z += (fwd.z * forward + rgt.z * right) * velocity;

    // Boundary limits to keep camera within the expansive 75x75m landscape
    if (target.x < -36.0f) target.x = -36.0f;
    if (target.x >  36.0f) target.x =  36.0f;
    if (target.z < -36.0f) target.z = -36.0f;
    if (target.z >  36.0f) target.z =  36.0f;

    updatePosition();
}
