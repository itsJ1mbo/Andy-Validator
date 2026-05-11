#pragma once
#include "Validation.h"

#include <iostream>

class Prueba1Validation : public Validation
{
public:
    void validate(const FbxScene* fbx, Results& results) override;
};

