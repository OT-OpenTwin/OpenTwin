// @otlicense

#pragma once
#include "SystemDependencies/SystemDependentDefines.h"

class Point3D
{
public:
	Point3D(index_t x, index_t y, index_t z)
		: _coordinateX(x), _coordinateY(y), _coordinateZ(z) {};
	
	Point3D(const Point3D &other)
		: _coordinateX(other.GetCoordinateX()), _coordinateY(other.GetCoordinateY()), _coordinateZ(other.GetCoordinateZ()) {};

	const index_t GetCoordinateX() const { return _coordinateX; }
	const index_t GetCoordinateY() const { return _coordinateY; }
	const index_t GetCoordinateZ() const { return _coordinateZ; }

	std::string Print() { return "(" + std::to_string(_coordinateX) + "," + std::to_string(_coordinateY) + "," + std::to_string(_coordinateZ) +")"; }
private:
	index_t _coordinateX;
	index_t _coordinateY;
	index_t _coordinateZ;
};
