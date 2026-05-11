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

struct Config
{
    int polygons = 10000;
    bool unreal = false;
};

struct Results
{
    std::string name;
    size_t index;
    ModelData model;
    
    bool test1 = false;
    bool polygon = false;
    bool validFaces = false;
    bool normals = false;
    bool pruebaX = false;
    bool pruebaY = false;
};