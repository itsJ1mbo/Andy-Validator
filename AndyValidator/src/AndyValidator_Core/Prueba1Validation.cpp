#include "AndyValidator_FBX/Prueba1Validation.h"

void Prueba1Validation::validate(const FbxScene* fbx, Results& results)
{
    results.validations.emplace_back(Prueba1Test, fbx->GetRootNode()->GetChildCount() > 0);
}
