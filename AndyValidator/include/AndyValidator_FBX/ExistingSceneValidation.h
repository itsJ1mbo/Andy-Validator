#pragma once
#include "Validation.h"

class ExistingSceneValidation : public Validation
{
public:
    void validate(const FbxScene* fbx, ModelResults& results) override;
};


