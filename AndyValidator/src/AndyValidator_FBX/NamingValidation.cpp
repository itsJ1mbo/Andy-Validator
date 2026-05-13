#include "AndyValidator_FBX/NamingValidation.h"

#include <regex>

void NamingValidation::validate(const FbxScene* fbx, Results& results)
{
    // Comprobamos si tenemos caracteres válidos y acaba en fbx
    std::regex namingPattern("^[a-zA-Z0-9_]+\\.fbx$");

    bool nameValid = std::regex_match(results.fileName, namingPattern);

    results.validations.push_back({ ValidationType::NamingTest, nameValid });

    if (!nameValid) results.allTestsPassed = false;
}
