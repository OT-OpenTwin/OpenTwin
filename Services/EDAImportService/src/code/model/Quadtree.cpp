// @otlicense
// File: Quadtree.cpp
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

#include "Quadtree.h"

Quadtree::Quadtree(double x, double y, double width, double height)
    : root(std::make_shared<QuadtreeNode>(x, y, width, height)) {}

void Quadtree::insert(BRepBuilderAPI_MakeFace& face) {
    root->insert(face);
}

void Quadtree::insert(std::list<BRepBuilderAPI_MakeFace>& faces) {
    for (auto& face : faces)
    {
        insert(face);
    }
}

TopoDS_Shape Quadtree::fuse() {
    return root->fuse();
}
