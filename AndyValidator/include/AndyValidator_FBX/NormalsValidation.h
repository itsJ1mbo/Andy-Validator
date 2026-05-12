#pragma once
#include "Validation.h"

class NormalsValidation : public Validation
{
public:
	NormalsValidation()
	{
		_validationType = NormalsTest;
	}
    void validate(const FbxScene* fbx, Results& results) override;

private:
    bool checkNormals(FbxNode* node);
};