// @otlicense
// File: TriangleCADWriter.cpp
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

#include "TriangleCADWriter.h"
#include "OTCADEntities/EntityGeometry.h"
#include "OTCADEntities/GeometryOperations.h"
#include "Application.h"

#include "OTCommunication/Msg.h"
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/uiComponent.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelEntities/EntityFacetData.h"
#include "OTModelEntities/Geometry.h"

#include <iostream>
#include <filesystem>

#include "Precision.hxx"

#include "base64.h"
#include "zlib.h"

void TriangleCADWriter::getExportFileContent(std::string& data)
{
	// Get all geometry entities and append them
	ot::UIDList geometryEntities = getAllGeometryEntities();
	std::list<ot::EntityInformation> geometryInfo;
	ot::ModelServiceAPI::getEntityInformation(geometryEntities, geometryInfo);

	DataBase::instance().prefetchDocumentsFromStorage(geometryInfo);

	ot::UIDList facetIDs;
	std::map<ot::UID, std::string> facetNameMap;

	for (auto geometryItem : geometryInfo)
	{
		EntityGeometry* geomEntity = dynamic_cast<EntityGeometry*>(ot::EntityAPI::readEntityFromEntityIDandVersion(geometryItem.getEntityID(), geometryItem.getEntityVersion()));
		assert(geomEntity != nullptr);

		if (geomEntity != nullptr)
		{
			ot::UID facetID = geomEntity->getFacetsStorageObjectID();
			facetNameMap[facetID] = geomEntity->getName();
			facetIDs.push_back(facetID);

			delete geomEntity;
			geomEntity = nullptr;
		}
	}

	std::list<ot::EntityInformation> facetInfo;
	ot::ModelServiceAPI::getEntityInformation(facetIDs, facetInfo);

	DataBase::instance().prefetchDocumentsFromStorage(facetInfo);

	// Now process the facets and append them to the STL data
	std::stringstream dataStream;


	for (auto facetItem : facetInfo)
	{
		EntityFacetData* facetEntity = dynamic_cast<EntityFacetData*>(ot::EntityAPI::readEntityFromEntityIDandVersion(facetItem.getEntityID(), facetItem.getEntityVersion()));
		assert(facetEntity != nullptr);

		if (facetEntity != nullptr)
		{
			appendData(facetNameMap[facetItem.getEntityID()], facetEntity, dataStream);

			delete facetEntity;
			facetEntity = nullptr;
		}
	}

	data = dataStream.str();
}

std::list<ot::UID> TriangleCADWriter::getAllGeometryEntities(void)
{
	ot::JsonDocument reqDoc;
	reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_GetAllGeometryEntitiesForMeshing, reqDoc.GetAllocator()), reqDoc.GetAllocator());

	std::string response;
	application->getModelComponent()->sendMessage(false, reqDoc, response);

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);
	return ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
}

