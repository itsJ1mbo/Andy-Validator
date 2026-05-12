#pragma once

#include <fbxsdk.h>

#include "ModelData.h"

class Validation
{
public:
	virtual ~Validation() = default;

	virtual void validate(const FbxScene* fbx, Results& results) = 0;
protected:
	ValidationType _validationType;
};

