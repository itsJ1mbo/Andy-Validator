#include "AndyValidator_FBX/NamingValidation.h"
#include "AndyValidator_FBX/FBX.h"  // Lo necesitamos para el config
#include <regex>

void NamingValidation::validate(const FbxScene* fbx, Results& results)
{
    Config cfg = FBX::instance().getConfig();
    std::string nameString = "";
    switch (cfg.naming)
    {
    case NamingNomenclature::UpperCamelCase:
        nameString = "[A-Z][a-zA-Z0-9]*\\.fbx$";
        break;

    case NamingNomenclature::lowerCamelCase:
        nameString = "[a-z][a-zA-Z0-9]*\\.fbx$";
        break;

    case NamingNomenclature::Upper_Snake_Case:
        nameString = "[A-Z][a-zA-Z0-9]*(_[A-Z][a-zA-Z0-9]*)*\\.fbx$";
        break;

    case NamingNomenclature::lower_snake_case:
        nameString = "[a-z0-9]+(_[a-z0-9]+)*\\.fbx$";
        break;
    }

    std::regex namingNomenclature(nameString);
    bool nameValid = std::regex_match(results.fileName, namingNomenclature);

    results.validations.push_back({ ValidationType::NamingTest, nameValid });

    if (!nameValid) results.allTestsPassed = false;
}