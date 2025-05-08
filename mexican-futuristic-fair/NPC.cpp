#pragma once
#define STB_EASY_FONT_IMPLEMENTATION
#include "NPC.h"
#include <vector>

NPC::NPC(const glm::vec3& worldPos,
    const std::string& message,
    float triggerDist)
    : posW(worldPos), text(message), radius(triggerDist) {}

void NPC::initGL()
{
    buildTextGeometry();
    buildQuadGeometry();
}
void NPC::update(const glm::vec3& playerPos, bool keyR) {
    float dist = glm::length(playerPos - posW);
    if (dist < radius && keyR)          visible = !visible;   // toggle
    if (dist >= radius)                 visible = false;      // fuera de rango
}

void NPC::setShader(GLuint program)
{
    textShaderID = program;
    uProj = glGetUniformLocation(program, "projection");
    uColor = glGetUniformLocation(program, "textColor");
}

void NPC::draw(const glm::mat4& P, int W, int H) {
    if (!visible) return;

    glUseProgram(textShaderID);          // shader 2-D monocromo
    glUniformMatrix4fv(uProj, 1, GL_FALSE, &P[0][0]);

    // 1) fondo blanco (quad)
    glBindVertexArray(quadVAO);
    glUniform3f(uColor, 1, 1, 1);        // blanco
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 2) texto negro
    glBindVertexArray(vao);
    glUniform3f(uColor, 0, 0, 0);        // negro
    glDrawArrays(GL_TRIANGLES, 0, numVerts);
}


void NPC::buildTextGeometry()
{
    // 1) Generar los “quads” con stb
    static char stbBuffer[100000];          // ~5 000 letras (de sobra)
    int numQuads = stb_easy_font_print(0.0f, 0.0f,
        const_cast<char*>(text.c_str()),
        /*color*/ nullptr,
        stbBuffer, sizeof(stbBuffer));

    // 2) Convertir cada quad (4 vértices) -> 2 triángulos (6 vértices)
    std::vector<float> verts;               // x,y por vértice
    verts.reserve(numQuads * 6 * 2);

    for (int q = 0; q < numQuads; ++q) {
        float* v0 = reinterpret_cast<float*>(stbBuffer + (q * 4 + 0) * 16);
        float* v1 = reinterpret_cast<float*>(stbBuffer + (q * 4 + 1) * 16);
        float* v2 = reinterpret_cast<float*>(stbBuffer + (q * 4 + 2) * 16);
        float* v3 = reinterpret_cast<float*>(stbBuffer + (q * 4 + 3) * 16);

        // triángulo 1: v0-v1-v2
        verts.insert(verts.end(), { v0[0], v0[1], v1[0], v1[1], v2[0], v2[1] });
        // triángulo 2: v2-v1-v3
        verts.insert(verts.end(), { v2[0], v2[1], v1[0], v1[1], v3[0], v3[1] });
    }
    numVerts = static_cast<int>(verts.size() / 2);   // 2 floats por vértice

    // 3) VAO / VBO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        verts.size() * sizeof(float),
        verts.data(),
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);                 // layout(location = 0) vec2
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    // 4) Guardar dimensiones para el rectángulo de fondo
    textW = stb_easy_font_width(const_cast<char*>(text.c_str()));
    textH = stb_easy_font_height(const_cast<char*>(text.c_str()));
}

// ─────────────────────────────────────
//  Generar QUAD de fondo (triangle strip)
// ─────────────────────────────────────
void NPC::buildQuadGeometry()
{
    const float m = 4.0f;          // margen en píxeles

    float l = -m;
    float r = textW + m;
    float b = -m;
    float t = textH + m;

    float quad[4][2] = {
        { l, b },  { r, b },
        { l, t },  { r, t }
    };

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
}