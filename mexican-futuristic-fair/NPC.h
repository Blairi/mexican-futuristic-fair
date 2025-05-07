#pragma once
#pragma once
#include <string>
#include <glm.hpp>
#include "ModelJerarquia.h"
#include "ObjectFocusCamera.h"
#include <map>

class NPC {
public:
    NPC(ModelJerarquia& model,
        const glm::vec3& worldPos,
        const std::string& dialogText,
        ObjectFocusCamera* focusCam,
        Mesh* quad,             // NUEVO
        Texture* font,          // NUEVO
        const std::map<char, glm::vec2>& atlas);   // NUEVO

    void render(const GLuint& uniformModel);
    void interact();            // (pulso E)
    void update(float dt);
    void renderDialog(GLuint shaderID,
        GLuint uModel, GLuint uColor);

    // --------------- (getters)
    const glm::vec3& getPosition() const { return position; }

private:
    ModelJerarquia& modelRef;
    glm::vec3        position;
    std::string      text;

    // Recursos UI
    Mesh* quadMesh;
    Texture* fontTex;
    const std::map<char, glm::vec2>& atlasRef;

    ObjectFocusCamera* cam;
    bool   dialogActive;
    float  dialogTimer;
};
