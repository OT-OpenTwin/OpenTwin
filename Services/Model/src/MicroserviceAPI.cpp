#include "stdafx.h"

#include "MicroserviceAPI.h"
#include "MicroserviceNotifier.h"
#include "ProjectTypeManager.h"
#include "Model.h"
#include "Types.h"
#include "curl/curl.h"

#include "EntityGeometry.h"

#include "DataBase.h"
#include <array>
#include <fstream>
#include <cstdio>
#include <chrono>
#include <thread>

#include "OTCore/Logger.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/PropertyGridCfg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/IpConverter.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ServiceLogNotifier.h"

#include "base64.h"
#include "zlib.h"

ot::JsonDocument MicroserviceAPI::BuildJsonDocFromAction(const std::string &action)
{
	ot::JsonDocument newDoc;
	newDoc.SetObject();

	newDoc.AddMember(OT_ACTION_MEMBER, rapidjson::Value(action.c_str(), newDoc.GetAllocator()), newDoc.GetAllocator());

	return newDoc;
}

ot::JsonDocument MicroserviceAPI::BuildJsonDocFromString(std::string json)
{
	ot::JsonDocument doc;
	doc.Parse(json.c_str());
	assert(doc.IsObject());

	return doc;
}

void MicroserviceAPI::addTreeIconsToJsonDoc(ot::JsonDocument &doc, const TreeIcon &treeIcons)
{
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconSize, rapidjson::Value(treeIcons.size), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconItemVisible, rapidjson::Value(treeIcons.visibleIcon.c_str(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_IconItemHidden, rapidjson::Value(treeIcons.hiddenIcon.c_str(), doc.GetAllocator()), doc.GetAllocator());
}

void MicroserviceAPI::AddDoubleArrayVectorToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::vector<std::array<double, 3>> &vector)
{
	ot::JsonArray vectorDouble;
	vectorDouble.Reserve(3 * (int) vector.size(), doc.GetAllocator());
	for (unsigned int i = 0; i < vector.size(); i++)
	{
		vectorDouble.PushBack(rapidjson::Value().SetDouble(vector[i][0]), doc.GetAllocator());
		vectorDouble.PushBack(rapidjson::Value().SetDouble(vector[i][1]), doc.GetAllocator());
		vectorDouble.PushBack(rapidjson::Value().SetDouble(vector[i][2]), doc.GetAllocator());
	}

	doc.AddMember(ot::JsonString(name, doc.GetAllocator()), vectorDouble, doc.GetAllocator());
}

void MicroserviceAPI::AddDoubleArrayPointerToJsonDoc(ot::JsonDocument &doc, const std::string &name, const double *doubleArray, int size)
{
	ot::JsonArray doubleArr;
	doubleArr.Reserve(size, doc.GetAllocator());
	for (int i = 0; i < size; i++)
	{
		doubleArr.PushBack(doubleArray[i], doc.GetAllocator());
	}

	doc.AddMember(ot::JsonString(name, doc.GetAllocator()), doubleArr, doc.GetAllocator());
}

std::vector<ot::UID> MicroserviceAPI::getVectorFromDocument(ot::JsonDocument &doc, const std::string &itemName)
{
	std::vector<ot::UID> result;

	rapidjson::Value vector = doc[itemName.c_str()].GetArray();

	for (unsigned int i = 0; i < vector.Size(); i++)
	{
		result.push_back(vector[i].GetUint64());
	}

	return result;
}

void MicroserviceAPI::AddNodeToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::vector<Geometry::Node> &nodes)
{
	rapidjson::Value nodeList(rapidjson::kArrayType);
	nodeList.Reserve(8 * (int) nodes.size(), doc.GetAllocator());
	for (auto node : nodes) {
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getCoord(0)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getCoord(1)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getCoord(2)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getNormal(0)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getNormal(1)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getNormal(2)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getUVpar(0)), doc.GetAllocator());
		nodeList.PushBack(rapidjson::Value().SetDouble(node.getUVpar(1)), doc.GetAllocator());
	}
	doc.AddMember(rapidjson::Value(name.c_str(), doc.GetAllocator()), nodeList, doc.GetAllocator());
}

void MicroserviceAPI::AddEdgeToJsonDoc(ot::JsonDocument &doc, const std::string &name,const std::list<Geometry::Edge> &edges)
{
	int elements = 0;

	for (auto edge : edges)
	{
		elements += 2 + 3 * edge.getNpoints();
	}
	rapidjson::Value edgeList(rapidjson::kArrayType);
	edgeList.Reserve(elements+2, doc.GetAllocator());
	edgeList.PushBack(rapidjson::Value().SetInt64(edges.size()), doc.GetAllocator());  // Store the number of edges
	for (auto edge : edges)
	{
		edgeList.PushBack(rapidjson::Value().SetInt64(edge.getFaceId()), doc.GetAllocator());   // For each edge store the corresponding faceId first
		edgeList.PushBack(rapidjson::Value().SetInt64(edge.getNpoints()), doc.GetAllocator());   // For each edge store the number of points 
		for (int i = 0; i < edge.getNpoints(); i++) {
			edgeList.PushBack(rapidjson::Value().SetDouble(edge.getPoint(i).getX()), doc.GetAllocator());   // Then store the points one by one
			edgeList.PushBack(rapidjson::Value().SetDouble(edge.getPoint(i).getY()), doc.GetAllocator());
			edgeList.PushBack(rapidjson::Value().SetDouble(edge.getPoint(i).getZ()), doc.GetAllocator());
		}
	}

	doc.AddMember(rapidjson::Value(name.c_str(), doc.GetAllocator()), edgeList, doc.GetAllocator());
}

void MicroserviceAPI::AddTriangleToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::list<Geometry::Triangle> &triangles)
{
	rapidjson::Value triangleArr(rapidjson::kArrayType);
	triangleArr.Reserve(4 * (int) triangles.size(), doc.GetAllocator());
	for (auto triangle : triangles) {
		triangleArr.PushBack(rapidjson::Value().SetInt64(triangle.getNode(0)), doc.GetAllocator());
		triangleArr.PushBack(rapidjson::Value().SetInt64(triangle.getNode(1)), doc.GetAllocator());
		triangleArr.PushBack(rapidjson::Value().SetInt64(triangle.getNode(2)), doc.GetAllocator());
		triangleArr.PushBack(rapidjson::Value().SetInt64(triangle.getFaceId()), doc.GetAllocator());
		
	}
	doc.AddMember(rapidjson::Value(name.c_str(), doc.GetAllocator()), triangleArr, doc.GetAllocator());
}

