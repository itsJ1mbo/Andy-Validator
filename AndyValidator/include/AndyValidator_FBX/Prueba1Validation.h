#pragma once
#include "Validation.h"

class Prueba1Validation : public Validation
{
public:
    Prueba1Validation()
    {
        _validationType = Prueba1Test;
    }
    void validate(const FbxScene* fbx, Results& results) override;
};


