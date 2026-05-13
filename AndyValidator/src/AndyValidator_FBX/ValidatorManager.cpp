#include "AndyValidator_FBX/ValidatorManager.h"

#include "AndyValidator_FBX/Prueba1Validation.h"
#include "AndyValidator_FBX/PolygonValidation.h"
#include "AndyValidator_FBX/VertexValidation.h"
#include "AndyValidator_FBX/NormalsValidation.h"
#include "AndyValidator_FBX/TransformationsValidation.h"
#include "AndyValidator_FBX/PivotValidation.h"
#include "AndyValidator_FBX/UnitsValidation.h"
#include "AndyValidator_FBX/NamingValidation.h"

ValidatorManager::ValidatorManager()
{
    _validations.push_back(std::make_unique<Prueba1Validation>());
    _validations.push_back(std::make_unique<PolygonValidation>());
    _validations.push_back(std::make_unique<VertexValidation>());
    _validations.push_back(std::make_unique<NormalsValidation>());
    _validations.push_back(std::make_unique<TransformationsValidation>());
    _validations.push_back(std::make_unique<PivotValidation>());
    _validations.push_back(std::make_unique<UnitsValidation>());
    _validations.push_back(std::make_unique<NamingValidation>());
}

ValidatorManager::~ValidatorManager() = default;

void ValidatorManager::runValidations(const FbxScene* model, Results& fileResults) const
{
    for (const auto& validation : _validations)
    {
        validation->validate(model, fileResults);
    }
}
