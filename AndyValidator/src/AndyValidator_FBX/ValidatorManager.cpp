#include "AndyValidator_FBX/ValidatorManager.h"

ValidatorManager::ValidatorManager()
{
    // _validations.push_back(std::make_unique<DummyValidation>());
}

void ValidatorManager::runValidations(const FbxScene* model, Results& fileResults) const
{
    for (const auto& validation : _validations)
    {
        validation->validate(model, fileResults);
    }
}
