#pragma once

#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"

#include "Types.h"
#include "Geometry.h"

#include <list>

extern std::string globalUIserviceURL;

namespace MicroserviceAPI
{
	void AddDoubleArrayVectorToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::vector<std::array<double, 3>> &vector);
	void AddDoubleArrayPointerToJsonDoc(ot::JsonDocument &doc, const std::string &name, const double *doubleArray, int size);
	void addTreeIconsToJsonDoc(ot::JsonDocument &doc, const TreeIcon &treeIcons);

	void AddEdgeToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::list<Geometry::Edge> &edges);
	void AddTriangleToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::list<Geometry::Triangle> &triangles);
	void AddNodeToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::vector<Geometry::Node> &nodes);

	std::vector<ot::UID> getVectorFromDocument(ot::JsonDocument &doc, const std::string &itemName);
	
	ot::JsonDocument BuildJsonDocFromAction(const std::string &action);
	ot::JsonDocument BuildJsonDocFromString(std::string json);

}

