#include "AndyValidator_FBX/NormalsValidation.h"

#include <map>

void NormalsValidation::validate(const FbxScene* fbx, Results& results)
{
    results.validations.push_back({ _validationType, checkNormals(fbx->GetRootNode()) });
}

bool NormalsValidation::checkNormals(FbxNode* node)
{
    FbxNodeAttribute* attribute = node->GetNodeAttribute();

    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        FbxMesh* mesh = node->GetMesh();
        int polygonCount = mesh->GetPolygonCount();

        //guardamos aristas y la direccion en la que se recorren
        std::map<std::pair<int, int>, int> edges;

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
                if (edges.contains(currentEdge))
                {
                    return false;
                }

                // guardamos la arista
                edges[currentEdge] = i;
            }
        }
    }

    // hijos
    for (int i = 0; i < node->GetChildCount(); i++)
    {
        if (!checkNormals(node->GetChild(i))) return false;
    }

    return true;
}