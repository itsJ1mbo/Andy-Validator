#include "AndyValidator_FBX/DimensionsValidation.h"
#include <cmath>
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

    if (textureCount > 0)
    {
        for (int j = 0; j < textureCount; j++)
        {
            // saca la textura 
            FbxTexture* texture = FbxCast<FbxTexture>(prop.GetSrcObject<FbxFileTexture>());

            // saca el fileTexture para conseguir el file
            FbxFileTexture* file = FbxCast<FbxFileTexture>(texture);
            if (file != nullptr) {

                // recoge el path
                FbxString path = file->GetFileName();

                // recoge la info de la imagen
                int x, y, comp;
                stbi_info(path, &x, &y, &comp);

                // si ambas dimensiones son potencia de 2 devuelve true
                return isPowerOfTwo(x) && isPowerOfTwo(y);
            }
        }
    }
    
    // si no hay textura esta bien
    return true;
    

}

bool DimensionsValidation::isPowerOfTwo(int n)
{
    if(n <= 0)
        return false;  

    // calculamos la base del logaritmo de 2
    int logValue = log2(n);

    // confirmamos si sirve como potencia
    return pow(2, logValue) == n;

}

bool DimensionsValidation::validDimensions(FbxNode* node)
{

    FbxNodeAttribute* attribute = node->GetNodeAttribute();
    // Comprobamos que sea malla
    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        // recoge el numero de materiales por si hay varios
        int materialCount = node->GetSrcObjectCount<FbxSurfaceMaterial>();

        // recorre los materiales
        for (int index = 0; index < materialCount; index++) {

            // recoge el material
            FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)node->GetSrcObject<FbxSurfaceMaterial>(index);

            // si el material existe
            if (material != NULL) {

                // si no es valida corta y devuelve false, si es valida sigue
                if(!isTextureValid(material))
                    return false;

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
