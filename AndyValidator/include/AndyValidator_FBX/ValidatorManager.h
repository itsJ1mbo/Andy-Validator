#pragma once

#include <vector>
#include <memory>

#include "AndyValidator_FBX/Validation.h"

class ValidatorManager 
{
public:
    ValidatorManager();

    void runValidations(const FbxScene* scene, Results& results) const;

private:
    std::vector<std::unique_ptr<Validation>> _validations;
};

