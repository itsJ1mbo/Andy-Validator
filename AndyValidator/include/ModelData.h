#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

struct Vertex 
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct TextureData 
{
    std::string filePath;
    std::string type;
};

struct MeshData 
{
    std::vector<Vertex> vertexes;
    std::vector<int> indexes;
    std::vector<TextureData> textures;
};

struct ModelData 
{
    std::vector<MeshData> meshes;
};

struct Results
{
    size_t index;
    ModelData model;
    // bool validacionX
    // bool validacionY
    // bool validacionZ
    // etc
};