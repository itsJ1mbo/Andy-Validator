#pragma once
#include "Validation.h"

class TransformationsValidation : public Validation
{
public:
    void validate(const FbxScene* fbx, ModelResults& results) override;

private:
    bool checkTransformations(FbxNode* node);

    //funciones auxiliares para comparar decimales
    bool isZero(double value, double epsilon = 1e-5);
    bool isOne(double value, double epsilon = 1e-5);
};