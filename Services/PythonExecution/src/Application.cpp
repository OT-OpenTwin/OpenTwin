#include "Application.h"

ot::components::ModelComponent* Application::modelComponent()
{
	return nullptr;
}

ClassFactory& Application::getClassFactory()
{
	
	return _classFactory;
}

void Application::prefetchDocumentsFromStorage(const std::list<ot::UID>& entities)
{
}


void Application::prefetchDocumentsFromStorage(const std::list<ot::EntityInformation>& entityInfo)
{

}
ot::UID Application::getPrefetchedEntityVersion(ot::UID entityID)
{
	return ot::UID();
}
