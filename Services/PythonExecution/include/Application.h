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

	ot::ModelServiceAPI& ModelServiceAPI() { return *_modelServiceAPI; };
	void setModelServiceURL(const std::string& url);
	void setUIServiceURL(const std::string& url);

	ClassFactory&  getClassFactory();

	void prefetchDocumentsFromStorage(const std::list<ot::UID>& entities);
	void prefetchDocumentsFromStorage(const std::list<ot::EntityInformation>& entityInfo);
	ot::UID getPrefetchedEntityVersion(ot::UID entityID);

private:
	Application()
	{
		_classFactory.SetNextHandler(&_classFactoryBlock);
		_classFactoryBlock.SetChainRoot(&_classFactory);
	}
	ClassFactory _classFactory;
	ClassFactoryBlock _classFactoryBlock;
	ot::ModelServiceAPI* _modelServiceAPI = nullptr;

	std::map<ot::UID, ot::UID> m_prefetchedEntityVersions;
	std::string _uiURL;
};
