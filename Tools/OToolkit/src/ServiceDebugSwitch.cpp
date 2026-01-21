// @otlicense
// File: ServiceDebugSwitch.cpp
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


// OpenTwin header
#include "SDSLSSEntry.h"
#include "ServiceDebugSwitch.h"
#include "OToolkitAPI/OToolkitAPI.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/GSSDebugInfo.h"
#include "OTCommunication/LSSDebugInfo.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/ExpanderWidget.h"
#include "OTWidgets/ToolTipHandler.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollarea.h>

// std header
#include <thread>

#define SDS_LOG(___message) OTOOLKIT_LOG("DebugSwitch", ___message)
#define SDS_LOGW(___message) OTOOLKIT_LOGW("DebugSwitch", ___message)
#define SDS_LOGE(___message) OTOOLKIT_LOGE("DebugSwitch", ___message)

bool ServiceDebugSwitch::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content)
{
	using namespace ot;

	QWidget* mainWidget = new QWidget;
	_content.addView(createCentralWidgetView(mainWidget, "Debug Switch"));

	QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);

	QHBoxLayout* topLayout = new QHBoxLayout;
	mainLayout->addLayout(topLayout);

	topLayout->addWidget(new Label("GSS URL:", mainWidget));
	m_gssUrl = new LineEdit(mainWidget);
	m_gssUrl->setPlaceholderText("e.g. 127.0.0.1:8091");
	topLayout->addWidget(m_gssUrl);

	PushButton* refreshButton = new PushButton("Refresh", mainWidget);
	connect(refreshButton, &PushButton::clicked, this, &ServiceDebugSwitch::refreshList);
	topLayout->addWidget(refreshButton);
	topLayout->addStretch();

	QScrollArea* scrollArea = new QScrollArea;
	mainLayout->addWidget(scrollArea, 1);
	scrollArea->setWidgetResizable(true);

	m_contentWidget = new QWidget;
	scrollArea->setWidget(m_contentWidget);
	clear();

	return true;
}

void ServiceDebugSwitch::restoreToolSettings(QSettings& _settings) {
	QString gssUrl = _settings.value("ServiceDebugSwitch.GSSUrl", "").toString();
	if (gssUrl.isEmpty()) {
		QByteArray envServicesUrl = qgetenv("OPEN_TWIN_SERVICES_ADDRESS");
		QByteArray envGssPort = qgetenv("OPEN_TWIN_GSS_PORT");
		if (!envServicesUrl.isEmpty() && !envGssPort.isEmpty()) {
			gssUrl = QString::fromUtf8(envServicesUrl) + ":" + QString::fromUtf8(envGssPort);
		}
	}

	if (!gssUrl.isEmpty()) {
		m_gssUrl->setText(gssUrl);
		refreshList();
	}
}

bool ServiceDebugSwitch::prepareToolShutdown(QSettings& _settings) {
	_settings.setValue("ServiceDebugSwitch.GSSUrl", m_gssUrl->text());

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public slots

void ServiceDebugSwitch::clear() {
	for (SDSLSSEntry* entry : m_services) {
		entry->getQWidget()->hide();
		delete entry;
	}
	m_services.clear();

	if (m_contentLayout) {
		delete m_contentLayout;
		m_contentLayout = nullptr;
	}
	m_contentLayout = new QVBoxLayout(m_contentWidget);
}

void ServiceDebugSwitch::refreshList() {
	clear();

	// Fetch data
	QString url = m_gssUrl->text();
	if (url.isEmpty()) {
		SDS_LOGW("GSS URL is empty");
		ot::ToolTipHandler::showToolTip(m_gssUrl, "Please specify the Global Session Service URL.");
		return;
	}

	m_gssUrl->setReadOnly(true);
	m_contentWidget->setEnabled(false);

	SDS_LOG("Fetching LSS list from GSS at \"" + url + "\"");

	std::thread worker(&ServiceDebugSwitch::workerFetchServices, this, url.toStdString());
	worker.detach();
}

void ServiceDebugSwitch::fetchCompleted(const std::list<ot::GSSDebugInfo::LSSData>& _lssData) {
	for (const ot::GSSDebugInfo::LSSData& lssData : _lssData) {
		ot::ServiceBase lssInfo;
		lssInfo.setServiceID(lssData.id);
		lssInfo.setServiceURL(lssData.url);
		lssInfo.setServiceType(OT_INFO_SERVICE_TYPE_LocalSessionService);
		lssInfo.setServiceName("LSS " + std::to_string(lssData.id) + " (" + lssInfo.getServiceURL() + ")");
		SDSLSSEntry* entry = new SDSLSSEntry(lssInfo, m_contentWidget);
		m_services.push_back(entry);
		m_contentLayout->addWidget(entry->getQWidget());

		if (_lssData.size() == 1) {
			entry->expandEntry();
		}

		entry->refreshData();
	}

	m_contentLayout->addStretch();

	m_gssUrl->setReadOnly(false);
	m_contentWidget->setEnabled(true);
}

void ServiceDebugSwitch::workerFetchServices(std::string _gssUrl) {
	using namespace ot;

	std::list<ot::GSSDebugInfo::LSSData> lssData;

	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_GetDebugInformation, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	std::string responseStr;
	if (!msg::send("", _gssUrl, ot::EXECUTE, requestDoc.toJson(), responseStr, 0, msg::DefaultFlagsNoExit)) {
		SDS_LOGE("Failed to send request to GSS at " + QString::fromStdString(_gssUrl));
		QMetaObject::invokeMethod(this, &ServiceDebugSwitch::fetchCompleted, Qt::QueuedConnection, lssData);
		return;
	}

	JsonDocument responseDoc;
	if (!responseDoc.fromJson(responseStr)) {
		SDS_LOGE("Failed to parse GSS response JSON");
		QMetaObject::invokeMethod(this, &ServiceDebugSwitch::fetchCompleted, Qt::QueuedConnection, lssData);
		return;
	}

	GSSDebugInfo gssDebugInfo;
	gssDebugInfo.setFromJsonObject(responseDoc.getConstObject());
	lssData = gssDebugInfo.getLSSList();

	QMetaObject::invokeMethod(this, &ServiceDebugSwitch::fetchCompleted, Qt::QueuedConnection, lssData);
}
