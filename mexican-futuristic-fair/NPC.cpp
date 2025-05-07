#include "NPC.h"
#include <gtc\matrix_transform.hpp>
#include "ModelJerarquia.h"
#include <map>

// Duración en segundos que el diálogo permanece abierto
static constexpr float DIALOG_DURATION = 5.0f;
NPC::NPC(ModelJerarquia& mdl,
    const glm::vec3& worldPos,
    const std::string& dialogText,
    ObjectFocusCamera* focusCam,
    Mesh* quad,
    Texture* font,
    const std::map<char, glm::vec2>& atlas)
    : modelRef(mdl),
    position(worldPos),
    text(dialogText),
    quadMesh(quad),
    fontTex(font),
    atlasRef(atlas),
    cam(focusCam),
    dialogActive(false),
    dialogTimer(0.0f)
{}

/* ---------- igual render() ---------- */

void NPC::render(const GLuint& uniformModel)
{
    /*
       • Colocamos el modelo en su posición ‘position’.
       • NO tocamos jerarquías internas; delegamos a ModelJerarquia.
       • No aplicamos rotación extra; si la necesitas, cámbiala aquí.
    */
    modelRef.MovFullModel(position);        // solo traslación
    modelRef.RenderModelJ(uniformModel);    // envía cada parte al shader
}

void NPC::interact() {
    if (dialogActive) return;
    dialogActive = true;
    dialogTimer = 0.0f;
    cam->beginFocus(&position);          // ¡enfoca al NPC!
}

void NPC::update(float dt) {
    if (!dialogActive) return;
    dialogTimer += dt;
    if (dialogTimer >= DIALOG_DURATION) {
        dialogActive = false;
        cam->endFocus();                 // soltamos la vista
    }
}

void NPC::renderDialog(GLuint shaderID,
    GLuint uModel,
    GLuint uColor)
{
    if (!dialogActive) return;

    GLboolean depthWasEnabled = glIsEnabled(GL_DEPTH_TEST);
    if (depthWasEnabled) glDisable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);                        // si quieres transparencia
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderID);

    // Orto 800×600 (hard‑code rápido)
    glm::mat4 ortho = glm::ortho(0.0f, 800.f, 0.f, 600.f);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, &ortho[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, &glm::mat4(1)[0][0]);

    // --- cuadro blanco ----------
    glm::mat4 M = glm::translate(glm::mat4(1), glm::vec3(200, 430, 0))
        * glm::scale(glm::mat4(1), glm::vec3(400, 120, 1));
    glUniformMatrix4fv(uModel, 1, GL_FALSE, &M[0][0]);
    glUniform3f(uColor, 1, 1, 1);          // blanco
    quadMesh->RenderMesh();              // sin textura -> quad del mismo mesh

    // --- texto -------------------
    fontTex->UseTexture();
    float x = 220.0f;

    for (unsigned char ch : text)                      // ← unsigned char para toupper
    {
        if (ch == ' ') { x += 18; continue; }

        ch = static_cast<unsigned char>(std::toupper(ch));   // a MAYÚSCULA

        auto it = atlasRef.find(ch);
        if (it == atlasRef.end()) {          // carácter no definido
            x += 18;                         // deja un “hueco” (o quítalo)
            continue;
        }

        glm::vec2 toff = it->second;
        glUniform2fv(glGetUniformLocation(shaderID, "offset"), 1, &toff[0]);
        glUniform3f(uColor, 0, 0, 0);

        glm::mat4 M = glm::translate(glm::mat4(1), glm::vec3(x, 475, 0)) *
            glm::scale(glm::mat4(1), glm::vec3(18, 18, 1));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, &M[0][0]);
        quadMesh->RenderMesh();

        x += 18;
    }

    glUseProgram(0);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);
    if (depthWasEnabled) glEnable(GL_DEPTH_TEST);
}

