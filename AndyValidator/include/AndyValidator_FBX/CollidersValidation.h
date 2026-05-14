#pragma once
#include "Validation.h"

class CollidersValidation : public Validation
{
public:
    void validate(const FbxScene* fbx, Results& results) override;

private:
    bool checkColliders(FbxNode* node, bool isUnreal);
};