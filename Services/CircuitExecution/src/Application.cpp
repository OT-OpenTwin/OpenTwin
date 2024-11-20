/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

#include "Application.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "DataBase.h"

void Application::setModelServiceURL(const std::string& url)
{
	_modelServiceAPI = new ot::ModelServiceAPI("", url);
}

void Application::setUIServiceURL(const std::string& url)
{
	_uiURL = url;
}

ClassFactory& Application::getClassFactory()
{

	return _classFactory;
}

void Application::prefetchDocumentsFromStorage(const std::list<ot::UID>& entities)
{
	// First get the version information for all entities
	std::list<ot::EntityInformation> entityInfo;
	_modelServiceAPI->getEntityInformation(entities, entityInfo);

	// Now prefetch the documents
	prefetchDocumentsFromStorage(entityInfo);
	 
}


void Application::prefetchDocumentsFromStorage(const std::list<ot::EntityInformation>& entityInfo)
{
	std::list<std::pair<ot::UID, ot::UID>> prefetchIdandVersion;

	for (auto entity : entityInfo)
	{
		m_prefetchedEntityVersions[entity.getEntityID()] = entity.getEntityVersion();

		prefetchIdandVersion.push_back(std::pair<ot::UID, ot::UID>(entity.getEntityID(), entity.getEntityVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIdandVersion);
}
ot::UID Application::getPrefetchedEntityVersion(ot::UID entityID)
{
	OTAssert(m_prefetchedEntityVersions.count(entityID) > 0, "The entity was not prefetched");

	return m_prefetchedEntityVersions[entityID];
}