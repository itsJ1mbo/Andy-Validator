#pragma once
#include "Validation.h"

class PivotValidation :
    public Validation
{

public:
    void validate(const FbxScene* fbx, Results& results) override;

private:
    bool validPivot(FbxNode* node);
};

