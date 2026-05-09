#pragma once

#include <string>

#include <fbxsdk.h>

#include "ModelData.h"

class ImportManager
{
public:
	FbxScene* import(const std::string& file, FbxManager* manager);
	ModelData processModel(FbxScene* scene, FbxManager* manager);

private:
	void processNode(FbxNode* node, ModelData& modelData);
	MeshData processMesh(FbxMesh* mesh);
};

