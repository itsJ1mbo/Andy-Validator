#include "AndyValidator_FBX/TransformationsValidation.h"
#include <cmath> 

void TransformationsValidation::validate(const FbxScene* fbx, Results& results)
{
    //evaluamos el nodo raiz
    results.validations.emplace_back(TransformationsTest, checkTransformations(fbx->GetRootNode()));
}

bool TransformationsValidation::isZero(double value, double epsilon)
{
    return std::abs(value) <= epsilon;
}

bool TransformationsValidation::isOne(double value, double epsilon)
{
    return std::abs(value - 1.0) <= epsilon;
}

bool TransformationsValidation::checkTransformations(FbxNode* node)
{

    if (node->GetParent() != nullptr)
    {
        FbxDouble3 translation = node->LclTranslation.Get();
        FbxDouble3 rotation = node->LclRotation.Get();
        FbxDouble3 scaling = node->LclScaling.Get();

        //posicion en (0, 0, 0)
        if (!isZero(translation[0]) || !isZero(translation[1]) || !isZero(translation[2])) return false;

        //rotacion en (0, 0, 0)
        if (!isZero(rotation[0]) || !isZero(rotation[1]) || !isZero(rotation[2])) return false;

        //escala en (1, 1, 1)
        if (!isOne(scaling[0]) || !isOne(scaling[1]) || !isOne(scaling[2])) return false;
    }

    //miramos todos los hijos
    for (int i = 0; i < node->GetChildCount(); ++i)
    {
        if (!checkTransformations(node->GetChild(i)))
        {
            return false;
        }
    }

    return true;
}