#pragma once
#include "Validation.h"
#include <iostream>

class PolygonValidation : public Validation
{
public:
    void validate(const FbxScene* fbx, Results& results) override;

private:

    int countPolygons(FbxNode* node);
};

