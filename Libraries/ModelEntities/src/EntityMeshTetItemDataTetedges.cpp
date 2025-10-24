
#include "EntityMeshTetItemDataTetedges.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include "Connection\ConnectionAPI.h"

#include <mongocxx/client.hpp>
#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityMeshTetItemDataTetedges> registrar("EntityMeshTetItemDataTetedges");

EntityMeshTetItemDataTetedges::EntityMeshTetItemDataTetedges(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, owner)
{
	
}

EntityMeshTetItemDataTetedges::~EntityMeshTetItemDataTetedges()
{

}

bool EntityMeshTetItemDataTetedges::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityMeshTetItemDataTetedges::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Add the specific data of the mesh item (tets)

	auto n1 = bsoncxx::builder::basic::array();
	auto n2 = bsoncxx::builder::basic::array();

	for (auto edge : edges)
	{
		size_t n[2];
		edge.getNodes(n);

		n1.append((long long)n[0]);
		n2.append((long long)n[1]);
	}

	storage.append(bsoncxx::builder::basic::kvp("Node1", n1));
	storage.append(bsoncxx::builder::basic::kvp("Node2", n2));
}

void EntityMeshTetItemDataTetedges::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	bool readGridFS = true;

	try
	{
		doc_view["FileId"].get_oid();
	}
	catch (std::exception)
	{
		readGridFS = false;
	}

	if (readGridFS)
	{
		size_t numberEdges = doc_view["NumberEdges"].get_int64();
		auto fileId = doc_view["FileId"].get_value();

		auto db = DataStorageAPI::ConnectionAPI::getInstance().getDatabase("ProjectsLargeData");
		auto bucket = db.gridfs_bucket();

		auto downloader = bucket.open_download_stream(fileId);
		size_t fileLength = downloader.file_length();

		assert(fileLength == 2 * sizeof(size_t) * numberEdges);

		size_t *buffer = new size_t[numberEdges];
		edges.resize(numberEdges);

		for (int nodeIndex = 0; nodeIndex < 2; nodeIndex++)
		{
			downloader.read((uint8_t *)buffer, numberEdges * sizeof(size_t) / sizeof(uint8_t));

			for (size_t index = 0; index < numberEdges; index++) edges[index].setNode(nodeIndex, buffer[index]);
		}

		downloader.close();

		delete[] buffer;
		buffer = nullptr;
	}
	else
	{
		// Now we read the information about the mesh item (tets and face list)

		// Read the tets first
		auto node1 = doc_view["Node1"].get_array().value;
		auto node2 = doc_view["Node2"].get_array().value;

		size_t numberEdges = std::distance(node1.begin(), node1.end());
		assert(numberEdges == std::distance(node2.begin(), node2.end()));

		auto n1 = node1.begin();
		auto n2 = node2.begin();

		setNumberEdges(numberEdges);

		for (unsigned long eindex = 0; eindex < numberEdges; eindex++)
		{
			size_t n[2];
			n[0] = (size_t)DataBase::getIntFromArrayViewIterator(n1);
			n[1] = (size_t)DataBase::getIntFromArrayViewIterator(n2);

			setEdgeNodes(eindex, n);

			n1++;
			n2++;
		}
	}

	resetModified();
}
