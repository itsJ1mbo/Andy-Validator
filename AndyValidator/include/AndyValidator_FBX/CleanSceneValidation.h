#pragma once
#include "Validation.h"

class CleanSceneValidation : public Validation
{
public:
    void validate(const FbxScene* fbx, Results& results) override;

private:
    bool checkCleanliness(FbxNode* node);

    bool checkDisplayLayers(const FbxScene* fbx);
};