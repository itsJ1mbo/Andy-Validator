#pragma once
#include "Validation.h"
#include <iostream>

class VertexValidation : public Validation
    // No me convence el nombre, si a alguien se le ocurre uno mas top, que lo diga
{
public:
    void validate(const FbxScene* fbx, Results& results) override;

private:

    bool validPolygon(FbxNode* node);
};

