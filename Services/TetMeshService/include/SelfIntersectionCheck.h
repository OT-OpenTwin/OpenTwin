// @otlicense

#pragma once
#include<vector>
class SelfIntersectionCheck
{
public:
	SelfIntersectionCheck();
	virtual ~SelfIntersectionCheck();
	void clear(void);
	void setNodes(uint32_t nTag, double CoordsX, double CoordsY, double CoordsZ);
	void setTriangles(uint32_t faceIndex, uint32_t Tid, uint32_t node1, uint32_t node2, uint32_t node3);
	bool IsSelfIntersecting();
	std::vector < std::tuple<double, double, double, uint32_t, uint32_t> > TrisSelfIntersecting();
	std::vector<std::tuple<double, double, double, uint32_t, uint32_t>> pointTriming(std::vector<std::tuple<double, double, double, uint32_t, uint32_t>> IntersectingInfo);
	
private:
	std::vector < std::tuple<double, double, double, uint32_t, uint32_t>> TrimmedPoints;
	std::map<uint32_t, uint32_t> mapNodeVertex;//mapping gmsh node-Tag to cgal Vertex ID
	std::map<uint32_t, uint32_t> mapTrianglesFaces; //mapping gmsh Triangles to cgal Face
	std::map<uint32_t, uint32_t> mapTrianglesTags;//mapping gmsh Triangles to  gmsh Faces
};
