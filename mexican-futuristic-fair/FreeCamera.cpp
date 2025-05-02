#include "FreeCamera.h"
#include <gtc/matrix_transform.hpp>

// Ctor: llama al base
FreeCamera::FreeCamera(glm::vec3 startPosition,
    glm::vec3 startUp,
    GLfloat  startYaw,
    GLfloat  startPitch,
    GLfloat  startMoveSpeed,
    GLfloat  startTurnSpeed)
    : Camera(startPosition, startUp),
    worldUp(startUp),
    yaw(startYaw),
    pitch(startPitch),
    moveSpeed(startMoveSpeed),
    turnSpeed(startTurnSpeed)
{
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    updateCameraVectors();
}

glm::mat4 FreeCamera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

// Ahora coincide la firma override
void FreeCamera::update(float /*deltaTime*/) {
    updateCameraVectors();
}

void FreeCamera::keyControl(bool* keys, float deltaTime) {
    // velocidad depende del deltaTime y de moveSpeed
    float velocity = moveSpeed * deltaTime;
    if (keys[GLFW_KEY_W]) position += front * velocity;
    if (keys[GLFW_KEY_S]) position -= front * velocity;
    if (keys[GLFW_KEY_A]) position -= right * velocity;
    if (keys[GLFW_KEY_D]) position += right * velocity;
    // después de cambiar posición, no hace falta recalcinar vectores de orientación
}

void FreeCamera::mouseControl(float xChange, float yChange) {
    // aplica sensibilidad
    xChange *= turnSpeed;
    yChange *= turnSpeed;

    yaw += xChange;
    pitch += yChange;

    // clamp para evitar "volteretas"
    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // recalcina front, right, up
    updateCameraVectors();
}

void FreeCamera::updateCameraVectors() {
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void FreeCamera::setPosition(const glm::vec3& pos) {
    position = pos;
}

void FreeCamera::setDirection(const glm::vec3& dir) {
    glm::vec3 d = glm::normalize(dir);
    pitch = glm::degrees(asin(d.y));
    yaw = glm::degrees(atan2(d.z, d.x));
    updateCameraVectors();
}

// Implementaciones de los getters
glm::vec3 FreeCamera::getCameraPosition() const {
    return position;
}
glm::vec3 FreeCamera::getCameraDirection() const {
    return glm::normalize(front);
}
