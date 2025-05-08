#pragma once
#include "Camera.h"
#include <vector>

class ObjectFocusCamera : public Camera {
public:
    explicit ObjectFocusCamera(glm::vec3 up,
        const std::vector<glm::vec3*>& objects,
        float dist = 10.0f);

    // M�todos de c�mara
    glm::mat4 getViewMatrix() override;
    void update(float deltaTime) override;
    glm::vec3 getCameraPosition() const override;
    glm::vec3 getCameraDirection() const override;

    // M�todos espec�ficos
    void nextObject();
    void beginFocus(glm::vec3* target);
    void endFocus();
    bool isFocusing() const { return tempTarget != nullptr; }

private:
    std::vector<glm::vec3*> targets;
    int currentTarget;
    glm::vec3* tempTarget;

    // Transici�n suave
    glm::vec3 currentPosition;    // Posici�n actual de la c�mara
    glm::vec3 targetPosition;     // Posici�n objetivo
    glm::vec3 currentLookAt;      // Punto actual al que mira
    glm::vec3 targetLookAt;       // Punto objetivo al que mirar
    float transitionSpeed;        // Velocidad de transici�n
    bool inTransition;            // Si est� en transici�n
    float      distFromTarget;
};