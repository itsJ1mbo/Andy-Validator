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
    PivotTest,
    UnitsTest,
    NamingTest,
    CleanSceneTest,
    TexelDensityTest,
    LODTest,
    OverlappingUVTest
};

enum class NamingNomenclature
{
    UpperCamelCase,
    lowerCamelCase,
    Upper_Snake_Case,
    lower_snake_case
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
    NamingNomenclature naming = NamingNomenclature::UpperCamelCase;
};

struct Results
{
    std::optional<size_t> index;
    std::string fileName;
    ModelData model;
    bool allTestsPassed = true;
    std::vector<std::pair<ValidationType, bool>> validations;
};