#pragma once

#include <fbxsdk.h>

struct ValidationResults
{
	size_t index;
	// bool validacionX
	// bool validacionY
	// bool validacionZ
	// etc
};

class Validation
{
public:
	virtual ~Validation() = default;

	virtual void validate(const FbxScene* fbx, ValidationResults& results) = 0;
};

