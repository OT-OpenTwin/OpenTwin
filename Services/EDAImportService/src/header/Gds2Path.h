// @otlicense

#ifndef GDS2PATH_H_
#define GDS2PATH_H_

#include <vector>
#include <string>
#include "Polygon.h"

class Gds2Path {
private:
	unsigned int width; // width of the path
	unsigned int layer; 
	unsigned int pathType;
	std::vector<std::pair<int, int>> coordinates;

	std::pair<int, int> calcVector(std::pair<int, int>& p1, std::pair<int, int>& p2);
	std::pair<int, int> calcOffset(std::pair<int, int>& p1, std::pair<int, int>& p2);

	


public:
	MyPolygon pathToPolygon();

	unsigned int getWidth();
	unsigned int getLayer();
	unsigned int getPathType();
	std::vector<std::pair<int, int>> getCoordinates();
	void setWidth(int width);
	void setLayer(unsigned int layer);
	void setPathType(unsigned int pathType);
	void setCoordinates(std::vector<std::pair<int, int>> coordinates);

};

#endif

