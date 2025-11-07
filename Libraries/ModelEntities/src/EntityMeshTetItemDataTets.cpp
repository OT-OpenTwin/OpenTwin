// @otlicense
// File: EntityMeshTetItemDataTets.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end


#include "EntityMeshTetItemDataTets.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include "Connection\ConnectionAPI.h"

#include <mongocxx/client.hpp>
#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityMeshTetItemDataTets> registrar("EntityMeshTetItemDataTets");

EntityMeshTetItemDataTets::EntityMeshTetItemDataTets(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityBase(ID, parent, obs, ms)
{
	
}

EntityMeshTetItemDataTets::~EntityMeshTetItemDataTets()
{

}

bool EntityMeshTetItemDataTets::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityMeshTetItemDataTets::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Add the specific data of the mesh item (tets)

	auto nodes = bsoncxx::builder::basic::array();

	nodes.append((long long) tets.size());

	for (auto tet : tets)
	{
		nodes.append((int) tet.getNumberOfNodes());

		for (int index = 0; index < tet.getNumberOfNodes(); index++)
		{
			nodes.append((long long) tet.getNode(index));
		}
	}

	storage.append(bsoncxx::builder::basic::kvp("Nodes", nodes));
}

void EntityMeshTetItemDataTets::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	try
	{
		// This is new code for reading the tets where each tet can have an arbitrary order
		auto nodes = doc_view["Nodes"].get_array().value;

		auto n = nodes.begin();

		// Read the number of triangles
		long long numberTets = DataBase::getIntFromArrayViewIterator(n);
		n++;

		setNumberTets(numberTets);

		for (unsigned long index = 0; index < numberTets; index++)
		{
			int numberNodes = (int) DataBase::getIntFromArrayViewIterator(n);
			n++;

			for (int iNode = 0; iNode < numberNodes; iNode++)
			{
				long long nodeIndex = DataBase::getIntFromArrayViewIterator(n);
				n++;

				setTetNode(index, iNode, nodeIndex);
			}
		}
	}
	catch (std::exception)
	{
		// This is for backward compatibility

		// Check whetehr the information is stored in the GridFS
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
			size_t numberTets = doc_view["NumberTets"].get_int64();
			auto fileId = doc_view["FileId"].get_value();

			auto db = DataStorageAPI::ConnectionAPI::getInstance().getDatabase("ProjectsLargeData");
			auto bucket = db.gridfs_bucket();

			auto downloader = bucket.open_download_stream(fileId);
			size_t fileLength = downloader.file_length();

			assert(fileLength == 4 * sizeof(size_t) * numberTets);

			size_t *buffer = new size_t[numberTets];
			tets.resize(numberTets);

			for (int nodeIndex = 0; nodeIndex < 4; nodeIndex++)
			{
				downloader.read((uint8_t *)buffer, numberTets * sizeof(size_t) / sizeof(uint8_t));

				for (size_t index = 0; index < numberTets; index++) tets[index].setNode(nodeIndex, buffer[index]);
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
			auto node3 = doc_view["Node3"].get_array().value;
			auto node4 = doc_view["Node4"].get_array().value;

			size_t numberTets = std::distance(node1.begin(), node1.end());
			assert(numberTets == std::distance(node2.begin(), node2.end()));
			assert(numberTets == std::distance(node3.begin(), node3.end()));
			assert(numberTets == std::distance(node4.begin(), node4.end()));

			auto n1 = node1.begin();
			auto n2 = node2.begin();
			auto n3 = node3.begin();
			auto n4 = node4.begin();

			setNumberTets(numberTets);

			for (unsigned long tindex = 0; tindex < numberTets; tindex++)
			{
				setTetNode(tindex, 0, DataBase::getIntFromArrayViewIterator(n1));
				setTetNode(tindex, 1, DataBase::getIntFromArrayViewIterator(n2));
				setTetNode(tindex, 2, DataBase::getIntFromArrayViewIterator(n3));
				setTetNode(tindex, 3, DataBase::getIntFromArrayViewIterator(n4));

				n1++;
				n2++;
				n3++;
				n4++;
			}
		}
	}

	resetModified();
}
