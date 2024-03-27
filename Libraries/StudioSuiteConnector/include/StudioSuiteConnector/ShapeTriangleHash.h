#pragma once

#include "OTCore/CoreTypes.h"

#include <string>
#include <map>

class ShapeTriangleHash
{
public:
	ShapeTriangleHash(ot::UID infoEntityID, ot::UID infoEntityVersion);
	ShapeTriangleHash() = delete;
	~ShapeTriangleHash() {};

	std::string getHash(const std::string& shapeName);


private:
	std::map<std::string, std::string> shapeHashMap;

};