#pragma once
#include "OTServiceFoundation/ModelServiceAPI.h"
#include "ClassFactory.h"
#include "ClassFactoryBlock.h"

class Application
{
public:
	static Application* instance()
	{
		static Application INSTANCE;
		return &INSTANCE;
	}

	ot::ModelServiceAPI& ModelServiceAPI() { return *m_modelServiceAPI; };
	void setModelServiceURL(const std::string& url);
	void setUIServiceURL(const std::string& url);

	ClassFactory& getClassFactory();

	void prefetchDocumentsFromStorage(const std::list<ot::UID>& entities);
	void prefetchDocumentsFromStorage(const std::list<ot::EntityInformation>& entityInfo);
	ot::UID getPrefetchedEntityVersion(ot::UID entityID);

private:
	Application()
	{
		m_classFactory.SetNextHandler(&m_classFactoryBlock);
		m_classFactoryBlock.SetChainRoot(&m_classFactory);
	}
	ClassFactory m_classFactory;
	ClassFactoryBlock m_classFactoryBlock;
	ot::ModelServiceAPI* m_modelServiceAPI = nullptr;

	std::map<ot::UID, ot::UID> m_prefetchedEntityVersions;
	std::string m_uiURL;
};