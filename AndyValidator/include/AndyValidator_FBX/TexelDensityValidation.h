#pragma once
#include "Validation.h"

class TexelDensityValidation : public Validation
{
public:
    void validate(const FbxScene* fbx, ModelResults& results) override;

private:
    bool checkTexelDensity(FbxNode* node, double& minDensity, double& maxDensity);
};