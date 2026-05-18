#include "AndyValidator_FBX/PivotValidation.h"

void PivotValidation::validate(const FbxScene* fbx, ModelResults& results)
{
    bool validationResult = validPivot(fbx->GetRootNode());
    if (!validationResult)
        results.allTestsPassed = false;

    ValidationResult res;
    res.type = PivotTest;
    res.description = "Comprueba si el punto de pivote esta en la base o en el centro de masa del objeto";
    res.passed = validationResult;

    results.validations.emplace_back(res);
}

bool PivotValidation::validPivot(FbxNode* node)
{
    bool valid = false;

    if (node->GetParent() != nullptr) {

        FbxNodeAttribute* attribute = node->GetNodeAttribute();

        if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            // coge la malla
            FbxMesh* mesh = node->GetMesh();

            // posicion del pivote
            FbxDouble3 pivotPosition = node->LclTranslation.Get();

            // posicion del centro
            FbxDouble3 centerPosition = getMeshCenter(mesh);

            // posicion de la base (plano valido para la base)
            FbxDouble3 deepestPosition = getMeshBase(mesh);

            // si esta en el centro ya esta bien
            if (pivotPosition == centerPosition) {
                return true;
            }

            // si esta en la base
            if ((pivotPosition[2] == 0 && deepestPosition[2] == 0))
                return true;
            
            // si ha llegado hasta aqui es porque ha suspendido
            return false;
        }
    }

    //miramos todos los hijos
    for (int i = 0; i < node->GetChildCount(); ++i)
    {
        if (!validPivot(node->GetChild(i)))
        {
            return false;
        }
    }

    return true;
}

FbxVector4 PivotValidation::getMeshCenter(FbxMesh* mesh)
{

    int polygonCount = mesh->GetPolygonCount();
    FbxVector4* controlPoints = mesh->GetControlPoints();

    FbxVector4 center(0, 0, 0, 0);

    // recorre las caras
    for (int i = 0; i < polygonCount; ++i) 
    {
        int polygonSize = mesh->GetPolygonSize(i);

        // recorre los vertices
        for (int j = 0; j < polygonSize; j++) { 

            int controlPointIndex = mesh->GetPolygonVertex(i, j);

            // calcula la posicion global (para que no sea relativa al pivot)
            FbxVector4 a = (controlPoints[controlPointIndex] + mesh->GetNode()->LclTranslation.Get());

            center += a;
        }
    }

    // calcula el centro de todos los puntos
    center /= mesh->GetPolygonVertexCount();

    return center;

}

FbxVector4 PivotValidation::getMeshBase(FbxMesh* mesh)
{
    int polygonCount = mesh->GetPolygonCount();
    FbxVector4* controlPoints = mesh->GetControlPoints();

    FbxVector4 deepest = controlPoints[0];

    for (int i = 0; i < polygonCount; ++i)  
    {
        int polygonSize = mesh->GetPolygonSize(i);

        for (int j = 0; j < polygonSize; j++) {

            int controlPointIndex = mesh->GetPolygonVertex(i, j);

            // calcula la posicion global (para que no sea relativa al pivot)
            FbxVector4 a = (controlPoints[controlPointIndex] + mesh->GetNode()->LclTranslation.Get());

            // busca la posicion mas baja (sistema XYZ(Z es altura))
            if (a[2] < deepest[2])
                deepest = a;
        }
    }

    return deepest;
}
