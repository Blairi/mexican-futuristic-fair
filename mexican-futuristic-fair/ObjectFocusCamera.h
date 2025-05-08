#pragma once
#include "Camera.h"
#include <vector>

class ObjectFocusCamera : public Camera {
public:
    explicit ObjectFocusCamera(glm::vec3 up,
        const std::vector<glm::vec3*>& objects,
        float dist = 10.0f);

    // Métodos de cámara
    glm::mat4 getViewMatrix() override;
    void update(float deltaTime) override;
    glm::vec3 getCameraPosition() const override;
    glm::vec3 getCameraDirection() const override;

    // Métodos específicos
    void nextObject();
    void beginFocus(glm::vec3* target);
    void endFocus();
    bool isFocusing() const { return tempTarget != nullptr; }

private:
    std::vector<glm::vec3*> targets;
    int currentTarget;
    glm::vec3* tempTarget;

    // Transición suave
    glm::vec3 currentPosition;    // Posición actual de la cámara
    glm::vec3 targetPosition;     // Posición objetivo
    glm::vec3 currentLookAt;      // Punto actual al que mira
    glm::vec3 targetLookAt;       // Punto objetivo al que mirar
    float transitionSpeed;        // Velocidad de transición
    bool inTransition;            // Si está en transición
    float      distFromTarget;
};