#include "ObjectFocusCamera.h"
#include <gtc/matrix_transform.hpp>

ObjectFocusCamera::ObjectFocusCamera(glm::vec3 up,
    const std::vector<glm::vec3*>& objects,
    float dist)
    : Camera(glm::vec3(0.0f), up),
    targets(objects),
    currentTarget(0),
    tempTarget(nullptr),
    currentPosition(glm::vec3(0.0f)),
    targetPosition(glm::vec3(0.0f)),
    currentLookAt(glm::vec3(0.0f)),
    targetLookAt(glm::vec3(0.0f)),
    transitionSpeed(5.0f),
    inTransition(false),
    distFromTarget(dist)           // ← guarda la distancia deseada

{
    // Inicializar posiciones
    if (!targets.empty() && targets[0]) {
        glm::vec3* tgt = targets[0];
        currentLookAt = *tgt;
        currentPosition = *tgt + glm::vec3(0.0f, 3.0f, distFromTarget);
        position = currentPosition;
    }
}

glm::mat4 ObjectFocusCamera::getViewMatrix() {
    return glm::lookAt(currentPosition, currentLookAt, up);
}

void ObjectFocusCamera::update(float deltaTime) {
    if (!inTransition) return;

    float step = transitionSpeed * deltaTime;
    if (step > 1.0f) step = 1.0f;

    currentPosition = glm::mix(currentPosition, targetPosition, step);
    currentLookAt = glm::mix(currentLookAt, targetLookAt, step);
    position = currentPosition;

    if (glm::distance(currentPosition, targetPosition) < 0.01f &&
        glm::distance(currentLookAt, targetLookAt) < 0.01f)
        inTransition = false;
}

void ObjectFocusCamera::beginFocus(glm::vec3* target) {
    if (!target) return;

    tempTarget = target;
    targetLookAt = *target;
    // Coloca la cámara a `distFromTarget` unidades justo enfrente del objeto.
    glm::vec3 dir = glm::normalize(*target - currentPosition);
    targetPosition = *target - dir * distFromTarget + glm::vec3(0.0f, 1.0f, 0.0f);
    inTransition = true;
}

void ObjectFocusCamera::endFocus() {
    if (!tempTarget || targets.empty()) return;

    tempTarget = nullptr;
    glm::vec3* tgt = targets[currentTarget];
    targetLookAt = *tgt;
    targetPosition = *tgt + glm::vec3(0.0f, 3.0f, distFromTarget);
    inTransition = true;
}

void ObjectFocusCamera::nextObject() {
    if (isFocusing() || targets.empty()) return;

    currentTarget = (currentTarget + 1) % targets.size();
    glm::vec3* tgt = targets[currentTarget];

    targetLookAt = *tgt;
    targetPosition = *tgt + glm::vec3(0.0f, 3.0f, distFromTarget);
    inTransition = true;
}

glm::vec3 ObjectFocusCamera::getCameraPosition() const {
    return currentPosition;
}

glm::vec3 ObjectFocusCamera::getCameraDirection() const {
    return glm::normalize(currentLookAt - currentPosition);
}