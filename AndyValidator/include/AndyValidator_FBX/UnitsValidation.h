#pragma once
#include "Validation.h"

class UnitsValidation : public Validation
{
public:

    void validate(const FbxScene* fbx, Results& results) override;
};