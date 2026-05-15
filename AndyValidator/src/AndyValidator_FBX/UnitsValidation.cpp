#include "AndyValidator_FBX/UnitsValidation.h"
#include "AndyValidator_FBX/FBX.h"

void UnitsValidation::validate(const FbxScene* fbx, ModelResults& results)
{
    const FbxSystemUnit sceneUnit = fbx->GetGlobalSettings().GetSystemUnit();

    Config cfg = FBX::instance().getConfig();

    bool validationResult = false;

    if (cfg.unreal)
    {
        //UE usa cm por defecto
        validationResult = (sceneUnit == FbxSystemUnit::cm);
    }
    else
    {
        //si no es UE asumimos metros
        validationResult = (sceneUnit == FbxSystemUnit::m);
    }

    if (!validationResult)
        results.allTestsPassed = false;

    ValidationResult res;
    res.type = UnitsTest;
    res.description = "Comprueba si las unidades del modelo son las mismas que las configuradas en el .cfg";
    res.passed = validationResult;

    results.validations.emplace_back(res);
}