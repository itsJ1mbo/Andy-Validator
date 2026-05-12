#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

enum ValidationType { Prueba1Test, PolygonTest, VertexTest, NormalsTest};

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
    size_t index = -1;
    ModelData model;
    
    std::vector<std::pair<ValidationType, bool>> validations;
};