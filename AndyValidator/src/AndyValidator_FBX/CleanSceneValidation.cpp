#include "AndyValidator_FBX/CleanSceneValidation.h"

void CleanSceneValidation::validate(const FbxScene* fbx, Results& results)
{
    bool nodesClean = checkCleanliness(fbx->GetRootNode());
    bool layersClean = checkDisplayLayers(fbx);

    bool validationResult = (nodesClean && layersClean);

    if (!validationResult)
        results.allTestsPassed = false;

    results.validations.emplace_back(CleanSceneTest, validationResult);
}

bool CleanSceneValidation::checkDisplayLayers(const FbxScene* fbx)
{
    int layerCount = fbx->GetMemberCount<FbxDisplayLayer>(); //capas de visualizacion

    for (int i = 0; i < layerCount; ++i)
    {
        FbxDisplayLayer* layer = fbx->GetMember<FbxDisplayLayer>(i);

        if (layer->GetMemberCount() == 0)
        {
            return false; 
        }
    }

    return true;
}

bool CleanSceneValidation::checkCleanliness(FbxNode* node)
{
    FbxNodeAttribute* attribute = node->GetNodeAttribute();

    if (attribute)
    {
        FbxNodeAttribute::EType type = attribute->GetAttributeType();

        if (type == FbxNodeAttribute::eLight || type == FbxNodeAttribute::eCamera) //camra y luces
        {
            return false;
        }

        if (type == FbxNodeAttribute::eNull && node->GetChildCount() == 0) //grupos vacios
        {
            //por si el rootnode es vacio
            if (node->GetParent() != nullptr)
            {
                return false;
            }
        }
    }

    for (int i = 0; i < node->GetChildCount(); ++i)
    {
        if (!checkCleanliness(node->GetChild(i)))
        {
            return false;
        }
    }

    return true;
}