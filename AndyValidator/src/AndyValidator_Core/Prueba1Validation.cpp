#include "AndyValidator_FBX/Prueba1Validation.h"

void Prueba1Validation::validate(const FbxScene* fbx, Results& results)
{
    bool validationResult = fbx->GetRootNode()->GetChildCount() > 0;
    if (!validationResult)
        results.allTestsPassed = false;
    results.validations.emplace_back(Prueba1Test, validationResult);
}
