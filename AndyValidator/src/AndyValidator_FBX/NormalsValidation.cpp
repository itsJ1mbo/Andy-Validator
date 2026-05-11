#include "AndyValidator_FBX/NormalsValidation.h"
#include <map>
#include <utility>

void NormalsValidation::validate(const FbxScene* fbx, Results& results)
{
    results.normals = checkNormals(fbx->GetRootNode());
}

bool NormalsValidation::checkNormals(FbxNode* node)
{
    FbxNodeAttribute* attribute = node->GetNodeAttribute();

    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        FbxMesh* mesh = node->GetMesh();
        int polygonCount = mesh->GetPolygonCount();
        FbxVector4* controlPoints = mesh->GetControlPoints();

        //guardamos aristas y la direccion en la que se recorren
        std::map<std::pair<int, int>, int> edges;

        double signedVolume = 0.0;

        for (int i = 0; i < polygonCount; i++)
        {
            int polySize = mesh->GetPolygonSize(i);
            if (polySize < 3) continue;

            // detectar caras individuales invertidas
            for (int j = 0; j < polySize; j++)
            {
                int v0 = mesh->GetPolygonVertex(i, j);
                int v1 = mesh->GetPolygonVertex(i, (j + 1) % polySize);

                std::pair<int, int> currentEdge(v0, v1);

                // caras flippeadas
                if (edges.find(currentEdge) != edges.end())
                {
                    return false;
                }

                // guardamos la arista
                edges[currentEdge] = i;
            }

            // calculamos volumen para ver si el objeto entero esta del reves
            FbxVector4 p0 = controlPoints[mesh->GetPolygonVertex(i, 0)];
            for (int j = 1; j < polySize - 1; j++)
            {
                FbxVector4 p1 = controlPoints[mesh->GetPolygonVertex(i, j)];
                FbxVector4 p2 = controlPoints[mesh->GetPolygonVertex(i, j + 1)];

                signedVolume += p0.DotProduct(p1.CrossProduct(p2)) / 6.0;
            }
        }

        // si el volumen total es negativo el modelo esta del reves
        if (signedVolume < -0.0001)
        {
            return false;
        }
    }

    // hijos
    for (int i = 0; i < node->GetChildCount(); i++)
    {
        if (!checkNormals(node->GetChild(i))) return false;
    }

    return true;
}