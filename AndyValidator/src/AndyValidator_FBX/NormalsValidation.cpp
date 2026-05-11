#include "AndyValidator_FBX/NormalsValidation.h"
#include <iostream>

void NormalsValidation::validate(const FbxScene* fbx, Results& results)
{
    results.normals = checkNormals(fbx->GetRootNode());
}

bool NormalsValidation::checkNormals(FbxNode* node)
{

    return true;
}