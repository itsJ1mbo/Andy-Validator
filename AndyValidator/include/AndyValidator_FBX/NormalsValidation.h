#pragma once
#include "Validation.h"

class NormalsValidation : public Validation
{
public:
    void validate(const FbxScene* fbx, ModelResults& results) override;

private:
    bool checkNormals(FbxNode* node);
};