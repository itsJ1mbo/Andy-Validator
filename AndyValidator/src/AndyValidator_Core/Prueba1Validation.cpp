#include "AndyValidator_FBX/Prueba1Validation.h"

void Prueba1Validation::validate(const FbxScene* fbx, Results& results)
{
    if (fbx->GetRootNode()->GetChildCount() > 0)    // Comprobamos si el fbx tiene hijos
    {
        results.test1 = true;
    }
    else
    {
        results.test1 = false;
    }
}
