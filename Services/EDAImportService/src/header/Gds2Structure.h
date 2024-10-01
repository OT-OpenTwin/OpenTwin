#ifndef GDS2STRUCTURE_H_
#define GDS2STRUCTURE_H_

#include <string>
#include <vector>
#include "Polygon.h"
#include "StructRef.h"
#include "Gds2Path.h"




// This class represents a gds2 structure whith all its polygons and structure references
// WARNING: only the boundary type is tracked
class Gds2Structure {
private:
	std::string name;
	std::vector<MyPolygon> polygons;
	std::vector<StructRef> structureReferences;
	std::vector<Gds2Path> paths;
public:
	Gds2Structure(std::string name, std::vector<MyPolygon> polygons);
	Gds2Structure(std::string name);
	Gds2Structure();

	std::string getName();
	std::vector<MyPolygon> getPolygons();
	std::vector<StructRef> getStructRef();
	std::vector<Gds2Path> getPaths();

	void setStructRef(std::vector<StructRef> structRef);
	void setPolygons(std::vector<MyPolygon> polygons);
	void setPaths(std::vector<Gds2Path> paths);
	void addStuctRef(StructRef newStrRef);
};


#endif
