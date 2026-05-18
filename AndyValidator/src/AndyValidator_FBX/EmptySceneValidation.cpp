#include "AndyValidator_FBX/EmptySceneValidation.h"

void EmptySceneValidation::validate(const FbxScene* fbx, ModelResults& results)
{
    bool validationResult = fbx->GetRootNode()->GetChildCount() > 0;
    if (!validationResult)
        results.allTestsPassed = false;
    ValidationResult res;
    res.type = EmptySceneTest;
    res.description = "Comprueba si la escena contiene elementos y no esta vacia";
    res.passed = validationResult;
    results.validations.emplace_back(res);
}