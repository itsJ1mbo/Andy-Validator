#include "AndyValidator_FBX/PivotValidation.h"


void PivotValidation::validate(const FbxScene* fbx, Results& results)
{
    bool validationResult = validPivot(fbx->GetRootNode());
    if (!validationResult)
        results.allTestsPassed = false;
    results.validations.emplace_back(PivotTest, validationResult);


}

bool PivotValidation::validPivot(FbxNode* node)
{


	return false;
}
