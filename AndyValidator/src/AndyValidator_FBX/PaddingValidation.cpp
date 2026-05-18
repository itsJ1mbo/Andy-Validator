#include "AndyValidator_FBX/PaddingValidation.h"
#include "AndyValidator_FBX/FBX.h"
#include <algorithm>
#include <cmath>

void PaddingValidation::validate(const FbxScene* fbx, ModelResults& results)
{
    double paddingMargin = FBX::instance().getConfig().uvPaddingMargin;
    bool success = checkPadding(fbx->GetRootNode(), paddingMargin);

    if (!success) 
        results.allTestsPassed = false;

    ValidationResult res;
    res.type = UVPaddingTest;
    res.description = "Comprueba si las islas UV tienen suficiente margen entre ellas";
    res.passed = success;

    results.validations.emplace_back(res);
}

bool PaddingValidation::checkPadding(FbxNode* node, double paddingMargin)
{
    FbxNodeAttribute* attribute = node->GetNodeAttribute();

    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        std::vector<PaddingUVTriangle> triangles;
        gatherAndGroupTriangles(node->GetMesh(), triangles);

        for (size_t i = 0; i < triangles.size(); i++)
        {
            for (size_t j = i + 1; j < triangles.size(); j++)
            {
                //ignorar si pertenecen a la misma isla
                if (triangles[i].islandID == triangles[j].islandID)
                    continue;

                // cajas delimitadoras engordadas por el padding
                if (AABBIntersectWithPadding(triangles[i], triangles[j], paddingMargin))
                {
                    // distancia matemática real
                    if (triangleDistance(triangles[i], triangles[j]) < paddingMargin)
                    {
                        return false; // si estan demasiado cerca, test fallido
                    }
                }
            }
        }
    }

    // hijos
    for (int i = 0; i < node->GetChildCount(); i++)
    {
        if (!checkPadding(node->GetChild(i), paddingMargin)) 
            return false;
    }

    return true;
}

void PaddingValidation::gatherAndGroupTriangles(FbxMesh* mesh, std::vector<PaddingUVTriangle>& outTriangles)
{
    FbxGeometryElementUV* uvElement = mesh->GetElementUV(0);
    if (!uvElement) return;

    int polygonCount = mesh->GetPolygonCount();
    int vertexCounter = 0;

    // Extraer todos los triangulos
    for (int i = 0; i < polygonCount; i++)
    {
        int polySize = mesh->GetPolygonSize(i);
        if (polySize < 3)
        {
            vertexCounter += polySize;
            continue;
        }

        std::vector<FbxVector2> polyUVs(polySize);
        for (int j = 0; j < polySize; j++)
        {
            int uvIndex = (uvElement->GetReferenceMode() == FbxGeometryElement::eDirect) ? vertexCounter : uvElement->GetIndexArray().GetAt(vertexCounter);

            polyUVs[j] = uvElement->GetDirectArray().GetAt(uvIndex);
            vertexCounter++;
        }

        for (int j = 1; j < polySize - 1; j++)
        {
            PaddingUVTriangle tri;
            tri.v[0] = polyUVs[0];
            tri.v[1] = polyUVs[j];
            tri.v[2] = polyUVs[j + 1];

            tri.minX = std::min({ tri.v[0][0], tri.v[1][0], tri.v[2][0] });
            tri.maxX = std::max({ tri.v[0][0], tri.v[1][0], tri.v[2][0] });
            tri.minY = std::min({ tri.v[0][1], tri.v[1][1], tri.v[2][1] });
            tri.maxY = std::max({ tri.v[0][1], tri.v[1][1], tri.v[2][1] });

            outTriangles.push_back(tri);
        }
    }

    // agrupar en islas
    DisjointSet ds(outTriangles.size());
    for (size_t i = 0; i < outTriangles.size(); i++)
    {
        for (size_t j = i + 1; j < outTriangles.size(); j++)
        {
            if (shareUVVertex(outTriangles[i], outTriangles[j])) 
            {
                ds.unite(i, j);
            }
        }
    }

	// id para cada triangulo segun su isla
    for (size_t i = 0; i < outTriangles.size(); i++) {
        outTriangles[i].islandID = ds.find(i);
    }
}

// dos triangulos estan en la misma isla si comparten al menos un vertice en el UV Map
bool PaddingValidation::shareUVVertex(const PaddingUVTriangle& t1, const PaddingUVTriangle& t2)
{
    // AABB rápida para no iterar vértices a lo tonto
    if (t1.maxX < t2.minX || t1.minX > t2.maxX || t1.maxY < t2.minY || t1.minY > t2.maxY) return false;

    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            if (std::abs(t1.v[i][0] - t2.v[j][0]) < 0.00001 && std::abs(t1.v[i][1] - t2.v[j][1]) < 0.00001)
				return true;
        }
    }
    return false;
}

// comprueba si los AABB se tocan después de sumarle el margen
bool PaddingValidation::AABBIntersectWithPadding(const PaddingUVTriangle& t1, const PaddingUVTriangle& t2, double padding)
{
    if (t1.maxX + padding <= t2.minX || t1.minX - padding >= t2.maxX) 
        return false;

    if (t1.maxY + padding <= t2.minY || t1.minY - padding >= t2.maxY) 
        return false;

    return true;
}

// distancia entre un punto y segmento
double PaddingValidation::pointToSegmentDistance(FbxVector2 p, FbxVector2 a, FbxVector2 b)
{
    double l2 = std::pow(a[0] - b[0], 2) + std::pow(a[1] - b[1], 2);
    if (l2 == 0.0) 
        return std::sqrt(std::pow(p[0] - a[0], 2) + std::pow(p[1] - a[1], 2)); // a == b

    // proyeccion del punto sobre el segmento
    double t = ((p[0] - a[0]) * (b[0] - a[0]) + (p[1] - a[1]) * (b[1] - a[1])) / l2;
    t = std::max(0.0, std::min(1.0, t)); // limitar t para que no se salga del segmento

    FbxVector2 projection(a[0] + t * (b[0] - a[0]), a[1] + t * (b[1] - a[1]));

    return std::sqrt(std::pow(p[0] - projection[0], 2) + std::pow(p[1] - projection[1], 2));
}

// calcula la distancia minima entre las aristas de dos triangulos
double PaddingValidation::triangleDistance(const PaddingUVTriangle& t1, const PaddingUVTriangle& t2)
{
    double min_dist = std::numeric_limits<double>::max();

    // comprobar los 3 vertices del T1 con las 3 aristas del T2
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            double dist = pointToSegmentDistance(t1.v[i], t2.v[j], t2.v[(j + 1) % 3]);

            if (dist < min_dist) 
                min_dist = dist;
        }
    }

    // comprobar los 3 vertices del T2 con las 3 aristas del T1
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            double dist = pointToSegmentDistance(t2.v[i], t1.v[j], t1.v[(j + 1) % 3]);

            if (dist < min_dist) 
                min_dist = dist;
        }
    }

    return min_dist;
}