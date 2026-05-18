#pragma once
#include "Validation.h"
#include <vector>

struct PaddingUVTriangle {
    FbxVector2 v[3];
    int islandID;
    double minX, minY, maxX, maxY;
};

// Estructura auxiliar para agrupar triángulos en islas
struct DisjointSet 
{
    std::vector<int> parent;
    DisjointSet(int n) {
        parent.resize(n);
        for (int i = 0; i < n; i++) parent[i] = i;
    }
    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }
    void unite(int i, int j) {
        int rootI = find(i);
        int rootJ = find(j);
        if (rootI != rootJ) parent[rootI] = rootJ;
    }
};

class PaddingValidation : public Validation
{
public:
    void validate(const FbxScene* fbx, ModelResults& results) override;

private:
    bool checkPadding(FbxNode* node, double paddingMargin);
    void gatherAndGroupTriangles(FbxMesh* mesh, std::vector<PaddingUVTriangle>& outTriangles);

    bool shareUVVertex(const PaddingUVTriangle& t1, const PaddingUVTriangle& t2);
    bool AABBIntersectWithPadding(const PaddingUVTriangle& t1, const PaddingUVTriangle& t2, double padding);

    double pointToSegmentDistance(FbxVector2 p, FbxVector2 a, FbxVector2 b);
    double triangleDistance(const PaddingUVTriangle& t1, const PaddingUVTriangle& t2);
};