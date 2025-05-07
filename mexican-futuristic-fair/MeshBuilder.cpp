#include "MeshBuilder.h"

MeshBuilder::MeshBuilder() {
    this->meshList = std::vector<Mesh*>();
    createSelectCharacterObjects();
    createSceneObjects();
}

void MeshBuilder::createSelectCharacterObjects() {
    // letras de interfaz
    unsigned int indicesLetras[] = { 0, 1, 2, 0, 2, 3 };
    GLfloat verticesLetras[] = {
        -0.5f, 0.0f,  0.5f,  0.0f, 0.8164f,   0.0f, -1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,  0.123f, 0.8164f, 0.0f, -1.0f, 0.0f,
         0.5f, 0.0f, -0.5f,  0.123f, 0.9707f, 0.0f, -1.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,  0.0f, 0.9707f,   0.0f, -1.0f, 0.0f,
    };

    Mesh *letrasMesh = new Mesh();
    letrasMesh->CreateMesh(verticesLetras, indicesLetras, 
        sizeof(verticesLetras), sizeof(indicesLetras) / sizeof(indicesLetras[0]));
    this->meshList.push_back(letrasMesh);

    // Mesh para logos
    unsigned int LogoInvencibleIndices[] = {
        0, 1, 2,
        0, 2, 3,
    };
    GLfloat LogoIncencibleVertices[] = {
        -0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
        0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
        0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
    };
    Mesh* logoInvencible = new Mesh();
    logoInvencible->CreateMesh(LogoIncencibleVertices, LogoInvencibleIndices, 32, 6);
    meshList.push_back(logoInvencible);
}

void MeshBuilder::createSceneObjects() {
    unsigned int indicesPantalla[] = { 0, 1, 2, 0, 2, 3 };
    GLfloat verticesPantalla[] = {
        -1.0f, 0.0f,  1.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
         1.0f, 0.0f,  1.5f,  0.2f, 0.0f,  0.0f, -1.0f, 0.0f,
         1.0f, 0.0f, -1.5f,  0.2f, 1.0f,  0.0f, -1.0f, 0.0f,
        -1.0f, 0.0f, -1.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
    };

    Mesh *pantallaMesh = new Mesh();
    pantallaMesh->CreateMesh(verticesPantalla, indicesPantalla, sizeof(verticesPantalla), sizeof(indicesPantalla) / sizeof(indicesPantalla[0]));
    this->meshList.push_back(pantallaMesh);
}
