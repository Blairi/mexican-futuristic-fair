#include "ThirdPersonCamera.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/constants.hpp>

ThirdPersonCamera::ThirdPersonCamera(glm::vec3 offset, glm::vec3 up, glm::vec3* targetObject)
    : Camera(*targetObject + offset, up), offset(offset), target(targetObject),
    yaw(-90.0f), pitch(0.0f), moveSpeed(0.6f), turnSpeed(0.9f) {}

glm::mat4 ThirdPersonCamera::getViewMatrix() {
    position = *target + offset;
    return glm::lookAt(position, *target, up);
}

void ThirdPersonCamera::update(float deltaTime) {
    calculateOffset();
}

void ThirdPersonCamera::calculateOffset() {
    offset.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    offset.y = sin(glm::radians(pitch));
    offset.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    offset = glm::normalize(offset) * glm::length(offset);
}

void ThirdPersonCamera::keyControl(bool* keys, float deltaTime) {
    float velocity = moveSpeed * deltaTime;

    if (keys[GLFW_KEY_UP])
        target->z -= velocity;
    if (keys[GLFW_KEY_DOWN])
        target->z += velocity;
    if (keys[GLFW_KEY_LEFT])
        target->x -= velocity;
    if (keys[GLFW_KEY_RIGHT])
        target->x += velocity;
    if (keys[GLFW_KEY_Q])
        target->y += velocity;
    if (keys[GLFW_KEY_E])
        target->y -= velocity;
}

void ThirdPersonCamera::mouseControl(float xChange, float yChange) {
    yaw += xChange * turnSpeed;
    pitch += yChange * turnSpeed;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    calculateOffset();
}

glm::vec3 ThirdPersonCamera::getCameraPosition() const {
    return position;
}
glm::vec3 ThirdPersonCamera::getCameraDirection() const {
    return glm::normalize((*target) - position);
}
