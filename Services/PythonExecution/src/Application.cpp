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

ClassFactory& Application::getClassFactory() {
	return m_classFactory;
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

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIdandVersion);
}
ot::UID Application::getPrefetchedEntityVersion(ot::UID entityID)
{
	OTAssert(m_prefetchedEntityVersions.count(entityID) > 0, "The entity was not prefetched");

	return m_prefetchedEntityVersions[entityID];
}

Application::Application() {
	m_classFactory.setNextHandler(&m_classFactoryBlock);
	m_classFactoryBlock.setChainRoot(&m_classFactory);
}