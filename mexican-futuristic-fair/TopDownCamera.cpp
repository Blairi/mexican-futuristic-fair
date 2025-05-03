#include "TopDownCamera.h"

TopDownCamera::TopDownCamera(glm::vec3 center, float height, glm::vec3 up)
    : Camera(center + glm::vec3(0.0f, height, 0.0f), up), centerPoint(center) {}

glm::mat4 TopDownCamera::getViewMatrix() {
    return glm::lookAt(position, centerPoint, glm::vec3(0.0f, 0.0f, -1.0f));
}

void TopDownCamera::update(float deltaTime) {}

void TopDownCamera::moveHorizontal(float amount) {
    position.x += amount;
    centerPoint.x += amount;
}

void TopDownCamera::moveVertical(float amount) {
    position.z += amount;
    centerPoint.z += amount;
}
glm::vec3 TopDownCamera::getCameraPosition() const {
    return position;
}
glm::vec3 TopDownCamera::getCameraDirection() const {
    return glm::normalize(centerPoint - position);
}