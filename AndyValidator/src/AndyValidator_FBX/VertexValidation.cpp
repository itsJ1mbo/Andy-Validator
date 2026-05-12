#include "AndyValidator_FBX/VertexValidation.h"

void VertexValidation::validate(const FbxScene* fbx, Results& results)
{
    results.validations.emplace_back(_validationType, validPolygon(fbx->GetRootNode()));
}

bool VertexValidation::validPolygon(FbxNode* node)
{
    FbxNodeAttribute* attribute = node->GetNodeAttribute();
    // Comprobamos que sea malla
    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        FbxMesh* mesh = node->GetMesh();
        int polygonCount = mesh->GetPolygonCount();

        for (int i = 0; i < polygonCount; ++i)  // Vemos las caras
        {
            if (mesh->GetPolygonSize(i) != 3)    // Comprobamos vertices de la cara
            {
                return false;
            }
        }
    }

    // Recursividad con sus hijos
    for (int i = 0; i < node->GetChildCount(); ++i)
    {
        if (!validPolygon(node->GetChild(i)))
        {
            return false;
        }
    }

    // Si no ha fallado ningun hijo todo el fbx está bien :)
    return true;
}
