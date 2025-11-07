// @otlicense
// File: EntityMeshCartesianFace.cpp
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


#include "EntityMeshCartesianFace.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityMeshCartesianFace> registrar("EntityMeshCartesianFace");

EntityMeshCartesianFace::EntityMeshCartesianFace(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityBase(ID, parent, obs, ms),
	type(INDEX_BASED),
	surfaceId(0)
{

}

void EntityMeshCartesianFace::setNumberCellFaces(int direction, size_t nT) 
{ 
	assert(type == INDEX_BASED);

	assert(direction >= 0 && direction < 3); 
	cellFaces[direction].resize(nT); 
	
	setModified(); 
}

void EntityMeshCartesianFace::setNumberCellFaces(size_t nT) 
{ 
	assert(type == POINT_BASED);

	pointIndices[0].resize(nT);
	pointIndices[1].resize(nT);
	pointIndices[2].resize(nT);
	pointIndices[3].resize(nT);

	setModified(); 
}

size_t EntityMeshCartesianFace::getNumberCellFaces(int direction) 
{
	assert(type == INDEX_BASED);

	assert(direction >= 0 && direction < 3); 
	return cellFaces[direction].size(); 
}

size_t EntityMeshCartesianFace::getNumberCellFaces(void) 
{
	assert(type == POINT_BASED);

	return pointIndices[0].size();
}

bool EntityMeshCartesianFace::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
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

void EntityMeshCartesianFace::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Now we store the particular information about the current object 
	storage.append(
		bsoncxx::builder::basic::kvp("Type", (int) type),
		bsoncxx::builder::basic::kvp("SurfaceId", surfaceId),
		bsoncxx::builder::basic::kvp("BoundingBox", boundingBox.getBSON())
	);

	if (type == INDEX_BASED)
	{
		auto fx = bsoncxx::builder::basic::array();
		for (auto face : cellFaces[0]) fx.append(face);
		storage.append(bsoncxx::builder::basic::kvp("CellFacesX", fx));

		auto fy = bsoncxx::builder::basic::array();
		for (auto face : cellFaces[1]) fy.append(face);
		storage.append(bsoncxx::builder::basic::kvp("CellFacesY", fy));

		auto fz = bsoncxx::builder::basic::array();
		for (auto face : cellFaces[2]) fz.append(face);
		storage.append(bsoncxx::builder::basic::kvp("CellFacesZ", fz));
	}
	else if (type == POINT_BASED)
	{
		auto p0 = bsoncxx::builder::basic::array();
		for (auto point : pointIndices[0]) p0.append((long long) point);
		storage.append(bsoncxx::builder::basic::kvp("P0", p0));

		auto p1 = bsoncxx::builder::basic::array();
		for (auto point : pointIndices[1]) p1.append((long long) point);
		storage.append(bsoncxx::builder::basic::kvp("P1", p1));

		auto p2 = bsoncxx::builder::basic::array();
		for (auto point : pointIndices[2]) p2.append((long long) point);
		storage.append(bsoncxx::builder::basic::kvp("P2", p2));

		auto p3 = bsoncxx::builder::basic::array();
		for (auto point : pointIndices[3]) p3.append((long long) point);
		storage.append(bsoncxx::builder::basic::kvp("P3", p3));
	}
	else
	{
		assert(0); // Unknown type
	}
}

void EntityMeshCartesianFace::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the Brep and Facet objects
	type = (eType) DataBase::getIntFromView(doc_view, "Type");
	surfaceId = (int) DataBase::getIntFromView(doc_view, "SurfaceId");
	boundingBox.setFromBSON(doc_view["BoundingBox"].get_document().value);

	if (type == INDEX_BASED)
	{
		readCellFaces(doc_view, 0, "CellFacesX");
		readCellFaces(doc_view, 1, "CellFacesY");
		readCellFaces(doc_view, 2, "CellFacesZ");
	}
	else if (type == POINT_BASED)
	{
		readPoints(doc_view, 0, "P0");
	}

	resetModified();
}

void EntityMeshCartesianFace::readCellFaces(bsoncxx::document::view &doc_view, int direction, const std::string &itemName)
{
	try
	{
		auto face = doc_view[itemName.c_str()].get_array().value;

		size_t numberFaces = std::distance(face.begin(), face.end());

		auto f = face.begin();

		setNumberCellFaces(direction, numberFaces);

		for (size_t findex = 0; findex < numberFaces; findex++)
		{
			setCellFace(direction, findex, DataBase::getIntFromArrayViewIterator(f));
			f++;
		}
	}
	catch (std::exception)
	{
		assert(0);
	}
}

void EntityMeshCartesianFace::readPoints(bsoncxx::document::view &doc_view, int index, const std::string &itemName)
{
	try
	{
		auto points = doc_view[itemName.c_str()].get_array().value;

		size_t numberPoints = std::distance(points.begin(), points.end());

		auto p = points.begin();

		pointIndices[index].resize(numberPoints);

		for (size_t pindex = 0; pindex < numberPoints; pindex++)
		{
			pointIndices[index][pindex] = DataBase::getIntFromArrayViewIterator(p);
			p++;
		}
	}
	catch (std::exception)
	{
		assert(0);
	}
}

