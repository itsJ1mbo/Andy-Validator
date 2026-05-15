#include "AndyValidator_FBX/NamingValidation.h"
#include "AndyValidator_FBX/FBX.h"  // Lo necesitamos para el config
#include <regex>

void NamingValidation::validate(const FbxScene* fbx, ModelResults& results)
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

    if (!nameValid)
        results.allTestsPassed = false;

    ValidationResult res;
    res.type = NamingTest;
    res.description = "Comprueba si el archivo sigue el criterio de nombres";
    res.passed = nameValid;

    results.validations.push_back(res);
}