// @otlicense

//////////////////////////////////////////////////////////////////////////////////////////////
// Please note: This code needs to be refactored
//////////////////////////////////////////////////////////////////////////////////////////////

#include <iterator>
#include <map>
#include "SelfIntersectionCheck.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/intersections.h>
#include <CGAL/Polygon_mesh_processing/self_intersections.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Polygon_mesh_processing/locate.h>
#include <CGAL/Real_timer.h>
#include <CGAL/tags.h>
#include <CGAL/Triangle_3.h>
#include <CGAL/hierarchy_simplify_point_set.h>
#include <CGAL/Memory_sizer.h>
#include <CGAL/Timer.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Surface_mesh<K::Point_3>                      Mesh;
typedef Mesh::Vertex_index vertex_descriptor;
typedef Mesh::Face_index face_descriptor;
namespace PMP = CGAL::Polygon_mesh_processing;
Mesh m;

std::vector<K::Point_3> intersecting_point;

SelfIntersectionCheck::SelfIntersectionCheck()
{
	m.clear();//cleaning the mesh
}

SelfIntersectionCheck::~SelfIntersectionCheck()
{
	
}

void SelfIntersectionCheck::clear(void)
{
	m.clear();

	TrimmedPoints.clear();
	mapNodeVertex.clear();
	mapTrianglesFaces.clear();
	mapTrianglesTags.clear();
}

//Store all values from gmsh Entities and map them to cgal vertices
void SelfIntersectionCheck::setNodes(uint32_t nTag, double CoordsX,double CoordsY, double CoordsZ )
{
	vertex_descriptor u = m.add_vertex(K::Point_3( CoordsX, CoordsY, CoordsZ ));
	mapNodeVertex[nTag]= u;
}

//Store Gmsh Triangles and Gmsh Faces and map them with that of cgal 
void SelfIntersectionCheck::setTriangles(uint32_t faceIndex, uint32_t Tid, uint32_t node1, uint32_t node2, uint32_t node3)
{
	vertex_descriptor u = CGAL::SM_Vertex_index((uint32_t) mapNodeVertex[node1]);
	vertex_descriptor v = CGAL::SM_Vertex_index((uint32_t) mapNodeVertex[node2]);
	vertex_descriptor w = CGAL::SM_Vertex_index((uint32_t) mapNodeVertex[node3]);

	face_descriptor f = m.add_face(u, v, w);
	
	mapTrianglesFaces[f] = Tid;
	mapTrianglesTags[Tid] = faceIndex;

}

//Checking if there are any Intersections
bool SelfIntersectionCheck::IsSelfIntersecting()
{
	bool intersecting = PMP::does_self_intersect(m, PMP::parameters::vertex_point_map(get(CGAL::vertex_point, m)));
	//std::cout
	//	<< (intersecting ? "There are self-intersections." : "There is no self-intersection.")
	//	<< std::endl;
	return intersecting;
}

std::vector < std::tuple<double, double, double, uint32_t, uint32_t> > SelfIntersectionCheck::TrisSelfIntersecting()
{
	CGAL::Real_timer time;
	time.start();
	//std::cout << " Triangles Self Intersecting Module starts now." << std::endl;
	std::vector<std::pair<face_descriptor, face_descriptor>> intersected_tris;

	PMP::self_intersections<CGAL::Parallel_if_available_tag>(faces(m), m, std::back_inserter(intersected_tris));
	//std::cout << intersected_tris.size() << " pairs of triangles intersect." << std::endl;

	std::vector<std::pair<uint32_t, uint32_t>> intersecting_gmsh_faces;
	std::vector <vertex_descriptor> vIndices; 
	std::vector <vertex_descriptor> vIndices1;
	std::vector< std::pair<K::Triangle_3, K::Triangle_3>> triangles;
	std::vector < CGAL::Object >intersect_result;
	K::Point_3 point;
	K::Segment_3 line;
	K::Triangle_3 triangle;
	std::vector<K::Point_3> polypoint;
	
	std::vector < std::tuple<double, double, double, uint32_t,uint32_t> >cross_point;

	for (int i = 0; i < (intersected_tris.size()); i++)
	{
		if (intersected_tris[i].first > intersected_tris[i].second)
		{
			std::swap(intersected_tris[i].first, intersected_tris[i].second);
		}
		//std::cout << mapTrianglesTags[mapTrianglesFaces[intersected_tris[i].first]] << "intersect." << mapTrianglesTags[mapTrianglesFaces[intersected_tris[i].second]] << "\n";
		intersecting_gmsh_faces.push_back(std::make_pair(mapTrianglesTags[mapTrianglesFaces[intersected_tris[i].first]], mapTrianglesTags[mapTrianglesFaces[intersected_tris[i].second]]));
		for (vertex_descriptor vd : vertices_around_face(m.halfedge(intersected_tris[i].first), m)) {
			vIndices.push_back(vd);
		}

		for (vertex_descriptor vd : vertices_around_face(m.halfedge(intersected_tris[i].second), m)) {
			vIndices1.push_back(vd);
		}
		triangles.push_back(std::make_pair(K::Triangle_3(m.point(vIndices[0]), m.point(vIndices[1]), m.point(vIndices[2])), K::Triangle_3(m.point(vIndices1[0]), m.point(vIndices1[1]), m.point(vIndices1[2]))));

		intersect_result.push_back(CGAL::intersection(triangles[i].first, triangles[i].second));

		if (CGAL::assign(point, intersect_result[i])) {
			//std::cout << "point." << std::endl;
			intersecting_point.push_back(point);
			cross_point.push_back(std::make_tuple(intersecting_point.back().x(), intersecting_point.back().y(), intersecting_point.back().z(),intersecting_gmsh_faces[i].first, intersecting_gmsh_faces[i].second));
		}           

		else if (CGAL::assign(line, intersect_result[i])) {
			//std::cout << "line." << std::endl;

			intersecting_point.push_back(CGAL::midpoint(line.source(), line.target()));
			
			cross_point.push_back(std::make_tuple(intersecting_point.back().x(), intersecting_point.back().y(), intersecting_point.back().z(),intersecting_gmsh_faces[i].first, intersecting_gmsh_faces[i].second));
		}

		else if (CGAL::assign(triangle, intersect_result[i])) {
			//std::cout << "triangle." << std::endl;
			intersecting_point.push_back(CGAL::centroid(triangle[0], triangle[1], triangle[2]));
			cross_point.push_back(std::make_tuple(intersecting_point.back().x(), intersecting_point.back().y(), intersecting_point.back().z(),intersecting_gmsh_faces[i].first, intersecting_gmsh_faces[i].second));

		}
		else
			if (CGAL::assign(polypoint, intersect_result[i])) {
				//std::cout << "a polygon." << std::endl;
				
			}
			else {
				//std::cout << "unknown!" << std::endl;

				//std::cout << "(" << m.point(vIndices[0]) << "," << m.point(vIndices[1]) << "," << m.point(vIndices[2]) << "),(" << m.point(vIndices1[0]) << "," << m.point(vIndices1[1]) << "," << m.point(vIndices1[2]) << ")";

			}
		vIndices.clear(); 
		vIndices1.clear();
	}  
	/*std::cout << "PLEASE WAIT TILL IT LOADS";
	for (int i = 0; i < (intersecting_point.size()); i++){
		cross_point.push_back(std::make_tuple(intersecting_point[i][0], intersecting_point[i][1], intersecting_point[i][2], mapTrianglesTags[mapTrianglesFaces[PMP::locate(intersecting_point[i], m).first]]));

	}*/
	time.stop();
	//std::cout << " Triangles Self Intersecting Module ends now. It took " << time.time() << "seconds";
		return cross_point;
}

std::vector < std::tuple<double, double, double, uint32_t, uint32_t>> SelfIntersectionCheck::pointTriming(std::vector < std::tuple<double, double, double, uint32_t, uint32_t> > IntersectingInfo)
{
	//std::cout << "Inside Point Trim" << std::endl;
	// Hierarchy function works for Point functions only,  cant use tuple input in it. 

	intersecting_point.erase(CGAL::hierarchy_simplify_point_set(intersecting_point,
		CGAL::parameters::size(1000)// Max cluster size
		.maximum_variation(0.01)), // Max surface variation
		intersecting_point.end());
	std::size_t memory = CGAL::Memory_sizer().virtual_size();
	//std::cout << intersecting_point.size() << " point(s) kept, computed in "
	//<< (memory >> 20) << " Mib allocated." << std::endl;
	for (int i = 0; i < (intersecting_point.size()); i++) {
		for (int j = 0; j < (IntersectingInfo.size()); j++) {
			if ((intersecting_point[i][0] == std::get<0>(IntersectingInfo[j])) && (intersecting_point[i][1] == std::get<1>(IntersectingInfo[j])) && (intersecting_point[i][2] == std::get<2>(IntersectingInfo[j]))) {

				TrimmedPoints.push_back(std::make_tuple(intersecting_point[i][0], intersecting_point[i][1], intersecting_point[i][2], std::get<3>(IntersectingInfo[j]), std::get<4>(IntersectingInfo[j])));

			}
		
		}
	}

	return TrimmedPoints;
}




