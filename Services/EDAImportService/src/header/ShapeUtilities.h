// @otlicense
// File: ShapeUtilities.h
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

#ifndef SHAPEUTILITIES_H
#define SHAPEUTILITIES_H

#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <list>
#include <vector>
#include <gp_Pnt.hxx>

class ShapeUtilities {
public:
    static BRepBuilderAPI_MakePolygon makePolygon(std::list<gp_Pnt>& points);
    static std::list<BRepBuilderAPI_MakePolygon> makePolygons(std::list<std::list<gp_Pnt>>& points);
    static BRepBuilderAPI_MakeFace makeFace(BRepBuilderAPI_MakePolygon& polygon);
    static std::list<BRepBuilderAPI_MakeFace> makeFaces(std::list<BRepBuilderAPI_MakePolygon>& polygons, std::list<BRepBuilderAPI_MakeWire>& wires);
    static BRepBuilderAPI_MakeWire createOffset(std::list<gp_Pnt>& points, int width, int pathType, Standard_Real height);
};

#endif // SHAPEUTILITIES_H
