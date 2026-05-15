#include "AndyValidator_FBX/Prueba1Validation.h"

void Prueba1Validation::validate(const FbxScene* fbx, ModelResults& results)
{
    bool validationResult = fbx->GetRootNode()->GetChildCount() > 0;
    if (!validationResult)
        results.allTestsPassed = false;
    ValidationResult res;
    res.type = Prueba1Test;
    res.description = "Test de prueba";
    res.passed = validationResult;
    results.validations.emplace_back(res);
}
