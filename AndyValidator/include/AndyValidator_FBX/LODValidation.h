#pragma once
#include "Validation.h"

class LODValidation : public Validation
{
public:
    void validate(const FbxScene* fbx, ModelResults& results) override;

private:
    bool checkLODNames(FbxNode* node);
};