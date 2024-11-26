#pragma once

#include <string>
#include <assert.h>
#include <vector>
#include <list>
#include <map>

#include <bsoncxx/builder/basic/document.hpp>

#include "OldTreeIcon.h"

namespace Geometry {

	// This namespace contains a collection of geometry manipulations on Entities

	class __declspec(dllexport) Node {
	public:
		Node() { coord[0] = coord[1] = coord[2] = 0.0; normal[0] = normal[1] = normal[2] = 0.0; uvpar[0] = uvpar[1] = 0.0; };
		virtual ~Node() {};

		void setCoords(double x, double y, double z) { coord[0] = x; coord[1] = y; coord[2] = z; };
		void setNormals(double x, double y, double z) { normal[0] = x; normal[1] = y; normal[2] = z; };
		void setUVpar(double u, double v) { uvpar[0] = u; uvpar[1] = v; };

		double getCoord(int index) { assert(index >= 0 && index < 3);  return coord[index]; };
		double getNormal(int index) { assert(index >= 0 && index < 3);  return normal[index]; };
		double getUVpar(int index) { assert(index >= 0 && index < 2);  return uvpar[index]; };

		void setX(double x) { coord[0] = x; };
		void setY(double y) { coord[1] = y; };
		void setZ(double z) { coord[2] = z; };

		void setNx(double x) { normal[0] = x; };
		void setNy(double y) { normal[1] = y; };
		void setNz(double z) { normal[2] = z; };

		void setU(double u) { uvpar[0] = u; };
		void setV(double v) { uvpar[1] = v; };

	private:
		double coord[3];
		double normal[3];
		double uvpar[2];
	};

	class __declspec(dllexport) Triangle {
	public:
		Triangle() { node[0] = node[1] = node[2] = 0; faceId = 0; };
		Triangle(ot::UID node1, ot::UID node2, ot::UID node3, ot::UID id) { node[0] = node1;  node[1] = node2;  node[2] = node3; faceId = id; };
		virtual ~Triangle() {};

		void setNodes(ot::UID node1, ot::UID node2, ot::UID node3) { node[0] = node1; node[1] = node2; node[2] = node3; }
		ot::UID getNode(int index) { assert(index >= 0 && index < 3);  return node[index]; };

		void setFaceId(ot::UID id) { faceId = id; };
		ot::UID getFaceId(void) { return faceId; };

	private:
		ot::UID node[3];
		ot::UID faceId;
	};

	class __declspec(dllexport) Point {
	public:
		Point() { coord[0] = coord[1] = coord[2] = 0.0; };
		Point(const Point &other) { if (&other != this) { coord[0] = other.coord[0];  coord[1] = other.coord[1]; coord[2] = other.coord[2]; } };
		Point(double x, double y, double z) { coord[0] = x; coord[1] = y; coord[2] = z; };
		virtual ~Point() {};

		void setCoord(double x, double y, double z) { coord[0] = x; coord[1] = y; coord[2] = z; };
		double getX(void) { return coord[0]; };
		double getY(void) { return coord[1]; };
		double getZ(void) { return coord[2]; };

	private:
		double coord[3] = {0.,0.,0.};
	};

	class __declspec(dllexport) Edge {
	public:
		Edge() { npoints = 0; points = nullptr; faceId = 0; };
		virtual ~Edge() { if (points != nullptr) delete[] points; };
		Edge(const Edge &other) { if (&other != this) { npoints = 0; faceId = other.faceId;  if (other.npoints > 0) { npoints = other.npoints; points = new Point[npoints]; for (int i = 0; i < npoints; i++) points[i] = other.points[i]; } } }

		int getNpoints(void) { return npoints; };
		Point &getPoint(int index) { return points[index]; };

		void setNpoints(int n) { if (points != nullptr) delete[] points; points = new Point[n]; npoints = n; };
		void setPoint(int index, double x, double y, double z) { points[index].setCoord(x, y, z); };

		void setFaceId(ot::UID id) { faceId = id; };
		ot::UID getFaceId(void) { return faceId; };

	private:
		int npoints = 0;
		Point *points = nullptr;
		ot::UID faceId = 0;
	};

	_declspec(dllexport) bsoncxx::document::value getBSON(std::vector<Geometry::Node> &nodes);
	_declspec(dllexport) bsoncxx::document::value getBSON(std::list<Geometry::Triangle> &triangles);
	_declspec(dllexport) bsoncxx::document::value getBSON(std::list<Geometry::Edge>& edges);
	_declspec(dllexport) bsoncxx::document::value getBSON(std::map<ot::UID, std::string>& faceNameMap);

	_declspec(dllexport) void readBSON(bsoncxx::document::view &nodesObj,     std::vector<Geometry::Node> &nodes);
	_declspec(dllexport) void readBSON(bsoncxx::document::view &trianglesObj, std::list<Geometry::Triangle> &triangles);
	_declspec(dllexport) void readBSON(bsoncxx::document::view& edgesObj, std::list<Geometry::Edge>& edges);
	_declspec(dllexport) void readBSON(bsoncxx::document::view& faceNamesObj, std::map<ot::UID, std::string>& faceNameMap);
}
