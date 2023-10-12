
#include "EntityMeshTetFace.h"
#include "EntityAnnotation.h"
#include "DataBase.h"
#include "Types.h"

#include <bsoncxx/builder/basic/array.hpp>


EntityMeshTetFace::EntityMeshTetFace(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, factory, owner),
	surfaceId(0)
{

}

bool EntityMeshTetFace::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	if (boundingBox.getEmpty()) return false;

	xmin = boundingBox.getXmin();
	xmax = boundingBox.getXmax();
	ymin = boundingBox.getYmin();
	ymax = boundingBox.getYmax();
	zmin = boundingBox.getZmin();
	zmax = boundingBox.getZmax();

	return true;
}

void EntityMeshTetFace::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Now we store the particular information about the current object 
	storage.append(
		bsoncxx::builder::basic::kvp("SurfaceId", surfaceId),
		bsoncxx::builder::basic::kvp("BoundingBox", boundingBox.getBSON()),
		bsoncxx::builder::basic::kvp("Triangles", getBSON(triangles)),
		bsoncxx::builder::basic::kvp("FaceAnnotationIDs", getBSON(faceAnnotationIDs))
	);
}

void EntityMeshTetFace::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the Brep and Facet objects
	surfaceId = (int) DataBase::GetIntFromView(doc_view, "SurfaceId");
	boundingBox.setFromBSON(doc_view["BoundingBox"].get_document().value);

	bsoncxx::document::view trianglesObj   = doc_view["Triangles"].get_document().view();
	bsoncxx::document::view annotationsObj = doc_view["FaceAnnotationIDs"].get_document().view();

	readBSON(trianglesObj, triangles);
	readBSON(annotationsObj, faceAnnotationIDs);

	resetModified();
}

bsoncxx::document::value EntityMeshTetFace::getBSON(std::vector<EntityMeshTetTriangle> &triangles)
{
	// This is new code for storing triangles which is able to handle arbitrary orders of triangles
	auto nodes = bsoncxx::builder::basic::array();

	// Write the number of triangles
	nodes.append((long long) triangles.size());

	for (EntityMeshTetTriangle &triangle : triangles)
	{
		// For each triangle, write the number of nodes
		nodes.append((int) triangle.getNumberOfNodes());

		for (int index = 0; index < triangle.getNumberOfNodes(); index++)
		{
			// Write the nodes of the triangle
			nodes.append((long long)triangle.getNode(index));
		}
	}

	auto doc = bsoncxx::builder::basic::document{};

	doc.append(
		bsoncxx::builder::basic::kvp("Nodes", nodes)
	);

	return doc.extract();
}

bsoncxx::document::value EntityMeshTetFace::getBSON(std::vector<std::pair<bool, ot::UID>> &faceAnnotationIDs)
{
	auto orientation  = bsoncxx::builder::basic::array();
	auto annotationId = bsoncxx::builder::basic::array();

	for (auto &annotation : faceAnnotationIDs)
	{
		orientation.append(annotation.first);
		annotationId.append((long long) annotation.second);
	}

	auto doc = bsoncxx::builder::basic::document{};

	doc.append(
		bsoncxx::builder::basic::kvp("Orientation", orientation),
		bsoncxx::builder::basic::kvp("AnnotationID", annotationId)
	);

	return doc.extract();
}

void EntityMeshTetFace::readBSON(bsoncxx::document::view &trianglesObj, std::vector<EntityMeshTetTriangle> &triangles)
{
	try
	{
		// This is new code for reading the triangles where each triangle can have an arbitrary order
		auto nodes = trianglesObj["Nodes"].get_array().value;

		auto n = nodes.begin();

		// Read the number of triangles
		long long numberTriangles = DataBase::GetIntFromArrayViewIterator(n);
		n++;

		for (unsigned long index = 0; index < numberTriangles; index++)
		{
			int numberNodes = (int) DataBase::GetIntFromArrayViewIterator(n);
			n++;

			EntityMeshTetTriangle triangle;
			triangle.setNumberOfNodes(numberNodes);

			for (int iNode = 0; iNode < numberNodes; iNode++)
			{
				long long nodeIndex = DataBase::GetIntFromArrayViewIterator(n);
				n++;

				triangle.setNode(iNode, nodeIndex);
			}

			triangles.push_back(triangle);
		}
	}
	catch (std::exception)
	{
		// This is for backward compatibility 
		auto node1 = trianglesObj["Node1"].get_array().value;
		auto node2 = trianglesObj["Node2"].get_array().value;
		auto node3 = trianglesObj["Node3"].get_array().value;

		size_t numberTriangles = std::distance(node1.begin(), node1.end());
		assert(numberTriangles == std::distance(node2.begin(), node2.end()));
		assert(numberTriangles == std::distance(node3.begin(), node3.end()));

		auto n1 = node1.begin();
		auto n2 = node2.begin();
		auto n3 = node3.begin();

		for (unsigned long index = 0; index < numberTriangles; index++)
		{
			EntityMeshTetTriangle triangle;
			triangle.setNumberOfNodes(3);

			triangle.setNode(0, DataBase::GetIntFromArrayViewIterator(n1));
			triangle.setNode(1, DataBase::GetIntFromArrayViewIterator(n2));
			triangle.setNode(2, DataBase::GetIntFromArrayViewIterator(n3));

			triangles.push_back(triangle);

			n1++;
			n2++;
			n3++;
		}
	}
}

void EntityMeshTetFace::readBSON(bsoncxx::document::view &annotationObj, std::vector<std::pair<bool, ot::UID>> &faceAnnotationIDs)
{
	auto orientation  = annotationObj["Orientation"].get_array().value;
	auto annotationId = annotationObj["AnnotationID"].get_array().value;

	size_t numberAnnotations = std::distance(orientation.begin(), orientation.end());
	assert(numberAnnotations == std::distance(annotationId.begin(), annotationId.end()));

	auto orient = orientation.begin();
	auto id = annotationId.begin();

	for (unsigned long index = 0; index < numberAnnotations; index++)
	{
		faceAnnotationIDs.push_back(std::make_pair(orient->get_bool(), DataBase::GetIntFromArrayViewIterator(id)));

		orient++;
		id++;
	}
}

