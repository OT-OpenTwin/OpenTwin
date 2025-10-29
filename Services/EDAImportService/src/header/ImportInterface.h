// @otlicense

#ifndef IMPORTINTERDFACE_H_
#define IMPORTINTERDFACE_H_

#include <vector>
#include <string>
#include "Polygon.h"



class ImportInterface{
public:
	virtual std::vector<MyPolygon> getPolygons(std::vector<std::byte> data);
	virtual std::vector<std::byte> readFileData(const std::string& name);
};

#endif

