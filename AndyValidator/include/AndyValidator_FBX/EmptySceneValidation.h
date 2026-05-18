#pragma once
#include "Validation.h"

class EmptySceneValidation : public Validation
{
public:
    void validate(const FbxScene* fbx, ModelResults& results) override;
};


