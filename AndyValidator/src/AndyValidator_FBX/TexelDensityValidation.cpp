#include "AndyValidator_FBX/TexelDensityValidation.h"
#include "AndyValidator_FBX/FBX.h"
#include <cmath>
#include <limits>
#include <vector>

void TexelDensityValidation::validate(const FbxScene* fbx, ModelResults& results)
{
    double minDensity = std::numeric_limits<double>::max();
    double maxDensity = 0.0;

    bool success = checkTexelDensity(fbx->GetRootNode(), minDensity, maxDensity);

    // si maxDensity sigue siendo 0, probablemente no tiene UVs
    if (success && maxDensity > 0.0)
    {
        // tolerancia entre la zona con mas resolucion y la que menos
		double tolerance = FBX::instance().getConfig().texelDensityTolerance;

        if ((maxDensity / minDensity) > tolerance)
        {
            success = false;
        }
    }
    else
    {
        success = false; // sin UVs no podemos
    }

    if (!success) 
        results.allTestsPassed = false;

    ValidationResult res;
    res.type = TexelDensityTest;
    res.description = "Comprueba si la densidad de los texels es consistente en todo el modelo acorde a la configuracion del .cfg";
    res.passed = success;

    results.validations.emplace_back(res);
}

bool TexelDensityValidation::checkTexelDensity(FbxNode* node, double& minDensity, double& maxDensity)
{
    FbxNodeAttribute* attribute = node->GetNodeAttribute();

    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        FbxMesh* mesh = node->GetMesh();
        FbxVector4* controlPoints = mesh->GetControlPoints();
        FbxGeometryElementUV* uvElement = mesh->GetElementUV(0);

        // si la malla no tiene canal de UVs false
        if (!uvElement) 
            return false;

        int polygonCount = mesh->GetPolygonCount();
        int vertexCounter = 0;

        for (int i = 0; i < polygonCount; i++)
        {
            int polySize = mesh->GetPolygonSize(i);
            if (polySize < 3)
            {
                vertexCounter += polySize;
                continue;
            }

            std::vector<FbxVector4> pos(polySize);
            std::vector<FbxVector2> uvs(polySize);

            // Extraer vertices y UVs para esta cara
            for (int j = 0; j < polySize; j++)
            {
                int cpIndex = mesh->GetPolygonVertex(i, j);
                pos[j] = controlPoints[cpIndex];

                int uvIndex = 0;
                if (uvElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
                    uvIndex = vertexCounter;
                }
                else if (uvElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
                    uvIndex = uvElement->GetIndexArray().GetAt(vertexCounter);
                }

                uvs[j] = uvElement->GetDirectArray().GetAt(uvIndex);
                vertexCounter++;
            }

            double polyArea3D = 0.0;
            double polyAreaUV = 0.0;

            // Calcular areas
            for (int j = 1; j < polySize - 1; j++)
            {
				// producto cruzado para el area 3D
                double vx1 = pos[j][0] - pos[0][0];
                double vy1 = pos[j][1] - pos[0][1];
                double vz1 = pos[j][2] - pos[0][2];

                double vx2 = pos[j + 1][0] - pos[0][0];
                double vy2 = pos[j + 1][1] - pos[0][1];
                double vz2 = pos[j + 1][2] - pos[0][2];

                double cx = vy1 * vz2 - vz1 * vy2;
                double cy = vz1 * vx2 - vx1 * vz2;
                double cz = vx1 * vy2 - vy1 * vx2;

                polyArea3D += 0.5 * std::sqrt(cx * cx + cy * cy + cz * cz);

				// determinante para el area 2D
                double uvx1 = uvs[j][0] - uvs[0][0];
                double uvy1 = uvs[j][1] - uvs[0][1];
                double uvx2 = uvs[j + 1][0] - uvs[0][0];
                double uvy2 = uvs[j + 1][1] - uvs[0][1];

                polyAreaUV += 0.5 * std::abs(uvx1 * uvy2 - uvx2 * uvy1);
            }

			// evaluamos densidad descartando caras con area 3D muy peque
            if (polyArea3D > 0.00001)
            {
                double density = std::sqrt(polyAreaUV / polyArea3D);
                if (density < minDensity) minDensity = density;
                if (density > maxDensity) maxDensity = density;
            }
        }
    }

    // hijos
    for (int i = 0; i < node->GetChildCount(); i++)
    {
        if (!checkTexelDensity(node->GetChild(i), minDensity, maxDensity))
            return false;
    }

    return true;
}