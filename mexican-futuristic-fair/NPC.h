#pragma once
#include <glm.hpp>
#include <string>
#include <glew.h>
// ───────── stb_easy_font ─────────────
#define STB_EASY_FONT_IMPLEMENTATION   //  ❗ solo en un TU
#include "stb_easy_font.h"
class NPC {
public:
    NPC(const glm::vec3& worldPos,
        const std::string& message,
        float triggerDist = 4.0f);

    // Llama una vez en la fase de inicialización OpenGL
    void initGL();
    void setShader(GLuint program);      //<— NUEVO


    // Llama cada frame antes de dibujar la interfaz
    // playerPos = posición del avatar en mundo
    // keyPressedR = flanco de subida de la tecla R
    void update(const glm::vec3& playerPos, bool keyPressedR);

    // Llama después de todo tu render 3D,
    // con depth-test desactivado y proyección ortográfica
    void draw(const glm::mat4& orthoProj,
        int screenW, int screenH);

private:
    glm::vec3 posW;          // posición del NPC en mundo
    std::string text;        // mensaje a mostrar
    float radius;            // distancia de activación
    bool visible = false;    // ¿muestro el diálogo este frame?
    GLuint textShaderID = 0;
    GLint  uProj = -1;
    GLint  uColor = -1;

    // recursos OpenGL para el texto
    GLuint vao = 0, vbo = 0;
    int numVerts = 0;

    // Recursos para el rectángulo de fondo
    GLuint quadVAO = 0, quadVBO = 0;

    int textW = 0, textH = 0;


    void buildTextGeometry();    // usa stb_easy_font
    void buildQuadGeometry();
};
