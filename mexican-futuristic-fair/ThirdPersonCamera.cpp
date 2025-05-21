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
    // Calcular la nueva posición
    position = *target + offsetDir * distance;

    // Verificar si la cámara está por debajo de la altura mínima permitida
    float minHeight = 1.0f; // Ajusta este valor según necesites

    if (position.y < (*target).y + minHeight) {
        // Calculamos la distancia horizontal
        float horizontalDist = sqrt(pow(offsetDir.x * distance, 2) + pow(offsetDir.z * distance, 2));

        // Recalculamos el offsetDir
        glm::vec3 newOffsetDir;
        newOffsetDir.x = offsetDir.x;
        newOffsetDir.z = offsetDir.z;

        // Normalizamos solo las componentes x y z
        float xyLength = sqrt(newOffsetDir.x * newOffsetDir.x + newOffsetDir.z * newOffsetDir.z);
        if (xyLength > 0) {
            newOffsetDir.x /= xyLength;
            newOffsetDir.z /= xyLength;
        }

        // Establecemos la componente y para que la altura sea exactamente target.y + minHeight
        position.y = (*target).y + minHeight;

        // Recalculamos la posición x,z manteniendo la distancia horizontal
        position.x = (*target).x + newOffsetDir.x * horizontalDist;
        position.z = (*target).z + newOffsetDir.z * horizontalDist;

        // Actualizar el offsetDir basado en la nueva posición
        offsetDir = glm::normalize(position - *target);

        // Actualizar el pitch (ángulo vertical)
        pitch = glm::degrees(asin(offsetDir.y));
    }
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

glm::vec3 ThirdPersonCamera::getHorizontalDirection() const {
    glm::vec3 dir = getCameraDirection();
    dir.y = 0.0f; // ignorar inclinación vertical
    return glm::normalize(dir);
}

float ThirdPersonCamera::getYaw() const { return yaw; }