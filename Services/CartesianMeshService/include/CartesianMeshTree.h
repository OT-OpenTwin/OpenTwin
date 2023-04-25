#pragma once

#include <vector>
#include <ctime>
#include <map>
#include <list>

#include "Geometry.h"

class EntityGeometry;


class CartesianMeshTree
{
public:
	CartesianMeshTree() {}
	virtual ~CartesianMeshTree() { }

	void renderTriangle(float nodes[3][3], float xmin, float xmax, float ymin, float ymax, float zmin, float zmax,
						std::vector<char> &shapeFill, size_t my, size_t mz,
						std::vector<double> &linesX, std::vector<double> &linesY, std::vector<double> &linesZ,
						unsigned int ixmin, unsigned int ixmax, unsigned int iymin, unsigned int iymax, unsigned int izmin, unsigned int izmax, float delta,
						std::vector<std::map<EntityGeometry*, std::list<Geometry::Triangle*>>> *triangleInCellInformation, Geometry::Triangle &triangle, EntityGeometry *geometryEntity);

private:
	bool triangleIntersects(float nodes[3][3], float xmin, float xmax, float ymin, float ymax, float zmin, float zmax,
							std::vector<double> &linesX, std::vector<double> &linesY, std::vector<double> &linesZ,
							unsigned int ixmin, unsigned int ixmax, unsigned int iymin, unsigned int iymax, unsigned int izmin, unsigned int izmax, float delta);
};

