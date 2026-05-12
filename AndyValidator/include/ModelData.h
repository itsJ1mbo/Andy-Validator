#pragma once

#include <optional>
#include <vector>
#include <string>

#include <glm/glm.hpp>

enum ValidationType { 
    None,
    Prueba1Test,
    PolygonTest, 
    VertexTest, 
    NormalsTest, 
    TransformationsTest,
    UnitsTest
};

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
    std::optional<size_t> index;
    ModelData model;
    bool allTestsPassed = true;
    std::vector<std::pair<ValidationType, bool>> validations;
};