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
    std::cout << "processing pivot...\n";

    FbxNodeAttribute* attribute = node->GetNodeAttribute();
    if(attribute == nullptr)
        std::cout << "\n mecago \n";


    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        std::cout << "\nFINALMENTE";

        // coge la malla
        FbxMesh* mesh = node->GetMesh();

        // matriz placeholder
        //FbxAMatrix* matrix;

        // coge la matriz del pivote del objeto
        //FbxAMatrix* pivot = &mesh->GetPivot(*matrix);
    }

    

    //miramos todos los hijos
    for (int i = 0; i < node->GetChildCount(); ++i)
    {
        if (false) {
            return true;
        }
    }

    return true;



}
