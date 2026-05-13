#include "AndyValidator_FBX/PivotValidation.h"

#include <iostream>

void PivotValidation::validate(const FbxScene* fbx, Results& results)
{
    bool validationResult = validPivot(fbx->GetRootNode());
    if (!validationResult)
        results.allTestsPassed = false;
    results.validations.emplace_back(PivotTest, validationResult);
}

bool PivotValidation::validPivot(FbxNode* node)
{
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


            // debug
            //std::cout << "pivot position: " << pivotPosition[0] << " " << pivotPosition[1] << " " << pivotPosition[2] << "\n";

        
            // si NO esta en el centro
            if (pivotPosition != centerPosition)
                return false;


            // si NO esta en la base



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

    for (int i = 0; i < polygonCount; ++i)  // Vemos las caras
    {
        int polygonSize = mesh->GetPolygonSize(i);

        for (int j = 0; j < polygonSize; j++) {

            int controlPointIndex = mesh->GetPolygonVertex(i, j);

            // calcula la posicion global (para que no sea relativa al pivot)
            FbxVector4 a = (controlPoints[controlPointIndex] + mesh->GetNode()->LclTranslation.Get());
            //std::cout << "center position: " << a[0] << " " << a[1] << " " << a[2] << "\n";

            center += a;

        }

    }

    center /= mesh->GetPolygonVertexCount();
    //std::cout << "center position: " << center[0] << " " << center[1] << " " << center[2] << "\n";

    return center;

}
