#include "AndyValidator_FBX/DimensionsValidation.h"
#include "AndyValidator_FBX/stb_image.h"
#include <iostream>

void DimensionsValidation::validate(const FbxScene* fbx, ModelResults& results)
{
    bool validationResult = validDimensions(fbx->GetRootNode());
    if (!validationResult)
        results.allTestsPassed = false;

    ValidationResult res;
    res.type = DimensionsTest;
    res.description = "Comprueba si las dimensiones de las texturas son potencia de 2";
    res.passed = validationResult;

    results.validations.emplace_back(res);
}

bool DimensionsValidation::isTextureValid(FbxSurfaceMaterial* material)
{

    // This only gets the material of type sDiffuse, you probably need to traverse all Standard Material Property by its name to get all possible textures.
    FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

    // Check if it's layeredtextures
    int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();

    if (layeredTextureCount > 0)
    {
        for (int j = 0; j < layeredTextureCount; j++)
        {
            FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
            int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();

            for (int k = 0; k < lcount; k++)
            {
                FbxTexture* texture = FbxCast<FbxTexture>(layered_texture->GetSrcObject<FbxFileTexture>(k));

                // Then, you can get all the properties of the texture, include its name
                const char* textureName = texture->GetName();

                std::cout << textureName << std::endl;
            }
        }
         
        return false;
    }

}

bool DimensionsValidation::validDimensions(FbxNode* node)
{

    FbxNodeAttribute* attribute = node->GetNodeAttribute();
    // Comprobamos que sea malla
    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        FbxMesh* mesh = node->GetMesh();


        int materialCount = node->GetSrcObjectCount<FbxSurfaceMaterial>();

        // recorre los materiales
        for (int index = 0; index < materialCount; index++) {

            FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)node->GetSrcObject<FbxSurfaceMaterial>(index);

            // si el material existe
            if (material != NULL) {



            }

        }


    }

    // Recursividad con sus hijos
    for (int i = 0; i < node->GetChildCount(); ++i)
    {
        if (!validDimensions(node->GetChild(i)))
        {
            return false;
        }
    }

    // Si no ha fallado ningun hijo todo el fbx está bien :)
    return true;
}
