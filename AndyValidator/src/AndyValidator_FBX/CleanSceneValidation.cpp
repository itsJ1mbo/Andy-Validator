#include "AndyValidator_FBX/CleanSceneValidation.h"

void CleanSceneValidation::validate(const FbxScene* fbx, ModelResults& results)
{
    bool nodesClean = checkCleanliness(fbx->GetRootNode());
    bool layersClean = checkDisplayLayers(fbx);

    bool validationResult = (nodesClean && layersClean);

    if (!validationResult)
        results.allTestsPassed = false;

    ValidationResult res;
    res.type = CleanSceneTest;
    res.description = "Comprueba si se han borrado historial, capas vacías, luces o cámaras que se hayan quedado en el archivo";
    res.passed = validationResult;

    results.validations.emplace_back(res);
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