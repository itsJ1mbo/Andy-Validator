#include "AndyValidator_FBX/LODValidation.h"

#include <regex>
#include <string>

void LODValidation::validate(const FbxScene* fbx, Results& results)
{
    bool lodValid = checkLODNames(fbx->GetRootNode());  // Llamamos que compruebe todos los nodos

    results.validations.push_back({ ValidationType::LODTest, lodValid });

    if (!lodValid) results.allTestsPassed = false;
}

bool LODValidation::checkLODNames(FbxNode* node)
{
    FbxNodeAttribute* attribute = node->GetNodeAttribute();

    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        std::string nodeName = node->GetName();

        std::string upperName = nodeName;
        for (auto& c : upperName) c = toupper(c);   // Pasamos a mayusculas para no saltarnos los "lod3_obstaculo"

        if (upperName.find("LOD") != std::string::npos) // Estamos en un nodo de LOD
        {
            std::regex lodFormat("^LOD[0-9]+_.*$"); // Formato que deseamos

            // Comprobamos
            if (!std::regex_match(nodeName, lodFormat)) return false;
        }
    }

    // Recursividad
    for (int i = 0; i < node->GetChildCount(); ++i)
    {
        if (!checkLODNames(node->GetChild(i)))
        {
            return false;
        }
    }

    return true;
}