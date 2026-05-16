#include "AndyValidator_FBX/ImportManager.h"

#include <format>
#include <iostream>

FbxScene* ImportManager::import(const std::string& file, FbxManager* manager)
{
	FbxImporter* importer = FbxImporter::Create(manager, "");
	if (!importer)
	{
#if _DEBUG
		std::cout << "Error al crear el importador del SDK de FBX\n";
#endif
		return nullptr;
	}

	if (!importer->Initialize(file.c_str(), -1, manager->GetIOSettings()))
	{
#if _DEBUG
		std::cout << std::format("Error al inicializar el archivo FBX: {}, {}", file, importer->GetStatus().GetErrorString());
#endif
        importer->Destroy();
		return nullptr;
	}

	FbxScene* scene = FbxScene::Create(manager, "Scene");
	if (!scene)
	{
#if _DEBUG
		std::cout << "Error al crear la escena del SDK de FBX\n";
#endif
        importer->Destroy();
		return nullptr;
	}

	if (!importer->Import(scene))
	{
#if _DEBUG
		std::cout << std::format("Error al importar la escena del SDK de FBX: {}", importer->GetStatus().GetErrorString());
#endif
		scene->Destroy();
        importer->Destroy();
		return nullptr;
	}

	importer->Destroy();

	return scene;
}

ModelData ImportManager::processModel(FbxScene* scene, FbxManager* manager)
{
    ModelData mD;

    FbxGeometryConverter converter(manager);
    converter.Triangulate(scene, false);

    FbxNode* rootNode = scene->GetRootNode();
    if (rootNode) 
    {
        processNode(rootNode, mD);
    }

    return mD;
}

void ImportManager::processNode(FbxNode* node, ModelData& modelData)
{
    FbxNodeAttribute* attribute = node->GetNodeAttribute();
    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        FbxMesh* mesh = node->GetMesh();
        modelData.meshes.push_back(processMesh(mesh));

        if (node && node->GetMaterialCount() > 0)
        {
            FbxSurfaceMaterial* material = node->GetMaterial(0);
            FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

            if (prop.IsValid() && prop.GetSrcObjectCount<FbxFileTexture>() > 0)
            {
                FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>(0);
                if (texture) 
                {
                    TextureData texData;
                    texData.filePath = texture->GetFileName();
                    texData.type = "texture_diffuse";

                    modelData.meshes.back().textures.push_back(texData);
                }
            }
        }
    }

    for (int i = 0; i < node->GetChildCount(); ++i)
    {
        processNode(node->GetChild(i), modelData);
    }
}

MeshData ImportManager::processMesh(FbxMesh* mesh)
{
    MeshData meshData;

    FbxVector4* controlPoints = mesh->GetControlPoints();
    int polygonCount = mesh->GetPolygonCount();

    int vertexCounter = 0;

    for (int i = 0; i < polygonCount; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Vertex vertex;

            int cpIndex = mesh->GetPolygonVertex(i, j);
            vertex.position.x = static_cast<float>(controlPoints[cpIndex][0]);
            vertex.position.y = static_cast<float>(controlPoints[cpIndex][1]);
            vertex.position.z = static_cast<float>(controlPoints[cpIndex][2]);

            FbxGeometryElementNormal* normalElement = mesh->GetElementNormal(0);
            if (normalElement)
            {
                int normalMapIndex = 0;
                if (normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
                    normalMapIndex = vertexCounter;
                }
                else if (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
                    normalMapIndex = cpIndex;
                }

                int finalNormalIndex = normalMapIndex;
                if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
                    finalNormalIndex = normalElement->GetIndexArray().GetAt(normalMapIndex);
                }

                FbxVector4 fbxNormal = normalElement->GetDirectArray().GetAt(finalNormalIndex);

                vertex.normal.x = static_cast<float>(fbxNormal[0]);
                vertex.normal.y = static_cast<float>(fbxNormal[1]);
                vertex.normal.z = static_cast<float>(fbxNormal[2]);
            }
            else {
                vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }

            FbxGeometryElementUV* uvElement = mesh->GetElementUV(0);
            if (uvElement)
            {
                int uvMapIndex = 0;
                if (uvElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
                    uvMapIndex = vertexCounter;
                }
                else if (uvElement->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
                    uvMapIndex = cpIndex;
                }

                int finalUvIndex = uvMapIndex;
                if (uvElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
                    finalUvIndex = uvElement->GetIndexArray().GetAt(uvMapIndex);
                }

                FbxVector2 fbxUV = uvElement->GetDirectArray().GetAt(finalUvIndex);
                vertex.texCoords.x = static_cast<float>(fbxUV[0]);
                vertex.texCoords.y = static_cast<float>(fbxUV[1]);
            }
            else {
                vertex.texCoords = glm::vec2(0.0f, 0.0f);
            }

            meshData.vertexes.push_back(vertex);
            meshData.indexes.push_back(vertexCounter);

            vertexCounter++;
        }
    }

    return meshData;
}