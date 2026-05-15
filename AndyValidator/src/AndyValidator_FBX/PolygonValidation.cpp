#include "AndyValidator_FBX/PolygonValidation.h"
#include "AndyValidator_FBX/FBX.h"

void PolygonValidation::validate(const FbxScene* fbx, ModelResults& results)
{
    int totalPolygons = countPolygons(fbx->GetRootNode());
    bool validationResult = (totalPolygons <= FBX::instance().getConfig().polygons);
    if (!validationResult)
        results.allTestsPassed = false;

    ValidationResult res;
    res.type = PolygonTest;
    res.description = "Comprueba si el numero de poligonos del modelo es menor que el configurado en el .cfg";
    res.passed = validationResult;

    results.validations.emplace_back(res);
}

int PolygonValidation::countPolygons(FbxNode* node)
{
    int count = 0;

    FbxNodeAttribute* nodeAtribute = node->GetNodeAttribute();
    // Comprobamos que estamos trabajando con una malla porque si el nodo es una luz, camara, etc. peta
    if (nodeAtribute && nodeAtribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        FbxMesh* mesh = node->GetMesh();
        count += mesh->GetPolygonCount();
    }

    // Recursividad con sus hijos
    for (int i = 0; i < node->GetChildCount(); ++i)
    {
        count += countPolygons(node->GetChild(i));
    }

    return count;
}