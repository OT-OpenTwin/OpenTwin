
#include "Geometry.h"
#include "DataBase.h"

#include <bsoncxx/builder/basic/array.hpp>

bsoncxx::document::value Geometry::getBSON(std::vector<Geometry::Node> &nodes)
{
	auto coordX = bsoncxx::builder::basic::array();
	auto coordY = bsoncxx::builder::basic::array();
	auto coordZ = bsoncxx::builder::basic::array();

	auto normalX = bsoncxx::builder::basic::array();
	auto normalY = bsoncxx::builder::basic::array();
	auto normalZ = bsoncxx::builder::basic::array();

	auto paramU = bsoncxx::builder::basic::array();
	auto paramV = bsoncxx::builder::basic::array();

	for (Geometry::Node &node : nodes)
	{
		coordX.append(node.getCoord(0));
		coordY.append(node.getCoord(1));
		coordZ.append(node.getCoord(2));

		normalX.append(node.getNormal(0));
		normalY.append(node.getNormal(1));
		normalZ.append(node.getNormal(2));

		paramU.append(node.getUVpar(0));
		paramV.append(node.getUVpar(1));
	}

	auto doc = bsoncxx::builder::basic::document{};

	doc.append(
		bsoncxx::builder::basic::kvp("CoordX", coordX),
		bsoncxx::builder::basic::kvp("CoordY", coordY),
		bsoncxx::builder::basic::kvp("CoordZ", coordZ),
		bsoncxx::builder::basic::kvp("NormalX", normalX),
		bsoncxx::builder::basic::kvp("NormalY", normalY),
		bsoncxx::builder::basic::kvp("NormalZ", normalZ),
		bsoncxx::builder::basic::kvp("ParamU", paramU),
		bsoncxx::builder::basic::kvp("ParamV", paramV)
	);

	return doc.extract();
}

bsoncxx::document::value Geometry::getBSON(std::list<Geometry::Triangle> &triangles)
{
	auto node1  = bsoncxx::builder::basic::array();
	auto node2  = bsoncxx::builder::basic::array();
	auto node3  = bsoncxx::builder::basic::array();
	auto faceID = bsoncxx::builder::basic::array();

	for (Geometry::Triangle &triangle : triangles)
	{
		node1.append((long long) triangle.getNode(0));
		node2.append((long long) triangle.getNode(1));
		node3.append((long long) triangle.getNode(2));
		faceID.append((long long) triangle.getFaceId());
	}

	auto doc = bsoncxx::builder::basic::document{};

	doc.append(
		bsoncxx::builder::basic::kvp("Node1", node1),
		bsoncxx::builder::basic::kvp("Node2", node2),
		bsoncxx::builder::basic::kvp("Node3", node3),
		bsoncxx::builder::basic::kvp("faceID", faceID)
	);

	return doc.extract();
}

bsoncxx::document::value Geometry::getBSON(std::list<Geometry::Edge> &edges)
{
	// In a first step, store all points in an array
	auto pointX = bsoncxx::builder::basic::array();
	auto pointY = bsoncxx::builder::basic::array();
	auto pointZ = bsoncxx::builder::basic::array();

	auto numberPoints = bsoncxx::builder::basic::array();
	auto pointIndex   = bsoncxx::builder::basic::array();
	auto faceID       = bsoncxx::builder::basic::array();

	long long index = 0;

	for (Geometry::Edge &edge : edges)
	{
		numberPoints.append(edge.getNpoints());
		pointIndex.append(index);
		faceID.append((long long) edge.getFaceId());

		for (int i = 0; i < edge.getNpoints(); i++)
		{
			pointX.append(edge.getPoint(i).getX());
			pointY.append(edge.getPoint(i).getY());
			pointZ.append(edge.getPoint(i).getZ());

			index++;
		}
	}

	auto doc = bsoncxx::builder::basic::document{};

	doc.append(
		bsoncxx::builder::basic::kvp("PointX", pointX),
		bsoncxx::builder::basic::kvp("PointY", pointY),
		bsoncxx::builder::basic::kvp("PointZ", pointZ),
		bsoncxx::builder::basic::kvp("NumberPoints", numberPoints),
		bsoncxx::builder::basic::kvp("PointIndex", pointIndex),
		bsoncxx::builder::basic::kvp("faceID", faceID)
	);

	return doc.extract();
}

bsoncxx::document::value Geometry::getBSON(std::map<ot::UID, std::string> &faceNameMap)
{
	// In a first step, store all points in an array
	auto faceID = bsoncxx::builder::basic::array();
	auto faceName = bsoncxx::builder::basic::array();

	long long index = 0;

	for (auto face : faceNameMap)
	{
		faceID.append((long long) face.first);
		faceName.append(face.second);
	}

	auto doc = bsoncxx::builder::basic::document{};

	doc.append(
		bsoncxx::builder::basic::kvp("FaceID", faceID),
		bsoncxx::builder::basic::kvp("FaceName", faceName)
	);

	return doc.extract();
}

void Geometry::readBSON(bsoncxx::document::view &nodesObj, std::vector<Geometry::Node> &nodes)
{
	auto coordX = nodesObj["CoordX"].get_array().value;
	auto coordY = nodesObj["CoordY"].get_array().value;
	auto coordZ = nodesObj["CoordZ"].get_array().value;

	auto normalX = nodesObj["NormalX"].get_array().value;
	auto normalY = nodesObj["NormalY"].get_array().value;
	auto normalZ = nodesObj["NormalZ"].get_array().value;

	auto paramU = nodesObj["ParamU"].get_array().value;
	auto paramV = nodesObj["ParamV"].get_array().value;


	size_t numberNodes = std::distance(coordX.begin(), coordX.end());
	assert(numberNodes == std::distance(coordY.begin(), coordZ.end()));
	assert(numberNodes == std::distance(coordY.begin(), coordZ.end()));

	assert(numberNodes == std::distance(normalX.begin(), normalX.end()));
	assert(numberNodes == std::distance(normalY.begin(), normalY.end()));
	assert(numberNodes == std::distance(normalZ.begin(), normalZ.end()));

	assert(numberNodes == std::distance(paramU.begin(), paramU.end()));
	assert(numberNodes == std::distance(paramV.begin(), paramV.end()));

	nodes.resize(numberNodes);

	auto cX = coordX.begin();
	auto cY = coordY.begin();
	auto cZ = coordZ.begin();
	auto nX = normalX.begin();
	auto nY = normalY.begin();
	auto nZ = normalZ.begin();
	auto pU = paramU.begin();
	auto pV = paramV.begin();

	for (unsigned long index = 0; index < numberNodes; index++)
	{
		nodes[index].setCoords(cX->get_double(), cY->get_double(), cZ->get_double());
		nodes[index].setNormals(nX->get_double(), nY->get_double(), nZ->get_double());
		nodes[index].setUVpar(pU->get_double(), pV->get_double());

		cX++;
		cY++;
		cZ++;
		nX++;
		nY++;
		nZ++;
		pU++;
		pV++;
	}
}

void Geometry::readBSON(bsoncxx::document::view &trianglesObj, std::list<Geometry::Triangle> &triangles)
{
	auto node1  = trianglesObj["Node1"].get_array().value;
	auto node2  = trianglesObj["Node2"].get_array().value;
	auto node3  = trianglesObj["Node3"].get_array().value;
	auto faceID = trianglesObj["faceID"].get_array().value;

	size_t numberTriangles = std::distance(node1.begin(), node1.end());
	assert(numberTriangles == std::distance(node2.begin(), node2.end()));
	assert(numberTriangles == std::distance(node3.begin(), node3.end()));
	assert(numberTriangles == std::distance(faceID.begin(), faceID.end()));

	auto n1  = node1.begin();
	auto n2  = node2.begin();
	auto n3  = node3.begin();
	auto fID = faceID.begin();

	for (unsigned long index = 0; index < numberTriangles; index++)
	{
		Geometry::Triangle triangle;

		triangle.setNodes(DataBase::getIntFromArrayViewIterator(n1), DataBase::getIntFromArrayViewIterator(n2), DataBase::getIntFromArrayViewIterator(n3));
		triangle.setFaceId(DataBase::getIntFromArrayViewIterator(fID));

		triangles.push_back(triangle);

		n1++;
		n2++;
		n3++;
		fID++;
	}
}

void Geometry::readBSON(bsoncxx::document::view &edgesObj, std::list<Geometry::Edge> &edges)
{
	auto pointX = edgesObj["PointX"].get_array().value;
	auto pointY = edgesObj["PointY"].get_array().value;
	auto pointZ = edgesObj["PointZ"].get_array().value;

	auto numberPoints = edgesObj["NumberPoints"].get_array().value;
	auto pointIndex   = edgesObj["PointIndex"].get_array().value;
	auto faceID       = edgesObj["faceID"].get_array().value;

	size_t numberEdges = std::distance(numberPoints.begin(), numberPoints.end());
	assert(numberEdges == std::distance(pointIndex.begin(), pointIndex.end()));
	assert(numberEdges == std::distance(faceID.begin(), faceID.end()));

	auto pX = pointX.begin();
	auto pY = pointY.begin();
	auto pZ = pointZ.begin();

	auto nP  = numberPoints.begin();
	auto pI  = pointIndex.begin();
	auto fID = faceID.begin();

	for (unsigned long index = 0; index < numberEdges; index++)
	{
		Geometry::Edge edge;

		edge.setNpoints((int)DataBase::getIntFromArrayViewIterator(nP));
		edge.setFaceId(DataBase::getIntFromArrayViewIterator(fID));

		for (unsigned long point = 0; point < (unsigned long) edge.getNpoints(); point++)
		{
			edge.setPoint((int) point, pX->get_double(), pY->get_double(), pZ->get_double());

			pX++;
			pY++;
			pZ++;
		}

		nP++;
		pI++;
		fID++;

		edges.push_back(edge);
	}
}

void Geometry::readBSON(bsoncxx::document::view& faceNamesObj, std::map<ot::UID, std::string>& faceNameMap)
{
	auto faceID = faceNamesObj["FaceID"].get_array().value;
	auto faceName = faceNamesObj["FaceName"].get_array().value;

	size_t numberIds = std::distance(faceID.begin(), faceID.end());
	assert(numberIds == std::distance(faceName.begin(), faceName.end()));

	auto pId = faceID.begin();
	auto pName = faceName.begin();

	for (unsigned long index = 0; index < numberIds; index++)
	{
		ot::UID id = DataBase::getIntFromArrayViewIterator(pId);
		std::string name = std::string(pName->get_utf8().value.data());

		pId++;
		pName++;

		faceNameMap[id] = name;
	}
}
