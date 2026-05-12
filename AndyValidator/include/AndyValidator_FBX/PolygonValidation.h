#pragma once
#include "Validation.h"

class PolygonValidation : public Validation
{
public:
    PolygonValidation()
    {
        _validationType = PolygonTest;
    }
    void validate(const FbxScene* fbx, Results& results) override;

private:

    int countPolygons(FbxNode* node);
};

