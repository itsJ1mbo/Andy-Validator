#include "AndyValidator_FBX/DimensionsValidation.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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

    // cuenta el numero de texturas que tiene
    int textureCount = prop.GetSrcObjectCount<FbxTexture>();
    //std::cout << "hola " << layeredTextureCount << "\n";

    if (textureCount > 0)
    {
        for (int j = 0; j < textureCount; j++)
        {
            // saca la textura y su nombre
            FbxTexture* texture = FbxCast<FbxTexture>(prop.GetSrcObject<FbxFileTexture>());
            const char* textureName = texture->GetName();

            FbxFileTexture* file = FbxCast<FbxFileTexture>(texture);
            if (file != nullptr) {
                FbxString path = file->GetFileName();

                std::cout << "hola " << path << "\n";
            

                int x = 0, y = 0, comp, req = 0;
                stbi_load(textureName, &x, &y, &comp, req);
            
            }


            //FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
            //int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();

            //for (int k = 0; k < lcount; k++)
            //{
            //    FbxTexture* texture = FbxCast<FbxTexture>(layered_texture->GetSrcObject<FbxFileTexture>(k));

            //    // Then, you can get all the properties of the texture, include its name
            //    const char* textureName = texture->GetName();

            //    std::cout << textureName << std::endl;
            //}
        }
    }
         
        return false;
    

}

bool DimensionsValidation::validDimensions(FbxNode* node)
{

    FbxNodeAttribute* attribute = node->GetNodeAttribute();
    // Comprobamos que sea malla
    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        FbxMesh* mesh = node->GetMesh();

        //FbxLayerElementArrayTemplate<FbxVector2>** a;

        //mesh->GetTextureUV(a, FbxLayerElement::eTextureDiffuse);

        int materialCount = node->GetSrcObjectCount<FbxSurfaceMaterial>();

        // recorre los materiales
        for (int index = 0; index < materialCount; index++) {

            FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)node->GetSrcObject<FbxSurfaceMaterial>(index);

            // si el material existe
            if (material != NULL) {

                return isTextureValid(material);

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
