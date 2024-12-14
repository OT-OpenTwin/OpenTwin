#pragma once

// OpenTwin header
#include "OTServiceFoundation/ModelServiceAPI.h"
#include "CommunicationHandler.h"
#include "ClassFactory.h"
#include "ClassFactoryBlock.h"

// Qt header
#include <QtCore/qobject.h>

class Application : public QObject {
	Q_OBJECT
public:
	static Application& instance(void);

	ot::ModelServiceAPI& getModelServiceAPI() { return *m_modelServiceAPI; };
	void setModelServiceURL(const std::string& url);
	void setUIServiceURL(const std::string& url);

	ClassFactory& getClassFactory(void);
	CommunicationHandler& getCommunicationHandler(void) { return m_communicationHandler; };

	void prefetchDocumentsFromStorage(const std::list<ot::UID>& entities);
	void prefetchDocumentsFromStorage(const std::list<ot::EntityInformation>& entityInfo);
	ot::UID getPrefetchedEntityVersion(ot::UID entityID);

private:
	Application();

	CommunicationHandler m_communicationHandler;

	ClassFactory m_classFactory;
	ClassFactoryBlock m_classFactoryBlock;
	ot::ModelServiceAPI* m_modelServiceAPI;

	std::map<ot::UID, ot::UID> m_prefetchedEntityVersions;
	std::string m_uiURL;
};
