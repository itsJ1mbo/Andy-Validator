#pragma once
#include "Validation.h"

class PolygonValidation : public Validation
{
public:
    void validate(const FbxScene* fbx, ModelResults& results) override;

private:
    int countPolygons(FbxNode* node);
};

