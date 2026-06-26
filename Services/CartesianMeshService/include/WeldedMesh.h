// @otlicense
// File: WeldedMesh.h
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

#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>

class EntityFacetData;
class vector3;

namespace Geometry
{
    class Triangle;
}

class WeldedMesh
{
public:
    struct Triangle
    {
        Geometry::Triangle *triangle;
        int node[3];
    };

public:
    WeldedMesh(EntityFacetData* facetData, float weldTolerance);

    const std::vector<vector3>& getPoints() const;
    const std::vector<Triangle>& getTriangles() const;
    const std::vector<vector3>& getVertexNormals() const;

    void computeVertexNormals();
    void movePointsInward(float delta);

private:
    struct CellKey
    {
        int64_t x;
        int64_t y;
        int64_t z;

        bool operator==(const CellKey& other) const;
    };

    struct CellKeyHash
    {
        std::size_t operator()(const CellKey& k) const;
    };

private:
    static CellKey makeCellKey(const vector3& p, float cellSize);
    static float distanceSquared(const vector3& a, const vector3& b);

    int findOrCreateVertex(const vector3& p, float weldTolerance);
    void buildMesh(EntityFacetData* facetData, float weldTolerance);

private:
    std::vector<vector3> m_points;
    std::vector<Triangle> m_triangles;
    std::vector<vector3> m_vertexNormals;

    std::unordered_map<CellKey, std::vector<int>, CellKeyHash> m_cellMap;
};
