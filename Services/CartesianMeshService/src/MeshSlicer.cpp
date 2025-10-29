// @otlicense

#include "stdafx.h"
#include "MeshSlicer.h"
#include "EntityFacetData.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_slicer.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/AABB_halfedge_graph_segment_primitive.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <fstream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel   Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3>                   Mesh;
typedef std::vector<Kernel::Point_3>                          Polyline_type;
typedef std::list<Polyline_type>                              Polylines;
typedef CGAL::AABB_halfedge_graph_segment_primitive<Mesh>     HGSP;
typedef CGAL::AABB_traits<Kernel, HGSP>                       AABB_traits;
typedef CGAL::AABB_tree<AABB_traits>                          AABB_tree;
typedef Mesh::Vertex_index									  vertex_descriptor;
typedef Mesh::Face_index									  face_descriptor;

namespace PMP = CGAL::Polygon_mesh_processing;

MeshSlicer::MeshSlicer()
{

}

MeshSlicer::~MeshSlicer()
{

}

void MeshSlicer::doit(EntityFacetData *facets)
{
	Mesh mesh;
	std::vector<vertex_descriptor> vertices;
	vertices.reserve(facets->getNodeVector().size());

	for (auto node : facets->getNodeVector())
	{
		vertices.push_back(mesh.add_vertex(Kernel::Point_3(node.getCoord(0), node.getCoord(1), node.getCoord(2))));
	}

	for (auto triangle : facets->getTriangleList())
	{
		face_descriptor f = mesh.add_face(vertices[triangle.getNode(0)], vertices[triangle.getNode(1)], vertices[triangle.getNode(2)]);
		assert(f != Mesh::null_face());
	}

	// Slicer constructor from the mesh
	CGAL::Polygon_mesh_slicer<Mesh, Kernel> slicer(mesh);

	Polylines polylines;
	slicer(Kernel::Plane_3(0, 0, 1, -0.4), std::back_inserter(polylines));
	std::cout << "At z = 0.4, the slicer intersects " << polylines.size() << " polylines" << std::endl;
	polylines.clear();
	slicer(Kernel::Plane_3(0, 0, 1, 0.2), std::back_inserter(polylines));
	std::cout << "At z = -0.2, the slicer intersects " << polylines.size() << " polylines" << std::endl;
	polylines.clear();
	// Use the Slicer constructor from a pre-built AABB_tree
	AABB_tree tree(edges(mesh).first, edges(mesh).second, mesh);
	CGAL::Polygon_mesh_slicer<Mesh, Kernel> slicer_aabb(mesh, tree);
	slicer_aabb(Kernel::Plane_3(0, 0, 1, -0.4), std::back_inserter(polylines));
	std::cout << "At z = 0.4, the slicer intersects " << polylines.size() << " polylines" << std::endl;
	polylines.clear();

}
