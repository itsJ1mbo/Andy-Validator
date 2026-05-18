#include "AndyValidator_FBX/ValidatorManager.h"

#include "AndyValidator_FBX/Prueba1Validation.h"
#include "AndyValidator_FBX/PolygonValidation.h"
#include "AndyValidator_FBX/VertexValidation.h"
#include "AndyValidator_FBX/NormalsValidation.h"
#include "AndyValidator_FBX/TransformationsValidation.h"
#include "AndyValidator_FBX/PivotValidation.h"
#include "AndyValidator_FBX/UnitsValidation.h"
#include "AndyValidator_FBX/NamingValidation.h"
#include "AndyValidator_FBX/CleanSceneValidation.h"
#include "AndyValidator_FBX/TexelDensityValidation.h"
#include "AndyValidator_FBX/LODValidation.h"
#include "AndyValidator_FBX/OverlappingUVValidation.h"
#include "AndyValidator_FBX/CollidersValidation.h"
#include "AndyValidator_FBX/DimensionsValidation.h"
#include "AndyValidator_FBX/PaddingValidation.h"

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
    _validations.push_back(std::make_unique<CleanSceneValidation>());
	_validations.push_back(std::make_unique<TexelDensityValidation>());
    _validations.push_back(std::make_unique<LODValidation>());
    _validations.push_back(std::make_unique<OverlappingUVValidation>());
    _validations.push_back(std::make_unique<CollidersValidation>());
    _validations.push_back(std::make_unique<DimensionsValidation>());
    _validations.push_back(std::make_unique<PaddingValidation>());
}

ValidatorManager::~ValidatorManager() = default;

void ValidatorManager::runValidations(const FbxScene* model, ModelResults& fileResults) const
{
    for (const auto& validation : _validations)
    {
        validation->validate(model, fileResults);
    }
}
