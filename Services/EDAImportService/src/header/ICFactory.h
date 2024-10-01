#ifndef ICFACTORY_H_
#define ICFACTORY_H_


#include <vector>
#include <string>

#include "Polygon.h"
#include "Layer.h"
#include "IC.h"
#include "Gds2Import.h"
#include "Gds2Path.h"

class ICFactory{
private:
	static std::vector<Layer> getLayers(std::vector<MyPolygon>& polygons, std::vector<std::pair<int, std::vector<int>>>& stackUpInfo);
	static std::vector<Layer> layers(std::pair < std::vector<MyPolygon>, std::vector<Gds2Path>> &modelData, std::vector<std::pair<int, std::vector<int>>>& stackUpInfo);
public:
	static IC generateIC(std::string name, std::string filepath, std::string stackUpPath);
};

#endif

