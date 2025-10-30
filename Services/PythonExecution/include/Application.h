// @otlicense
// File: Application.h
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

#pragma once

// OpenTwin header
#include "OTModelAPI/ModelServiceAPI.h"
#include "CommunicationHandler.h"

// Qt header
#include <QtCore/qobject.h>

class Application : public QObject {
	Q_OBJECT
public:
	static Application& instance(void);

	void setModelServiceURL(const std::string& url);
	void setUIServiceURL(const std::string& url);

	CommunicationHandler& getCommunicationHandler(void) { return m_communicationHandler; };

	void prefetchDocumentsFromStorage(const std::list<ot::UID>& entities);
	void prefetchDocumentsFromStorage(const std::list<ot::EntityInformation>& entityInfo);
	ot::UID getPrefetchedEntityVersion(ot::UID entityID);

private:
	Application();

	CommunicationHandler m_communicationHandler;

	std::map<ot::UID, ot::UID> m_prefetchedEntityVersions;
	std::string m_uiURL;
};
