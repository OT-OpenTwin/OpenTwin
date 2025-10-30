// @otlicense
// File: Application.cpp
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

#include "Application.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "DataBase.h"
#include "OTModelAPI/ModelAPIManager.h"

Application& Application::instance(void) {
	static Application g_instance;
	return g_instance;
}

void Application::setModelServiceURL(const std::string& url)
{
	ot::ModelAPIManager::setModelServiceURL(url);
}

void Application::setUIServiceURL(const std::string& url)
{
	m_uiURL = url;
}

void Application::prefetchDocumentsFromStorage(const std::list<ot::UID>& entities)
{
	// First get the version information for all entities
	std::list<ot::EntityInformation> entityInfo;
	ot::ModelServiceAPI::getEntityInformation(entities, entityInfo);

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

	DataBase::instance().prefetchDocumentsFromStorage(prefetchIdandVersion);
}
ot::UID Application::getPrefetchedEntityVersion(ot::UID entityID)
{
	OTAssert(m_prefetchedEntityVersions.count(entityID) > 0, "The entity was not prefetched");

	return m_prefetchedEntityVersions[entityID];
}

Application::Application() {
}