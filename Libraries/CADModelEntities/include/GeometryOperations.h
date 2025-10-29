// @otlicense

#pragma once
#pragma warning(disable : 4251)

#include <list>
#include <map>

#include "Geometry.h"
#include "BoundingBox.h"

class EntityBase;
class EntityGeometry;
class EntityBrep;
class TopoDS_Face;
class TopoDS_Shape;
class EntityPropertiesEntityList;

namespace GeometryOperations {

	typedef std::list<EntityBase *> EntityList;

	__declspec(dllexport) bool facetEntity(TopoDS_Shape &shape, EntityBrep* brep, double deflection,
										   std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::map<ot::UID, std::string>& faceNames, std::string &errors);
	__declspec(dllexport) bool checkPointInTriangle(double x, double y, double z, double nodeCoord1[], double nodeCoord2[], double nodeCoord3[], double tolerance);
	__declspec(dllexport) bool SameSide(double p1[], double p2[], double a[], double b[]);
	__declspec(dllexport) void calculateCrossProduct(double vector1[3], double vector2[3], double normal[3]);
	__declspec(dllexport) double calculateDotProduct(double vector1[3], double vector2[3]);
	__declspec(dllexport) double calculateDistancePointToPlane(double p[], double v1[], double v2[], double v3[]);
	__declspec(dllexport) double getMaximumFaceCurvature(TopoDS_Face &aFace);
	__declspec(dllexport) std::vector<std::pair<std::pair<double, double>, int>> getCurvatureRadiusHistogram(std::list<double> &faceCurvaturRadius, int nbins);
	__declspec(dllexport) BoundingBox getBoundingBox(std::list<EntityGeometry *> &geometryEntities);
}
