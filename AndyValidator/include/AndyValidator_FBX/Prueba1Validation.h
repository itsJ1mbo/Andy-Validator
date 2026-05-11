#pragma once
#include "Validation.h"

#include <iostream>

class Prueba1Validation : public Validation
{
public:
    void validate(const FbxScene* fbx, Results& results) override {
        if (fbx != nullptr && fbx->GetRootNode() != nullptr)    // Si tenemos algo que comprobarr
        {
            if (fbx->GetRootNode()->GetChildCount() > 0)    // Comprobamos si el fbx tiene hijos
            {
                results.prueba1 = true;
                std::cout << "-Prueba 1- Nicee: El fbx existe.\n";
            }
            else
            {
                results.prueba1 = false;
                std::cout << "-Prueba 1- Caca: No tiene hijos el fbx.\n";
            }
        }
    }
};

