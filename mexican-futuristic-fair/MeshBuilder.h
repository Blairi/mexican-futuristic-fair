#pragma once
#include "Mesh.h"
#include "vector"

class MeshBuilder {
public:
    MeshBuilder();

private:
    void createSelectCharacterObjects();
    void createSceneObjects();

public:
    std::vector<Mesh*> meshList;
};
