#include "AndyValidator_FBX/CollidersValidation.h"
#include "AndyValidator_FBX/FBX.h"
#include <string>

void CollidersValidation::validate(const FbxScene* fbx, Results& results)
{
    Config cfg = FBX::instance().getConfig();

    bool validationResult = checkColliders(fbx->GetRootNode(), cfg.unreal);

    if (!validationResult)
        results.allTestsPassed = false;

    results.validations.emplace_back(CollidersTest, validationResult);
}

bool CollidersValidation::checkColliders(FbxNode* node, bool isUnreal)
{
    std::string nodeName = node->GetName();

    if (isUnreal)
    {
        //si el nombre del nodo empieza exactamente por "UCX_"
        if (nodeName.find("UCX_") == 0)
        {
            FbxNodeAttribute* attribute = node->GetNodeAttribute();

            //miramos si es una malla realmente
            if (!attribute || attribute->GetAttributeType() != FbxNodeAttribute::eMesh)
            {
                return false; 
            }
        }
    }

    for (int i = 0; i < node->GetChildCount(); ++i)
    {
        if (!checkColliders(node->GetChild(i), isUnreal))
        {
            return false;
        }
    }

    return true;
}