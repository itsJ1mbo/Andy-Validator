#pragma once
#include "Validation.h"

class Prueba1Validation : public Validation
{
public:
    void validate(const FbxScene* fbx, ModelResults& results) override;
};


