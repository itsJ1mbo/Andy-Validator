#pragma once
#include "Validation.h"

class PivotValidation :
    public Validation
{

public:
    void validate(const FbxScene* fbx, Results& results) override;

private:
    bool validPivot(FbxNode* node);


    /// <summary>
    /// devuelve el Vector4 de la posicion central global de la geometria 
    /// </summary>
    /// <param name="mesh"></param>
    /// <returns></returns>
    FbxVector4 getMeshCenter(FbxMesh* mesh);

    /// <summary>
    /// devuelve el punto mas bajo, asumiendo que las coordenadas estan en XYZ, por lo que Z es la altura
    /// </summary>
    /// <returns></returns>
    FbxVector4 getMeshBase(FbxMesh* mesh);
};

