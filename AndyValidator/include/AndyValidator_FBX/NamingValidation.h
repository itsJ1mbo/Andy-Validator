#pragma once
#include "Validation.h"

class NamingValidation : public Validation
{
public:
    void validate(const FbxScene* fbx, Results& results) override;
};

