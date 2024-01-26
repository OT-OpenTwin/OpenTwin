#pragma once
#include "OTServiceFoundation/ModelComponent.h"
#include "ClassFactory.h"


class Application
{
public:
	static Application* instance()
	{
		static Application INSTANCE;
		return &INSTANCE;
	}

	ot::components::ModelComponent* modelComponent();
	ClassFactory&  getClassFactory();
	
	void prefetchDocumentsFromStorage(const std::list<ot::UID>& entities);
	void prefetchDocumentsFromStorage(const std::list<ot::EntityInformation>& entityInfo);
	ot::UID getPrefetchedEntityVersion(ot::UID entityID);
private:
	ClassFactory _classFactory;
};
