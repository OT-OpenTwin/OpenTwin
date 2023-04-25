#pragma once

#include <list>
#include <map>

#include "Geometry.h"
#include "BoundingBox.h"

class EntityBase;
class EntityGeometry;
class TopoDS_Face;
class TopoDS_Shape;
class EntityPropertiesEntityList;

namespace GeometryOperations {

	typedef std::list<EntityBase *> EntityList;

	bool facetEntity(TopoDS_Shape &shape, double deflection,
				     std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::string &errors);
	bool checkPointInTriangle(double x, double y, double z, double nodeCoord1[], double nodeCoord2[], double nodeCoord3[], double tolerance);
	bool SameSide(double p1[], double p2[], double a[], double b[]);
	void calculateCrossProduct(double vector1[3], double vector2[3], double normal[3]);
	double calculateDotProduct(double vector1[3], double vector2[3]);
	double calculateDistancePointToPlane(double p[], double v1[], double v2[], double v3[]);
	double getMaximumFaceCurvature(TopoDS_Face &aFace);
	std::vector<std::pair<std::pair<double, double>, int>> getCurvatureRadiusHistogram(std::list<double> &faceCurvaturRadius, int nbins);
	BoundingBox getBoundingBox(std::list<EntityGeometry *> &geometryEntities);
}
