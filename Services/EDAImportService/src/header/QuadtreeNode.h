// @otlicense
// File: QuadtreeNode.h
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

#ifndef QUADTREENODE_H
#define QUADTREENODE_H

#include <vector>
#include <memory>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepAlgoAPI_Fuse.hxx>

class QuadtreeNode {
public:
    QuadtreeNode(double x, double y, double width, double height);

    void insert(BRepBuilderAPI_MakeFace& face);
    TopoDS_Shape fuse();

private:
    bool contains(BRepBuilderAPI_MakeFace& face);
    void subdivide();
    bool intersects(double qx, double qy, double qwidth, double qheight);
    bool contains_in_query(BRepBuilderAPI_MakeFace& face, double qx, double qy, double qwidth, double qheight);

    std::vector<BRepBuilderAPI_MakeFace> faces;
    std::vector<std::shared_ptr<QuadtreeNode>> children;
    double x, y, width, height;
    bool divided = false;
    static const int max_faces;
    static const double min_size;
};

#endif // QUADTREENODE_H
