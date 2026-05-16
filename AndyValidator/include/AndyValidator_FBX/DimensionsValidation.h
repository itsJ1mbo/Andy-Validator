#pragma once
#include "Validation.h"
class DimensionsValidation :
    public Validation
{


public:
    void validate(const FbxScene* fbx, ModelResults& results) override;

private:
    bool validDimensions(FbxNode* node);


    bool isTextureValid(FbxSurfaceMaterial* material);

    bool isPowerOfTwo(int n);

};

