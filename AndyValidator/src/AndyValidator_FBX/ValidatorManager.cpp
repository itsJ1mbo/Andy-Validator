#include "AndyValidator_FBX/ValidatorManager.h"

#include "AndyValidator_FBX/Prueba1Validation.h"    // Validacion de prueba

ValidatorManager::ValidatorManager()
{
    _validations.push_back(std::make_unique<Prueba1Validation>());
}

ValidatorManager::~ValidatorManager() = default;

void ValidatorManager::runValidations(const FbxScene* model, Results& fileResults) const
{
    for (const auto& validation : _validations)
    {
        validation->validate(model, fileResults);
    }
}
