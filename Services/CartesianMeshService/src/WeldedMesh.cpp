// @otlicense
// File: WeldedMesh.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "WeldedMesh.h"

#include "vector3.h"

#include <cmath>
#include <functional>

#include "OTModelEntities/EntityFacetData.h"


WeldedMesh::WeldedMesh(EntityFacetData* facetData, float weldTolerance)
{
    buildMesh(facetData, weldTolerance);
}

const std::vector<vector3>& WeldedMesh::getPoints() const
{
    return m_points;
}

const std::vector<WeldedMesh::Triangle>& WeldedMesh::getTriangles() const
{
    return m_triangles;
}

bool WeldedMesh::CellKey::operator==(const CellKey& other) const
{
    return x == other.x &&
        y == other.y &&
        z == other.z;
}

std::size_t WeldedMesh::CellKeyHash::operator()(const CellKey& k) const
{
    std::size_t h1 = std::hash<int64_t>{}(k.x);
    std::size_t h2 = std::hash<int64_t>{}(k.y);
    std::size_t h3 = std::hash<int64_t>{}(k.z);

    return h1 ^ (h2 << 1) ^ (h3 << 2);
}

WeldedMesh::CellKey WeldedMesh::makeCellKey(
    const vector3& p,
    float cellSize)
{
    return CellKey{
        static_cast<int64_t>(std::floor(p.getX() / cellSize)),
        static_cast<int64_t>(std::floor(p.getY() / cellSize)),
        static_cast<int64_t>(std::floor(p.getZ() / cellSize))
    };
}

float WeldedMesh::distanceSquared(
    const vector3& a,
    const vector3& b)
{
    float dx = a.getX() - b.getX();
    float dy = a.getY() - b.getY();
    float dz = a.getZ() - b.getZ();

    return dx * dx + dy * dy + dz * dz;
}

int WeldedMesh::findOrCreateVertex(
    const vector3& p,
    float weldTolerance)
{
    CellKey baseKey = makeCellKey(p, weldTolerance);
    float tol2 = weldTolerance * weldTolerance;

    // Search current cell and all neighboring cells.
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            for (int dz = -1; dz <= 1; ++dz)
            {
                CellKey key{
                    baseKey.x + dx,
                    baseKey.y + dy,
                    baseKey.z + dz
                };

                auto it = m_cellMap.find(key);
                if (it == m_cellMap.end())
                    continue;

                for (int index : it->second)
                {
                    if (distanceSquared(p, m_points[index]) <= tol2)
                        return index;
                }
            }
        }
    }

    // Create a new vertex.
    int newIndex = static_cast<int>(m_points.size());

    m_points.push_back(p);
    m_cellMap[baseKey].push_back(newIndex);

    return newIndex;
}

void WeldedMesh::buildMesh(
    EntityFacetData* facetData,
    float weldTolerance)
{
    auto& oldNodes = facetData->getNodeVector();
    auto& oldTriangles = facetData->getTriangleList();

    m_points.reserve(oldNodes.size());
    m_triangles.reserve(oldTriangles.size());

    for (auto& triangle : oldTriangles)
    {
        Triangle newTriangle;
        newTriangle.triangle = &triangle;

        for (int i = 0; i < 3; ++i)
        {
            ot::UID oldIndex = triangle.getNode(i);

            vector3 p;
            p.setX(static_cast<float>(oldNodes[oldIndex].getCoord(0)));
            p.setY(static_cast<float>(oldNodes[oldIndex].getCoord(1)));
            p.setZ(static_cast<float>(oldNodes[oldIndex].getCoord(2)));

            newTriangle.node[i] = findOrCreateVertex(p, weldTolerance);
        }

        m_triangles.push_back(newTriangle);
    }
}

const std::vector<vector3>& WeldedMesh::getVertexNormals() const
{
    return m_vertexNormals;
}

void WeldedMesh::computeVertexNormals()
{
    m_vertexNormals.clear();
    m_vertexNormals.resize(m_points.size());

    for (const Triangle& tri : m_triangles)
    {
        int i0 = tri.node[0];
        int i1 = tri.node[1];
        int i2 = tri.node[2];

        const vector3& p0 = m_points[i0];
        const vector3& p1 = m_points[i1];
        const vector3& p2 = m_points[i2];

        vector3 e1 = p1 - p0;
        vector3 e2 = p2 - p0;

        // Area-weighted face normal.
        // This must point outward.
        vector3 faceNormal = e1.cross(e2);

        m_vertexNormals[i0] += faceNormal;
        m_vertexNormals[i1] += faceNormal;
        m_vertexNormals[i2] += faceNormal;
    }

    for (vector3& n : m_vertexNormals)
    {
        n.normalize();
    }
}

void WeldedMesh::movePointsInward(float delta)
{
    if (m_vertexNormals.size() != m_points.size())
    {
        computeVertexNormals();
    }

    for (size_t i = 0; i < m_points.size(); ++i)
    {
        // Move against the outward vertex normal.
        m_points[i] = m_points[i] - m_vertexNormals[i] * delta;
    }
}
