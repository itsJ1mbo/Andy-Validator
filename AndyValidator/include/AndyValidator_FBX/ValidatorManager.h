#pragma once

#include <vector>
#include <memory>

#include <fbxsdk.h>
#include "ModelData.h"

class Validation;

class ValidatorManager 
{
public:
    ValidatorManager();
    ~ValidatorManager();

    void runValidations(const FbxScene* scene, Results& results) const;

private:
    std::vector<std::unique_ptr<Validation>> _validations;
};

