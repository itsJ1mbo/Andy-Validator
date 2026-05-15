#pragma once
#include "Validation.h"
#include <vector>

struct UVTriangle {
    FbxVector2 v[3];
    int polyIndex; // Para saber a que poligono 3D pertenece
    double minX, minY, maxX, maxY; // bounding box (AABB)
};

class OverlappingUVValidation : public Validation
{
public:
    void validate(const FbxScene* fbx, ModelResults& results) override;

private:
    bool checkOverlapping(FbxNode* node);
    void gatherUVTriangles(FbxMesh* mesh, std::vector<UVTriangle>& outTriangles);

    // mates
    bool trianglesIntersect(const UVTriangle& t1, const UVTriangle& t2);
    bool AABBIntersect(const UVTriangle& t1, const UVTriangle& t2);
    int orientation(FbxVector2 p, FbxVector2 q, FbxVector2 r);
    bool segmentsIntersect(FbxVector2 p1, FbxVector2 q1, FbxVector2 p2, FbxVector2 q2);
    bool pointInTriangle(FbxVector2 pt, FbxVector2 v1, FbxVector2 v2, FbxVector2 v3);
};