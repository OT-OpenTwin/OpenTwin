
#include "EntityFacetData.h"
#include "DataBase.h"
#include "Types.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityFacetData::EntityFacetData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler*factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, factory, owner)
{

}

EntityFacetData::~EntityFacetData()
{

}

void EntityFacetData::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Store the facet information
	storage.append(
		bsoncxx::builder::basic::kvp("NumberOfNodes", (long long) nodes.size()),
		bsoncxx::builder::basic::kvp("NumberOfTriangles", (long long) triangles.size()),
		bsoncxx::builder::basic::kvp("NumberOfEdges", (long long) edges.size()),
		bsoncxx::builder::basic::kvp("Nodes", Geometry::getBSON(nodes)),
		bsoncxx::builder::basic::kvp("Triangles", Geometry::getBSON(triangles)),
		bsoncxx::builder::basic::kvp("Edges", Geometry::getBSON(edges)),
		bsoncxx::builder::basic::kvp("FaceNames", Geometry::getBSON(faceNameMap)),
		bsoncxx::builder::basic::kvp("Errors", errors)
	);
}

void EntityFacetData::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the facet data

	bsoncxx::document::view nodesObj      = doc_view["Nodes"].get_document().view();
	bsoncxx::document::view  trianglesObj = doc_view["Triangles"].get_document().view();
	bsoncxx::document::view  edgesObj = doc_view["Edges"].get_document().view();

	errors = doc_view["Errors"].get_utf8().value.data();

	Geometry::readBSON(nodesObj,     nodes);
	Geometry::readBSON(trianglesObj, triangles);
	Geometry::readBSON(edgesObj, edges);

	faceNameMap.clear();
	try 
	{
		bsoncxx::document::view  faceNamesObj = doc_view["FaceNames"].get_document().view();
		Geometry::readBSON(faceNamesObj, faceNameMap);
	}
	catch (std::exception)
	{

	}

	resetModified();
}
