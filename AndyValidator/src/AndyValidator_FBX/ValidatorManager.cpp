#include "AndyValidator_FBX/ValidatorManager.h"

#include "AndyValidator_FBX/Prueba1Validation.h"
#include "AndyValidator_FBX/PolygonValidation.h"
#include "AndyValidator_FBX/VertexValidation.h"
#include "AndyValidator_FBX/NormalsValidation.h"

ValidatorManager::ValidatorManager()
{
    _validations.push_back(std::make_unique<Prueba1Validation>());
    _validations.push_back(std::make_unique<PolygonValidation>());
    _validations.push_back(std::make_unique<VertexValidation>());
    _validations.push_back(std::make_unique<NormalsValidation>());
}

ValidatorManager::~ValidatorManager() = default;

void ValidatorManager::runValidations(const FbxScene* model, Results& fileResults) const
{
    for (const auto& validation : _validations)
    {
        validation->validate(model, fileResults);
    }
}
