// @otlicense
// File: QuadtreeNode.cpp
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

#include "QuadtreeNode.h"
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>

const int QuadtreeNode::max_faces = 4;
const double QuadtreeNode::min_size = 1.0;

QuadtreeNode::QuadtreeNode(double x, double y, double width, double height)
    : x(x), y(y), width(width), height(height) {}

void QuadtreeNode::insert(BRepBuilderAPI_MakeFace& face) {
    if (faces.size() < max_faces || (width <= min_size && height <= min_size)) {
        faces.push_back(face);
    }
    else {
        if (!divided) subdivide();
        for (auto& child : children) {
            if (child->contains(face)) {
                child->insert(face);
                return;
            }
        }
        faces.push_back(face); // If it doesn't fit perfectly in any child, store it here.
    }
}

bool QuadtreeNode::contains(BRepBuilderAPI_MakeFace& face) {
    Bnd_Box bbox;
    BRepBndLib::Add(face.Shape(), bbox);
    double xmin, ymin, zmin, xmax, ymax, zmax;
    bbox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    return (xmin >= x && xmax <= x + width && ymin >= y && ymax <= y + height);
}

void QuadtreeNode::subdivide() {
    double halfWidth = width / 2.0;
    double halfHeight = height / 2.0;
    children.push_back(std::make_shared<QuadtreeNode>(x, y, halfWidth, halfHeight));
    children.push_back(std::make_shared<QuadtreeNode>(x + halfWidth, y, halfWidth, halfHeight));
    children.push_back(std::make_shared<QuadtreeNode>(x, y + halfHeight, halfWidth, halfHeight));
    children.push_back(std::make_shared<QuadtreeNode>(x + halfWidth, y + halfHeight, halfWidth, halfHeight));
    divided = true;
}

TopoDS_Shape QuadtreeNode::fuse() {
    if (faces.empty()) {
        // If no faces in this node, return an empty shape
        return TopoDS_Shape();
    }

    TopoDS_Shape result_shape;

    if (faces.size() == 1) {
        // If there is only one face, use it directly without fusing
        result_shape = faces.front().Shape();
    }
    else if (faces.size() > 1) {
        auto itr = faces.begin();
        BRepAlgoAPI_Fuse* fused_face = new BRepAlgoAPI_Fuse(itr->Shape(), (++itr)->Shape());

        for (++itr; itr != faces.end(); ++itr) {
            fused_face = new BRepAlgoAPI_Fuse(fused_face->Shape(), itr->Shape());
        }

        result_shape = fused_face->Shape();
    }

    // Fuse results from child nodes
    if (divided) {
        for (auto& child : children) {
            TopoDS_Shape child_fuse_result = child->fuse();
            if (!child_fuse_result.IsNull()) {
                if (result_shape.IsNull()) {
                    result_shape = child_fuse_result;
                }
                else {
                    BRepAlgoAPI_Fuse fused_face(result_shape, child_fuse_result);
                    result_shape = fused_face.Shape();
                }
            }
        }
    }

    return result_shape; // Return the final fused shape
}

bool QuadtreeNode::intersects(double qx, double qy, double qwidth, double qheight) {
    return !(x > qx + qwidth || x + width < qx || y > qy + qheight || y + height < qy);
}

bool QuadtreeNode::contains_in_query(BRepBuilderAPI_MakeFace& face, double qx, double qy, double qwidth, double qheight) {
    Bnd_Box bbox;
    BRepBndLib::Add(face.Shape(), bbox);
    double xmin, ymin, zmin, xmax, ymax, zmax;
    bbox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    return (xmin >= qx && xmax <= qx + qwidth && ymin >= qy && ymax <= qy + qheight);
}
