// @otlicense
// File: GeometryOperations.h
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
#pragma warning(disable : 4251)

// OpenTwin header
#include "OTModelEntities/Geometry.h"
#include "OTModelEntities/BoundingBox.h"
#include "OTCADEntities/CADModelEntitiesAPIExport.h"

// std header
#include <list>
#include <map>

class EntityBase;
class EntityGeometry;
class EntityBrep;
class TopoDS_Face;
class TopoDS_Shape;
class EntityPropertiesEntityList;

namespace ot {
	class GeometryOperations {
		OT_DECL_STATICONLY(GeometryOperations)
	public:
		typedef std::list<EntityBase*> EntityList;

		static OT_CADENTITIES_API_EXPORT bool facetEntity(TopoDS_Shape& shape, EntityBrep* brep, double deflection, std::vector<Geometry::Node>& nodes, std::list<Geometry::Triangle>& triangles, std::list<Geometry::Edge>& edges, std::map<ot::UID, std::string>& faceNames, std::string& errors);
		static OT_CADENTITIES_API_EXPORT bool checkPointInTriangle(double x, double y, double z, double nodeCoord1[], double nodeCoord2[], double nodeCoord3[], double tolerance);
		static OT_CADENTITIES_API_EXPORT bool SameSide(double p1[], double p2[], double a[], double b[]);
		static OT_CADENTITIES_API_EXPORT void calculateCrossProduct(double vector1[3], double vector2[3], double normal[3]);
		static OT_CADENTITIES_API_EXPORT double calculateDotProduct(double vector1[3], double vector2[3]);
		static OT_CADENTITIES_API_EXPORT double calculateDistancePointToPlane(double p[], double v1[], double v2[], double v3[]);
		static OT_CADENTITIES_API_EXPORT double getMaximumFaceCurvature(TopoDS_Face& aFace);
		static OT_CADENTITIES_API_EXPORT std::vector<std::pair<std::pair<double, double>, int>> getCurvatureRadiusHistogram(std::list<double>& faceCurvaturRadius, int nbins);
		static OT_CADENTITIES_API_EXPORT BoundingBox getBoundingBox(std::list<EntityGeometry*>& geometryEntities);
	};
}
