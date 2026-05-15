#include "AndyValidator_FBX/OverlappingUVValidation.h"
#include <algorithm>

void OverlappingUVValidation::validate(const FbxScene* fbx, ModelResults& results)
{
    bool success = checkOverlapping(fbx->GetRootNode());

    if (!success) results.allTestsPassed = false;

    ValidationResult res;
    res.type = OverlappingUVTest;
    res.description = "Comprueba si hay UVs que se superpongan";
    res.passed = success;

    results.validations.emplace_back(res);
}

bool OverlappingUVValidation::checkOverlapping(FbxNode* node)
{
    FbxNodeAttribute* attribute = node->GetNodeAttribute();

    if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        std::vector<UVTriangle> triangles;
        gatherUVTriangles(node->GetMesh(), triangles);

        // algoritmo O(N^2) para comprobar choques
        for (size_t i = 0; i < triangles.size(); i++)
        {
            for (size_t j = i + 1; j < triangles.size(); j++)
            {
                // ignorar si son del mismo poligono
                if (triangles[i].polyIndex == triangles[j].polyIndex) 
                    continue;

                // Choque de bounding boxes
                if (AABBIntersect(triangles[i], triangles[j]))
                {
					// choque matematico preciso
                    if (trianglesIntersect(triangles[i], triangles[j]))
                    {
                        return false; // overlapping detectado
                    }
                }
            }
        }
    }

    for (int i = 0; i < node->GetChildCount(); i++)
    {
        if (!checkOverlapping(node->GetChild(i))) return false;
    }

    return true;
}

void OverlappingUVValidation::gatherUVTriangles(FbxMesh* mesh, std::vector<UVTriangle>& outTriangles)
{
    FbxGeometryElementUV* uvElement = mesh->GetElementUV(0);
    if (!uvElement) return;

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

        std::vector<FbxVector2> polyUVs(polySize);
        for (int j = 0; j < polySize; j++)
        {
            int uvIndex = 0;
            if (uvElement->GetReferenceMode() == FbxGeometryElement::eDirect) {
                uvIndex = vertexCounter;
            }
            else if (uvElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
                uvIndex = uvElement->GetIndexArray().GetAt(vertexCounter);
            }
            polyUVs[j] = uvElement->GetDirectArray().GetAt(uvIndex);
            vertexCounter++;
        }

        // triangulacion en abanico
        for (int j = 1; j < polySize - 1; j++)
        {
            UVTriangle tri;
            tri.v[0] = polyUVs[0];
            tri.v[1] = polyUVs[j];
            tri.v[2] = polyUVs[j + 1];
            tri.polyIndex = i;

            // calculo de la bounding box
            tri.minX = std::min({ tri.v[0][0], tri.v[1][0], tri.v[2][0] });
            tri.maxX = std::max({ tri.v[0][0], tri.v[1][0], tri.v[2][0] });
            tri.minY = std::min({ tri.v[0][1], tri.v[1][1], tri.v[2][1] });
            tri.maxY = std::max({ tri.v[0][1], tri.v[1][1], tri.v[2][1] });

            outTriangles.push_back(tri);
        }
    }
}

bool OverlappingUVValidation::AABBIntersect(const UVTriangle& t1, const UVTriangle& t2)
{
    if (t1.maxX <= t2.minX || t1.minX >= t2.maxX) return false;
    if (t1.maxY <= t2.minY || t1.minY >= t2.maxY) return false;
    return true;
}

int OverlappingUVValidation::orientation(FbxVector2 p, FbxVector2 q, FbxVector2 r)
{
    double val = (q[1] - p[1]) * (r[0] - q[0]) - (q[0] - p[0]) * (r[1] - q[1]);

    if (std::abs(val) < 0.000001) 
        return 0;

    return (val > 0) ? 1 : 2; // horario o antihorario
}

bool OverlappingUVValidation::segmentsIntersect(FbxVector2 p1, FbxVector2 q1, FbxVector2 p2, FbxVector2 q2)
{
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    if (o1 != o2 && o3 != o4) 
        return true;

    return false;
}

bool OverlappingUVValidation::pointInTriangle(FbxVector2 pt, FbxVector2 v1, FbxVector2 v2, FbxVector2 v3)
{
    double d1, d2, d3;
    bool has_neg, has_pos;

    auto sign = [](FbxVector2 p1, FbxVector2 p2, FbxVector2 p3) {
        return (p1[0] - p3[0]) * (p2[1] - p3[1]) - (p2[0] - p3[0]) * (p1[1] - p3[1]);
        };

    d1 = sign(pt, v1, v2);
    d2 = sign(pt, v2, v3);
    d3 = sign(pt, v3, v1);

    has_neg = (d1 < -0.00001) || (d2 < -0.00001) || (d3 < -0.00001);
    has_pos = (d1 > 0.00001) || (d2 > 0.00001) || (d3 > 0.00001);

    return !(has_neg && has_pos);
}

bool OverlappingUVValidation::trianglesIntersect(const UVTriangle& t1, const UVTriangle& t2)
{
    // comprobar si los bordes se cruzan
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            if (segmentsIntersect(t1.v[i], t1.v[(i + 1) % 3], t2.v[j], t2.v[(j + 1) % 3])) 
            {
                return true;
            }
        }
    }

    // comprobar si el T1 esta completamente dentro de T2
    FbxVector2 center1((t1.v[0][0] + t1.v[1][0] + t1.v[2][0]) / 3.0,
        (t1.v[0][1] + t1.v[1][1] + t1.v[2][1]) / 3.0);

    if (pointInTriangle(center1, t2.v[0], t2.v[1], t2.v[2])) 
        return true;

    // comprobar si el T2 esta dentro de T1
    FbxVector2 center2((t2.v[0][0] + t2.v[1][0] + t2.v[2][0]) / 3.0,
        (t2.v[0][1] + t2.v[1][1] + t2.v[2][1]) / 3.0);

    if (pointInTriangle(center2, t1.v[0], t1.v[1], t1.v[2])) 
        return true;

    return false;
}