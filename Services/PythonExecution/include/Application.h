#pragma once

// OpenTwin header
#include "OTModelAPI/ModelServiceAPI.h"
#include "CommunicationHandler.h"
#include "ClassFactory.h"
#include "ClassFactoryBlock.h"

// Qt header
#include <QtCore/qobject.h>

class Application : public QObject {
	Q_OBJECT
public:
	static Application& instance(void);

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

	std::map<ot::UID, ot::UID> m_prefetchedEntityVersions;
	std::string m_uiURL;
};
