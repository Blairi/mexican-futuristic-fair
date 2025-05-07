#include "ThirdPersonCamera.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/constants.hpp>
ThirdPersonCamera::ThirdPersonCamera(glm::vec3 offset,
    glm::vec3 up,
    glm::vec3* targetObj)
    : Camera(*targetObj + offset, up),
    target(targetObj),
    distance(glm::length(offset)),
    yaw(-90.0f), pitch(0.0f),
    turnSpeed(0.8f)
{
    offsetDir = glm::normalize(offset);
}

void ThirdPersonCamera::recalcOffsetDir() {
    offsetDir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    offsetDir.y = sin(glm::radians(pitch));
    offsetDir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    offsetDir = glm::normalize(offsetDir);
}

void ThirdPersonCamera::update(float) {
    // posición = personaje + dirección*distancia
    position = *target + offsetDir * distance;
}

glm::mat4 ThirdPersonCamera::getViewMatrix() {
    return glm::lookAt(position, *target, up);
}

void ThirdPersonCamera::mouseControl(float xDelta, float yDelta) {
    yaw -= xDelta * turnSpeed;
    pitch -= yDelta * turnSpeed;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
    recalcOffsetDir();
}

void ThirdPersonCamera::addDistance(float d) {
    distance = glm::clamp(distance + d, 3.0f, 20.0f);   // 3 ≤ r ≤ 20
}


glm::vec3 ThirdPersonCamera::getCameraPosition()  const { return position; }
glm::vec3 ThirdPersonCamera::getCameraDirection() const {
    return glm::normalize((*target) - position);            // hacia el avatar
}
float ThirdPersonCamera::getYaw() const { return yaw; }

glm::vec3 ThirdPersonCamera::getHorizontalDirection() const {
    glm::vec3 dir = getCameraDirection();
    dir.y = 0.0f; // ignorar inclinación vertical
    return glm::normalize(dir);
}