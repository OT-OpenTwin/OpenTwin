// @otlicense
// File: SelfIntersectionCheck.h
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
#include<vector>
class SelfIntersectionCheck
{
public:
	SelfIntersectionCheck();
	virtual ~SelfIntersectionCheck();
	void clear(void);
	void setNodes(uint32_t nTag, double CoordsX, double CoordsY, double CoordsZ);
	void setTriangles(uint32_t faceIndex, uint32_t Tid, uint32_t node1, uint32_t node2, uint32_t node3);
	bool IsSelfIntersecting();
	std::vector < std::tuple<double, double, double, uint32_t, uint32_t> > TrisSelfIntersecting();
	std::vector<std::tuple<double, double, double, uint32_t, uint32_t>> pointTriming(std::vector<std::tuple<double, double, double, uint32_t, uint32_t>> IntersectingInfo);
	
private:
	std::vector < std::tuple<double, double, double, uint32_t, uint32_t>> TrimmedPoints;
	std::map<uint32_t, uint32_t> mapNodeVertex;//mapping gmsh node-Tag to cgal Vertex ID
	std::map<uint32_t, uint32_t> mapTrianglesFaces; //mapping gmsh Triangles to cgal Face
	std::map<uint32_t, uint32_t> mapTrianglesTags;//mapping gmsh Triangles to  gmsh Faces
};
