#pragma once

#include <fbxsdk.h>

#include "ModelData.h"

class Validation
{
public:
	virtual ~Validation() = default;

	virtual void validate(const FbxScene* fbx, ModelResults& results) = 0;
};

