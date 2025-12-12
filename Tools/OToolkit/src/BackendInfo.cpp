// @otlicense
// File: BackendInfo.cpp
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

// OToolkit header
#include "AppBase.h"
#include "BackendInfo.h"
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/Table.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/TableItem.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/TreeWidgetItem.h"
#include "OTWidgets/ExpanderWidget.h"
#include "OTWidgets/JsonTreeWidget.h"
#include "OTWidgets/IndicatorWidget.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// Qt header
#include <QtCore/qjsonparseerror.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qheaderview.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qfiledialog.h>

#define BACKINFO_LOG(___message) OTOOLKIT_LOG("Backend Info", ___message)
#define BACKINFO_LOGW(___message) OTOOLKIT_LOGW("Backend Info", ___message)
#define BACKINFO_LOGE(___message) OTOOLKIT_LOGE("Backend Info", ___message)

namespace intern {

	class CustomJsonTree : public QWidget {
	public:
		CustomJsonTree(QWidget* _parent)
			: QWidget(_parent)
		{
			QVBoxLayout* layout = new QVBoxLayout(this);
			QScrollArea* serviceArea = new QScrollArea(this);
			layout->addWidget(serviceArea, 1);
			serviceArea->setWidgetResizable(true);
			m_tree = new ot::JsonTreeWidget(serviceArea);
			serviceArea->setWidget(m_tree);

			QHBoxLayout* controlLayout = new QHBoxLayout;
			layout->addLayout(controlLayout);

			ot::PushButton* expandButton = new ot::PushButton("Expand All", this);
			controlLayout->addWidget(expandButton);
			connect(expandButton, &ot::PushButton::clicked, m_tree, &ot::JsonTreeWidget::expandAll);

			ot::PushButton* collapseButton = new ot::PushButton("Collapse All", this);
			controlLayout->addWidget(collapseButton);
			connect(collapseButton, &ot::PushButton::clicked, m_tree, &ot::JsonTreeWidget::collapseAll);

			controlLayout->addWidget(new ot::Label("Filter:", this));
			ot::LineEdit* filterEdit = new ot::LineEdit(this);
			filterEdit->setPlaceholderText("Filter...");
			filterEdit->setMinimumWidth(150);
			controlLayout->addWidget(filterEdit);
			connect(filterEdit, &ot::LineEdit::textChanged, m_tree, &ot::JsonTreeWidget::filterItems);

			controlLayout->addStretch(1);

			ot::PushButton* exportButton = new ot::PushButton("Export", this);
			controlLayout->addWidget(exportButton);
			connect(exportButton, &ot::PushButton::clicked, this, &CustomJsonTree::slotExport);
		}

		ot::JsonTreeWidget* getTree() const { return m_tree; };

	private Q_SLOTS:
		void slotExport() {
			QJsonDocument doc = m_tree->toJsonDocument();
			QByteArray jsonString = doc.toJson(QJsonDocument::Indented);
			if (jsonString.length() < 3) {
				BACKINFO_LOGW("No data to export");
				return;
			}

			QString fileName = QFileDialog::getSaveFileName(this, "Export Debug Info", "", "JSON Files (*.json);;All Files (*)");
			if (fileName.isEmpty()) {
				return;
			}

			QFile file(fileName);
			if (!file.open(QIODevice::WriteOnly)) {
				BACKINFO_LOGE("Could not open file for writing: " + fileName);
				return;
			}

			file.write(jsonString);
			file.close();
		}

	private:
		ot::JsonTreeWidget* m_tree;
	};

}

BackendInfo::BackendInfo() 
	: m_sectionsLayout(nullptr), m_loadButton(nullptr), m_cancelButton(nullptr),
	m_clearButton(nullptr), m_gssUrl(nullptr), m_loadThread(nullptr)
{
	connectAction(c_addServiceDebugInfoAction, this, &BackendInfo::handleAddServiceDebugInfo);
}

BackendInfo::~BackendInfo() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

QString BackendInfo::getToolName(void) const {
	return "Backend Info";
}

bool BackendInfo::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	QWidget* root = new QWidget;
	QVBoxLayout* layout = new QVBoxLayout(root);

	// Controls
	m_gssUrl = new ot::LineEdit(root);
	m_loadButton = new ot::PushButton("Load", root);
	m_cancelButton = new ot::PushButton("Cancel", root);
	m_clearButton = new ot::PushButton("Clear", root);

	QHBoxLayout* controlLayout = new QHBoxLayout;
	controlLayout->addWidget(new ot::Label("GSS URL:", root));
	controlLayout->addWidget(m_gssUrl);
	controlLayout->addWidget(m_loadButton);
	controlLayout->addWidget(m_cancelButton);
	controlLayout->addStretch(1);
	controlLayout->addWidget(m_clearButton);
	layout->addLayout(controlLayout);

	this->connect(m_loadButton, &ot::PushButton::clicked, this, &BackendInfo::slotLoad);
	this->connect(m_cancelButton, &ot::PushButton::clicked, this, &BackendInfo::slotCancel);
	this->connect(m_clearButton, &ot::PushButton::clicked, this, &BackendInfo::slotClear);

	// Content
	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidgetResizable(true);

	QWidget* scrollContent = new QWidget;
	scrollArea->setWidget(scrollContent);
	
	QVBoxLayout* sectionsLayoutWrap = new QVBoxLayout(scrollContent);
	sectionsLayoutWrap->setContentsMargins(0, 0, 0, 0);
	m_sectionsLayout = new QVBoxLayout;
	sectionsLayoutWrap->addLayout(m_sectionsLayout);
	sectionsLayoutWrap->addStretch(1);
	layout->addWidget(scrollArea, 1);

	_content.addView(this->createCentralWidgetView(root, this->getToolName()));

	return true;
}

void BackendInfo::restoreToolSettings(QSettings& _settings) {
	m_gssUrl->setText(_settings.value("BackedInfo.GSSUrl", "").toString());
}

bool BackendInfo::prepareToolShutdown(QSettings& _settings) {
	_settings.setValue("BackedInfo.GSSUrl", m_gssUrl->text());

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void BackendInfo::slotClear() {
	for (ot::ExpanderWidget* w : m_sections) {
		if (w) {
			w->setVisible(false);
			m_sectionsLayout->removeWidget(w);
			delete w;
		}
	}
	
	m_sections.clear();

	m_gssInfos.clear();
	m_lssInfos.clear();
	m_gdsInfos.clear();
	m_ldsInfos.clear();
}

void BackendInfo::slotLoad() {
	if (m_gssUrl->text().isEmpty()) {
		BACKINFO_LOGE("No GSS URL provided");
		return;
	}

	if (m_loadThread) {
		BACKINFO_LOGE("Loading already in progress");
		return;
	}

	this->slotClear();

	m_cancelButton->setEnabled(true);
	m_loadButton->setEnabled(false);
	m_clearButton->setEnabled(false);

	m_loadThread = new std::thread(&BackendInfo::loadWorker, this, m_gssUrl->text().toStdString());
}

void BackendInfo::slotCancel() {

}

void BackendInfo::slotAddGSS(const ot::GSSDebugInfo& _info) {
	ot::ExpanderWidget* mainExpander = new ot::ExpanderWidget("GSS { \"URL\": \"" + QString::fromStdString(_info.getUrl()) + "\" }", m_sectionsLayout->widget());

	QSplitter* mainSplitter = new QSplitter(mainExpander);
	mainSplitter->setMinimumHeight(400);

	QGroupBox* generalInfoGroup = new QGroupBox("General Info", mainSplitter);
	mainSplitter->addWidget(generalInfoGroup);

	QGridLayout* generalInfoLayout = new QGridLayout(generalInfoGroup);
	int r = 0;
	generalInfoLayout->addWidget(new ot::Label("Url:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getUrl(), generalInfoGroup), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("DB Url:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getDatabaseUrl(), generalInfoGroup), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Auth Url:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getAuthorizationUrl(), generalInfoGroup), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("GDS Url:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getGlobalDirectoryUrl(), generalInfoGroup), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("LMS Url:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getLibraryManagementUrl(), generalInfoGroup), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Worker Running:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.isWorkerRunning(), generalInfoGroup), r++, 1);

	generalInfoLayout->setRowStretch(r, 1);

	// Sessions
	Qt::ItemFlags cellFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	QGroupBox* sessionGroup = new QGroupBox("Sessions", mainSplitter);
	mainSplitter->addWidget(sessionGroup);

	QVBoxLayout* sessionLayout = new QVBoxLayout(sessionGroup);
	sessionLayout->setContentsMargins(0, 10, 0, 0);

	ot::Table* sessionTable = new ot::Table(_info.getSessionToLssList().size(), 2, sessionGroup);
	sessionTable->verticalHeader()->setVisible(false);
	sessionTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sessionTable->setHorizontalHeaderLabels({ "Session", "LSS ID" });
	sessionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	sessionTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
	r = 0;
	for (const auto& it : _info.getSessionToLssList()) {
		sessionTable->addItem(r, 0, QString::fromStdString(it.first))->setFlags(cellFlags);
		sessionTable->addItem(r++, 1, QString::number(it.second))->setFlags(cellFlags);
	}

	sessionLayout->addWidget(sessionTable);

	// LSS Info

	QGroupBox* lssInfoGroup = new QGroupBox("LSS Info", mainSplitter);
	mainSplitter->addWidget(lssInfoGroup);

	QVBoxLayout* lssInfoGroupLayout = new QVBoxLayout(lssInfoGroup);
	lssInfoGroupLayout->setContentsMargins(0, 10, 0, 0);

	QScrollArea* lssArea = new QScrollArea(lssInfoGroup);
	lssArea->setWidgetResizable(true);
	lssArea->setContentsMargins(0, 0, 0, 0);
	lssArea->setMinimumSize(400, 500);
	lssArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	lssArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	lssInfoGroupLayout->addWidget(lssArea);

	QWidget* lssMainLayoutWidget = new QWidget(lssArea);
	QVBoxLayout* lssMainLayout = new QVBoxLayout(lssMainLayoutWidget);
	lssMainLayout->setContentsMargins(0, 0, 0, 0);
	lssArea->setWidget(lssMainLayoutWidget);

	for (const auto& it : _info.getLSSList()) {
		ot::ExpanderWidget* expander = new ot::ExpanderWidget(QString::fromStdString("LSS { \"ID\": " + std::to_string(it.id) + ", \"URL\": \"" + it.url + "\" }"), lssMainLayoutWidget);

		QScrollArea* lssWidget = new QScrollArea(expander);
		lssWidget->setWidgetResizable(true);
		lssWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
		lssWidget->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
		lssWidget->setMinimumHeight(200);

		QWidget* lssContent = new QWidget(lssWidget);
		lssWidget->setWidget(lssContent);

		QVBoxLayout* lssLayout = new QVBoxLayout(lssContent);

		QGridLayout* lssInfoLayout = new QGridLayout;
		r = 0;
		lssInfoLayout->addWidget(new ot::Label("ID:", lssContent), r, 0);
		lssInfoLayout->addWidget(lineEdit(it.id, lssContent), r++, 1);

		lssInfoLayout->addWidget(new ot::Label("Url:", lssContent), r, 0);
		lssInfoLayout->addWidget(lineEdit(it.url, lssContent), r++, 1);

		lssLayout->addLayout(lssInfoLayout);

		// Active sessions
		lssLayout->addWidget(new ot::Label("Active Sessions:", lssContent));
		lssLayout->addWidget(this->createGssSessionTable(it.activeSessions, lssContent));

		// Initializing sessions
		lssLayout->addWidget(new ot::Label("Initializing Sessions:", lssContent));
		lssLayout->addWidget(this->createGssSessionTable(it.initializingSessions, lssContent));
	
		//expander->setMinExpandedHeight(450);
		expander->setWidget(lssWidget);
		lssMainLayout->addWidget(expander);
	}

	lssMainLayout->addStretch(1);

	//mainExpander->setMinExpandedHeight(500);
	mainExpander->setWidget(mainSplitter);

	m_sectionsLayout->addWidget(mainExpander);
	m_sections.push_back(mainExpander);
}

void BackendInfo::slotAddLSS(const ot::LSSDebugInfo& _info) {
	ot::ExpanderWidget* mainExpander = new ot::ExpanderWidget("LSS { \"ID\": " + QString::number(_info.getID()) + ", \"URL\": \"" + QString::fromStdString(_info.getURL()) + "\" }", m_sectionsLayout->widget());

	QSplitter* mainSplitter = new QSplitter(mainExpander);
	mainSplitter->setMinimumHeight(400);

	// General Info

	QGroupBox* generalInfoGroup = new QGroupBox("General Info", mainSplitter);
	mainSplitter->addWidget(generalInfoGroup);

	QVBoxLayout* generalInfoGroupLayout = new QVBoxLayout(generalInfoGroup);

	QScrollArea* infoArea = new QScrollArea;
	infoArea->setWidgetResizable(true);
	infoArea->setContentsMargins(0, 0, 0, 0);
	infoArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	infoArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	generalInfoGroupLayout->addWidget(infoArea);

	QWidget* infoAreaWidget = new QWidget(infoArea);
	infoArea->setWidget(infoAreaWidget);
	QVBoxLayout* infoAreaLayout = new QVBoxLayout(infoAreaWidget);

	QWidget* generalInfoLayoutWidget = new QWidget(infoAreaWidget);
	QGridLayout* generalInfoLayout = new QGridLayout(generalInfoLayoutWidget);

	int r = 0;
	generalInfoLayout->addWidget(new ot::Label("Worker Running:", generalInfoLayoutWidget), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getWorkerRunning(), generalInfoLayoutWidget), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Url:", generalInfoLayoutWidget), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getURL(), generalInfoLayoutWidget), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("ID:", generalInfoLayoutWidget), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getID(), generalInfoLayoutWidget), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("GSS Url:", generalInfoLayoutWidget), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getGSSUrl(), generalInfoLayoutWidget), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("GSS Connected:", generalInfoLayoutWidget), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getGSSConnected(), generalInfoLayoutWidget), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("GDS Url:", generalInfoLayoutWidget), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getGDSUrl(), generalInfoLayoutWidget), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("GDS Connected:", generalInfoLayoutWidget), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getGDSConnected(), generalInfoLayoutWidget), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Auth Url:", generalInfoLayoutWidget), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getAuthUrl(), generalInfoLayoutWidget), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Database Url:", generalInfoLayoutWidget), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getDataBaseUrl(), generalInfoLayoutWidget), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("LMS Url:", generalInfoLayoutWidget), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getLMSUrl(), generalInfoLayoutWidget), r++, 1);

	infoAreaLayout->addWidget(generalInfoLayoutWidget);

	QWidget* mandatoryServicesWidget = new QWidget(infoAreaWidget);
	QVBoxLayout* mandatoryServicesLayout = new QVBoxLayout(mandatoryServicesWidget);
	
	for (const auto& it : _info.getMandatoryServices()) {
		QListWidget* mandatoryWidget = new QListWidget(mandatoryServicesWidget);
		for (const auto& service : it.second) {
			QListWidgetItem* itm = new QListWidgetItem(QString::fromStdString(service));
			itm->setFlags(Qt::ItemIsEnabled);
			mandatoryWidget->addItem(itm);
		}

		ot::ExpanderWidget* expander = new ot::ExpanderWidget(QString::fromStdString("Madatory Services (" + it.first + ")"), mandatoryServicesWidget);
		expander->setWidget(mandatoryWidget);
		mandatoryServicesLayout->addWidget(expander);
	}

	infoAreaLayout->addWidget(mandatoryServicesWidget);

	infoAreaLayout->addStretch(1);

	// Additional Info

	QGroupBox* additionalInfoGroup = new QGroupBox("Additional Info", mainSplitter);
	mainSplitter->addWidget(additionalInfoGroup);

	QVBoxLayout* additionalInfoGroupLayout = new QVBoxLayout(additionalInfoGroup);

	QScrollArea* additionalInfoArea = new QScrollArea(additionalInfoGroup);
	additionalInfoArea->setWidgetResizable(true);
	additionalInfoArea->setContentsMargins(0, 0, 0, 0);
	additionalInfoArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	additionalInfoArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	additionalInfoGroupLayout->addWidget(additionalInfoArea);

	QWidget* additionalInfoLayoutWidget = new QWidget(additionalInfoArea);
	additionalInfoArea->setWidget(additionalInfoLayoutWidget);

	QVBoxLayout* additionalInfoLayout = new QVBoxLayout(additionalInfoLayoutWidget);

	ot::ExpanderWidget* debugServicesExpander = new ot::ExpanderWidget("Debug Services (" + QString::number(_info.getDebugServices().size()) + ")", additionalInfoLayoutWidget);

	QListWidget* debugServicesList = new QListWidget(debugServicesExpander);
	for (const auto& it : _info.getDebugServices()) {
		QListWidgetItem* itm = new QListWidgetItem(QString::fromStdString(it));
		itm->setFlags(Qt::ItemIsEnabled);
		debugServicesList->addItem(itm);
	}
	debugServicesExpander->setWidget(debugServicesList);
	additionalInfoLayout->addWidget(debugServicesExpander);

	ot::ExpanderWidget* shutdownQueueExpander = new ot::ExpanderWidget("Shutdown Queue (" + QString::number(_info.getShutdownQueue().size()) + ")", additionalInfoLayoutWidget);

	ot::TreeWidget* shutdownQueueTree = new ot::TreeWidget(shutdownQueueExpander);
	shutdownQueueTree->setColumnCount(2);
	shutdownQueueTree->setHeaderLabels({ "SessionID", "Emergency" });
	for (const auto& it : _info.getShutdownQueue()) {
		ot::TreeWidgetItem* item = new ot::TreeWidgetItem;
		item->setText(0, QString::fromStdString(it.first));
		item->setText(1, it.second ? "true" : "");
		shutdownQueueTree->addTopLevelItem(item);
	}
	shutdownQueueExpander->setWidget(shutdownQueueTree);
	additionalInfoLayout->addWidget(shutdownQueueExpander);

	ot::ExpanderWidget* shutdownCompletedExpander = new ot::ExpanderWidget("Shutdown Completed (" + QString::number(_info.getShutdownCompletedQueue().size()) + ")", additionalInfoLayoutWidget);

	QListWidget* shutdownCompletedList = new QListWidget(shutdownCompletedExpander);
	for (const auto& it : _info.getShutdownCompletedQueue()) {
		QListWidgetItem* itm = new QListWidgetItem(QString::fromStdString(it));
		itm->setFlags(Qt::ItemIsEnabled);
		shutdownCompletedList->addItem(itm);
	}
	
	shutdownCompletedExpander->setWidget(shutdownCompletedList);
	additionalInfoLayout->addWidget(shutdownCompletedExpander);

	ot::ExpanderWidget* blockedPortsExpander = new ot::ExpanderWidget("Blocked Ports (" + QString::number(_info.getBlockedPorts().size()) + ")", additionalInfoLayoutWidget);

	QListWidget* blockedPortsList = new QListWidget(blockedPortsExpander);
	for (const auto& it : _info.getBlockedPorts()) {
		QListWidgetItem* itm = new QListWidgetItem(QString::number(it));
		itm->setFlags(Qt::ItemIsEnabled);
		blockedPortsList->addItem(itm);
	}
	
	blockedPortsExpander->setWidget(blockedPortsList);
	additionalInfoLayout->addWidget(blockedPortsExpander);

	additionalInfoLayout->addStretch(1);

	// Sessions

	QGroupBox* sessionsGroup = new QGroupBox("Sessions", mainSplitter);
	mainSplitter->addWidget(sessionsGroup);

	QVBoxLayout* sessionsGroupLayout = new QVBoxLayout(sessionsGroup);

	QScrollArea* sessionsArea = new QScrollArea(sessionsGroup);
	sessionsArea->setWidgetResizable(true);
	sessionsArea->setContentsMargins(0, 0, 0, 0);
	sessionsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	sessionsArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	sessionsGroupLayout->addWidget(sessionsArea);

	QWidget* sessionsAreaWidget = new QWidget(sessionsArea);
	sessionsArea->setWidget(sessionsAreaWidget);

	QVBoxLayout* sessionsAreaLayout = new QVBoxLayout(sessionsAreaWidget);

	for (const auto& it : _info.getSessions()) {
		ot::ExpanderWidget* expander = new ot::ExpanderWidget(QString::fromStdString("Session { \"ID\": \"" + it.id + "\" }"), sessionsAreaWidget);

		QScrollArea* sessionArea = new QScrollArea(expander);
		sessionArea->setWidgetResizable(true);

		QWidget* sessionContent = new QWidget(sessionArea);
		sessionArea->setWidget(sessionContent);

		QVBoxLayout* sessionLayout = new QVBoxLayout(sessionContent);
		
		QGridLayout* sessionInfoLayout = new QGridLayout;
		r = 0;
		sessionInfoLayout->addWidget(new ot::Label("ID:", sessionContent), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.id, sessionContent), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Type:", sessionContent), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.type, sessionContent), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("User Name:", sessionContent), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.userName, sessionContent), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Project Name:", sessionContent), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.projectName, sessionContent), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Collection Name:", sessionContent), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.collectionName, sessionContent), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("User Collection:", sessionContent), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.userCollection, sessionContent), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("User Credentials Name:", sessionContent), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.userCredName, sessionContent), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("User Credentials Password:", sessionContent), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.userCredPassword, sessionContent), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Database Credentials Name:", sessionContent), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.dataBaseCredName, sessionContent), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Database Credentials Password:", sessionContent), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.dataBaseCredPassword, sessionContent), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Health Check Running:", sessionContent), r, 0);
		sessionInfoLayout->addWidget(checkBox(it.isHealthCheckRunning, sessionContent), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Shutting Down:", sessionContent), r, 0);
		sessionInfoLayout->addWidget(checkBox(it.isShuttingDown, sessionContent), r++, 1);

		sessionLayout->addLayout(sessionInfoLayout);

		// Services

		sessionLayout->addWidget(new ot::Label("Services:", sessionContent));

		sessionLayout->addWidget(this->createLssServicesTable(it.services, sessionContent));

		//expander->setMinExpandedHeight(800);
		expander->setWidget(sessionArea);
		sessionsAreaLayout->addWidget(expander);
	}

	//mainExpander->setMinExpandedHeight(500);
	mainExpander->setWidget(mainSplitter);

	m_sectionsLayout->addWidget(mainExpander);
	m_sections.push_back(mainExpander);
}

void BackendInfo::slotAddGDS(const ot::GDSDebugInfo& _info) {
	ot::ExpanderWidget* mainExpander = new ot::ExpanderWidget("GDS { \"URL\": \"" + QString::fromStdString(_info.getURL()) + "\" }", m_sectionsLayout->widget());

	QSplitter* mainSplitter = new QSplitter(mainExpander);
	mainSplitter->setMinimumHeight(400);

	QGroupBox* generalInfoGroup = new QGroupBox("General Info", mainSplitter);
	mainSplitter->addWidget(generalInfoGroup);

	QGridLayout* generalInfoLayout = new QGridLayout(generalInfoGroup);
	int r = 0;
	generalInfoLayout->addWidget(new ot::Label("Url:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getURL(), generalInfoGroup), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("GSS Url:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getGSSUrl(), generalInfoGroup), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("Startup Worker Running:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getStartupWorkerRunning(), generalInfoGroup), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("Startup Worker Stopping:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getStartupWorkerStopping(), generalInfoGroup), r++, 1);
	generalInfoLayout->setRowStretch(r, 1);

	// Requested Services
	QGroupBox* requestedServicesGroup = new QGroupBox("Requested Services", mainSplitter);
	mainSplitter->addWidget(requestedServicesGroup);

	QVBoxLayout* requestedServicesLayout = new QVBoxLayout(requestedServicesGroup);
	requestedServicesLayout->addWidget(this->createGdsServicesTable(_info.getRequestedServices(), requestedServicesGroup));

	// Local Directory Services Info
	QGroupBox* ldsInfoGroup = new QGroupBox("LDS Info", mainSplitter);
	mainSplitter->addWidget(ldsInfoGroup);

	QVBoxLayout* ldsInfoGroupLayout = new QVBoxLayout(ldsInfoGroup);
	ldsInfoGroupLayout->setContentsMargins(0, 10, 0, 0);

	QScrollArea* ldsArea = new QScrollArea(ldsInfoGroup);
	ldsArea->setWidgetResizable(true);
	ldsArea->setContentsMargins(0, 0, 0, 0);
	ldsArea->setMinimumSize(400, 500);
	ldsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	ldsArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	ldsInfoGroupLayout->addWidget(ldsArea);

	QVBoxLayout* ldsMainLayout = new QVBoxLayout;
	ldsMainLayout->setContentsMargins(0, 0, 0, 0);
	ldsArea->setLayout(ldsMainLayout);

	for (const auto& it : _info.getLocalDirectoryServices()) {
		QScrollArea* ldsWidget = new QScrollArea;
		ldsWidget->setWidgetResizable(true);
		ldsWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
		ldsWidget->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

		QWidget* ldsContent = new QWidget;
		ldsWidget->setWidget(ldsContent);

		QVBoxLayout* ldsLayout = new QVBoxLayout(ldsContent);
		
		ldsLayout->addWidget(new ot::Label("Supported Services:", ldsContent));
		QListWidget* supportedServicesList = new QListWidget;
		for (const auto& service : it.supportedServices) {
			QListWidgetItem* itm = new QListWidgetItem(QString::fromStdString(service));
			itm->setFlags(Qt::ItemIsEnabled);
			supportedServicesList->addItem(itm);
		}
		ldsLayout->addWidget(supportedServicesList);

		ldsLayout->addWidget(new ot::Label("Services:", ldsContent));
		ldsLayout->addWidget(this->createGdsServicesTable(it.services, ldsContent), 1);

		ot::ExpanderWidget* expander = new ot::ExpanderWidget(QString::fromStdString("LDS { \"ID\": " + std::to_string(it.serviceID) + ", \"URL\": \"" + it.url + "\" }"), ldsArea);
		//expander->setMinExpandedHeight(450);
		expander->setWidget(ldsWidget);
		ldsMainLayout->addWidget(expander);
	}
	ldsMainLayout->addStretch(1);
	
	//mainExpander->setMinExpandedHeight(500);
	mainExpander->setWidget(mainSplitter);

	m_sectionsLayout->addWidget(mainExpander);
	m_sections.push_back(mainExpander);
}

void BackendInfo::slotAddLDS(const ot::LDSDebugInfo& _info) {
	ot::ExpanderWidget* mainExpander = new ot::ExpanderWidget("LDS { \"ID\": " + QString::number(_info.getId()) + ", \"URL\": \"" + QString::fromStdString(_info.getURL()) + "\" }", m_sectionsLayout->widget());

	QSplitter* mainSplitter = new QSplitter(mainExpander);
	mainSplitter->setMinimumHeight(400);

	// General Info

	QGroupBox* generalInfoGroup = new QGroupBox("General Info", mainSplitter);
	mainSplitter->addWidget(generalInfoGroup);

	QGridLayout* generalInfoLayout = new QGridLayout(generalInfoGroup);
	int r = 0;

	generalInfoLayout->addWidget(new ot::Label("Url:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getURL(), generalInfoGroup), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("ID:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getId(), generalInfoGroup), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("GDS Url:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getGDSURL(), generalInfoGroup), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("GDS Connected:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getGDSConnected(), generalInfoGroup), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Services IP:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getServicesIPAddress(), generalInfoGroup), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("Last Error:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getLastError(), generalInfoGroup), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Worker Running:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getWorkerRunning(), generalInfoGroup), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("Check Alive Frequency:", generalInfoGroup), r, 0);
	generalInfoLayout->addWidget(lineEdit(QString::number(_info.getServiceCheckAliveFrequency()) + "s", generalInfoGroup), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Used Ports:", generalInfoGroup), r, 0);
	QListWidget* usedPortsList = new QListWidget(generalInfoGroup);
	for (const auto& it : _info.getUsedPorts()) {
		QListWidgetItem* itm = new QListWidgetItem(QString::number(it));
		itm->setFlags(Qt::ItemIsEnabled);
		usedPortsList->addItem(itm);
	}
	generalInfoLayout->addWidget(usedPortsList, r++, 1);
	generalInfoLayout->setRowStretch(r, 1);

	// Config
	QGroupBox* configGroup = new QGroupBox("Config", mainSplitter);
	mainSplitter->addWidget(configGroup);

	QVBoxLayout* configGroupLayout = new QVBoxLayout(configGroup);
	QScrollArea* configArea = new QScrollArea(configGroup);
	configArea->setWidgetResizable(true);
	configArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	configArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	configGroupLayout->addWidget(configArea);

	QWidget* configContent = new QWidget(configArea);
	configArea->setWidget(configContent);
	QVBoxLayout* configLayout = new QVBoxLayout(configContent);

	QGridLayout* configInfoLayout = new QGridLayout;
	r = 0;
	configInfoLayout->addWidget(new ot::Label("Config Imported:", configContent), r, 0);
	configInfoLayout->addWidget(checkBox(_info.getConfig().configImported, configContent), r++, 1);
	configInfoLayout->addWidget(new ot::Label("Launcher Path:", configContent), r, 0);
	configInfoLayout->addWidget(lineEdit(_info.getConfig().launcherPath, configContent), r++, 1);
	configInfoLayout->addWidget(new ot::Label("Services Path:", configContent), r, 0);
	configInfoLayout->addWidget(lineEdit(_info.getConfig().servicesLibraryPath, configContent), r++, 1);
	configInfoLayout->addWidget(new ot::Label("Start Workers:", configContent), r, 0);
	configInfoLayout->addWidget(lineEdit(QString::number(_info.getConfig().serviceStartWorkerCount), configContent), r++, 1);
	configInfoLayout->addWidget(new ot::Label("Initialize Workers:", configContent), r, 0);
	configInfoLayout->addWidget(lineEdit(QString::number(_info.getConfig().iniWorkerCount), configContent), r++, 1);
	configInfoLayout->addWidget(new ot::Label("Default Startup Restarts:", configContent), r, 0);
	configInfoLayout->addWidget(lineEdit(QString::number(_info.getConfig().defaultMaxStartupRestarts), configContent), r++, 1);
	configInfoLayout->addWidget(new ot::Label("Default Crash Restarts:", configContent), r, 0);
	configInfoLayout->addWidget(lineEdit(QString::number(_info.getConfig().defaultMaxCrashRestarts), configContent), r++, 1);
	configLayout->addLayout(configInfoLayout);

	configLayout->addWidget(new ot::Label("Supported Services:", configContent));
	QStringList supportedServicesHeader = { "Name", "Type", "Crash Restarts", "Startup Restarts" };
	ot::Table* supportedServicesTable = new ot::Table(_info.getConfig().supportedServices.size(), supportedServicesHeader.size(), configContent);
	configLayout->addWidget(supportedServicesTable, 1);
	supportedServicesTable->verticalHeader()->setVisible(false);
	supportedServicesTable->setHorizontalHeaderLabels(supportedServicesHeader);
	supportedServicesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	supportedServicesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
	supportedServicesTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	Qt::ItemFlags cellFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	r = 0;
	for (const auto& it : _info.getConfig().supportedServices) {
		supportedServicesTable->addItem(r, 0, QString::fromStdString(it.name))->setFlags(cellFlags);
		supportedServicesTable->addItem(r, 1, QString::fromStdString(it.type))->setFlags(cellFlags);
		supportedServicesTable->addItem(r, 2, QString::number(it.maxCrashRestarts))->setFlags(cellFlags);
		supportedServicesTable->addItem(r, 3, QString::number(it.maxStartupRestarts))->setFlags(cellFlags);
		r++;
	}

	// Services
	QGroupBox* servicesGroup = new QGroupBox("Service Manager", mainSplitter);
	mainSplitter->addWidget(servicesGroup);

	QVBoxLayout* servicesGroupLayout = new QVBoxLayout(servicesGroup);
	QScrollArea* servicesArea = new QScrollArea(servicesGroup);
	servicesArea->setWidgetResizable(true);
	servicesArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	servicesArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	servicesGroupLayout->addWidget(servicesArea);

	QWidget* servicesContent = new QWidget(servicesArea);
	servicesArea->setWidget(servicesContent);

	QVBoxLayout* servicesLayout = new QVBoxLayout(servicesContent);
	
	ot::ExpanderWidget* aliveSessionsExpander = new ot::ExpanderWidget("Alive Sessions (" + QString::number(_info.getAliveSessions().size()) + ")", servicesContent);

	ot::LDSDebugInfo::SessionInfo dummySession;
	createLDSServiceInfo(dummySession, _info.getRequestedServices(), LDSServiceInfoMode::RequestedServices, servicesLayout, servicesContent);
	createLDSServiceInfo(dummySession, _info.getInitializingServices(), LDSServiceInfoMode::InitializingServices, servicesLayout, servicesContent);

	QWidget* aliveSessionsWidget = new QWidget(aliveSessionsExpander);
	QVBoxLayout* aliveSessionsLayout = new QVBoxLayout(aliveSessionsWidget);
	int minHeight = 0;
	for (const auto& it : _info.getAliveSessions()) {
		minHeight += createLDSServiceInfo(it.first, it.second, LDSServiceInfoMode::AliveServices, aliveSessionsLayout, aliveSessionsWidget);
	}
	
	aliveSessionsExpander->setWidget(aliveSessionsWidget);
	//aliveSessionsExpander->setMinExpandedHeight(minHeight + 100);
	servicesLayout->addWidget(aliveSessionsExpander);

	createLDSServiceInfo(dummySession, _info.getFailedServices(), LDSServiceInfoMode::FailedServices, servicesLayout, servicesContent);
	createLDSServiceInfo(dummySession, _info.getNewStoppingServices(), LDSServiceInfoMode::NewStoppingServices, servicesLayout, servicesContent);
	createLDSServiceInfo(dummySession, _info.getStoppingServices(), LDSServiceInfoMode::StoppingServices, servicesLayout, servicesContent);

	servicesLayout->addStretch(1);
	
	//mainExpander->setMinExpandedHeight(500);
	mainExpander->setWidget(mainSplitter);
	
	m_sectionsLayout->addWidget(mainExpander);
	m_sections.push_back(mainExpander);
	
}

void BackendInfo::slotAddService(const std::string& _serviceName, const std::string& _serviceId, const std::string& _serviceUrl, const std::string& _debugInfoJson) {
	if (_debugInfoJson.empty()) {
		BACKINFO_LOGW("No debug information provided { \"Name\": \"" + QString::fromStdString(_serviceName) + "\", \"ID\": \"" + QString::fromStdString(_serviceId) + "\", \"Url\": \"" + QString::fromStdString(_serviceUrl) + "\" }");
		return;
	}

	// Parse JSON
	QJsonParseError err;
	QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(_debugInfoJson), &err);
	if (err.error != QJsonParseError::NoError) {
		BACKINFO_LOGW("Failed to parse service debug information { \"Name\": \"" + QString::fromStdString(_serviceName) + "\", \"ID\": \"" + QString::fromStdString(_serviceId) + "\", \"Url\": \"" + QString::fromStdString(_serviceUrl) + "\", \"Error\": \"" + err.errorString() + "\" }");
		BACKINFO_LOGW("JSON: " + QString::fromStdString(_debugInfoJson));
		return;
	}

	// Create widget
	ot::ExpanderWidget* serviceExpander = new ot::ExpanderWidget(QString::fromStdString("Service { \"Name\": \"" + _serviceName + "\", \"ID\": \"" + _serviceId + "\", \"Url\": \"" + _serviceUrl + "\" }"), m_sectionsLayout->widget());
	intern::CustomJsonTree* tree = new intern::CustomJsonTree(serviceExpander);
	tree->getTree()->setReadOnly(true);
	tree->getTree()->setJsonDocument(doc);
	serviceExpander->setWidget(tree);

	m_sectionsLayout->addWidget(serviceExpander);
	m_sections.push_back(serviceExpander);
}

void BackendInfo::loadWorkerFinished() {
	if (m_loadThread) {
		if (m_loadThread->joinable()) {
			m_loadThread->join();
		}
		delete m_loadThread;
		m_loadThread = nullptr;
	}

	m_cancelButton->setEnabled(false);
	m_loadButton->setEnabled(true);
	m_clearButton->setEnabled(true);

	BACKINFO_LOG("Getting Backend data worker finished");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Worker

void BackendInfo::loadWorker(std::string _gssUrl) {

	// Get GSS info
	ot::GSSDebugInfo gssInfo;
	if (!this->gssLoad(_gssUrl, gssInfo)) {
		QMetaObject::invokeMethod(this, &BackendInfo::loadWorkerFinished, Qt::QueuedConnection);
		return;
	}

	m_gssInfos.push_back(gssInfo);
	QMetaObject::invokeMethod(this, &BackendInfo::slotAddGSS, Qt::QueuedConnection, gssInfo);

	// Get LSS info
	for (const auto& lss : gssInfo.getLSSList()) {
		ot::LSSDebugInfo lssInfo;
		if (this->lssLoad(lss.url, lssInfo)) {
			m_lssInfos.push_back(lssInfo);
			QMetaObject::invokeMethod(this, &BackendInfo::slotAddLSS, Qt::QueuedConnection, lssInfo);
		}
	}

	// Get GDS info
	ot::GDSDebugInfo gdsInfo;
	if (!this->gdsLoad(gssInfo.getGlobalDirectoryUrl(), gdsInfo)) {
		QMetaObject::invokeMethod(this, &BackendInfo::loadWorkerFinished, Qt::QueuedConnection);
		return;
	}

	m_gdsInfos.push_back(gdsInfo);
	QMetaObject::invokeMethod(this, &BackendInfo::slotAddGDS, Qt::QueuedConnection, gdsInfo);

	// Get LDS info
	for (const auto& lds : gdsInfo.getLocalDirectoryServices()) {
		ot::LDSDebugInfo ldsInfo;
		if (this->ldsLoad(lds.url, ldsInfo)) {
			m_ldsInfos.push_back(ldsInfo);
			QMetaObject::invokeMethod(this, &BackendInfo::slotAddLDS, Qt::QueuedConnection, ldsInfo);
		}
	}

	// Get service infos
	for (const auto& lss : m_lssInfos) {
		for (const auto& session : lss.getSessions()) {
			for (const auto& service : session.services) {
				if (service.isRunning) {
					serviceLoad(service.name, std::to_string(service.id), service.url);
				}
			}
		}
	}

	QMetaObject::invokeMethod(this, &BackendInfo::loadWorkerFinished, Qt::QueuedConnection);
}

bool BackendInfo::gssLoad(const std::string& _gssUrl, ot::GSSDebugInfo& _gssInfo) {
	BACKINFO_LOG("Gathering Global Session Service information from \"" + QString::fromStdString(_gssUrl) + "\"...");

	ot::JsonDocument gssDoc;
	gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetDebugInformation, gssDoc.GetAllocator()), gssDoc.GetAllocator());
	std::string gssResponse;
	if (!ot::msg::send("", _gssUrl, ot::EXECUTE, gssDoc.toJson(), gssResponse, ot::msg::defaultTimeout, ot::msg::NoRequestFlags)) {
		BACKINFO_LOGE("Failed to get GSS debug information from GSS at \"" + QString::fromStdString(_gssUrl) + "\"");
		return false;
	}

	if (gssResponse.empty()) {
		BACKINFO_LOGE("Received empty response from GSS at \"" + QString::fromStdString(_gssUrl) + "\"");
		return false;
	}

	ot::JsonDocument gssResponseDoc;
	if (!gssResponseDoc.fromJson(gssResponse)) {
		BACKINFO_LOGE("Failed to parse GSS response from GSS at \"" + QString::fromStdString(_gssUrl) + "\".");
		return false;
	}

	_gssInfo.setFromJsonObject(gssResponseDoc.getConstObject());
	return true;
}

bool BackendInfo::lssLoad(const std::string& _lssUrl, ot::LSSDebugInfo& _lssInfo) {
	BACKINFO_LOG("Gathering Local Session Service information from \"" + QString::fromStdString(_lssUrl) + "\"...");

	ot::JsonDocument lssDoc;
	lssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetDebugInformation, lssDoc.GetAllocator()), lssDoc.GetAllocator());
	std::string lssResponse;
	if (!ot::msg::send("", _lssUrl, ot::EXECUTE, lssDoc.toJson(), lssResponse, ot::msg::defaultTimeout, ot::msg::NoRequestFlags)) {
		BACKINFO_LOGE("Failed to get LSS debug information from LSS at \"" + QString::fromStdString(_lssUrl) + "\"");
		return false;
	}

	if (lssResponse.empty()) {
		BACKINFO_LOGE("Received empty response from LSS at \"" + QString::fromStdString(_lssUrl) + "\"");
		return false;
	}

	ot::JsonDocument lssResponseDoc;
	if (!lssResponseDoc.fromJson(lssResponse)) {
		BACKINFO_LOGE("Failed to parse LSS response from LSS at \"" + QString::fromStdString(_lssUrl) + "\".");
		return false;
	}

	_lssInfo.setFromJsonObject(lssResponseDoc.getConstObject());
	return true;
}

bool BackendInfo::gdsLoad(const std::string& _gdsUrl, ot::GDSDebugInfo& _gdsInfo) {
	BACKINFO_LOG("Gathering Global Directory Service information from \"" + QString::fromStdString(_gdsUrl) + "\"...");

	ot::JsonDocument gdsDoc;
	gdsDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetDebugInformation, gdsDoc.GetAllocator()), gdsDoc.GetAllocator());
	std::string gdsResponse;
	if (!ot::msg::send("", _gdsUrl, ot::EXECUTE, gdsDoc.toJson(), gdsResponse, ot::msg::defaultTimeout, ot::msg::NoRequestFlags)) {
		BACKINFO_LOGE("Failed to get GDS debug information from GDS at \"" + QString::fromStdString(_gdsUrl) + "\"");
		return false;
	}

	if (gdsResponse.empty()) {
		BACKINFO_LOGE("Received empty response from GDS at \"" + QString::fromStdString(_gdsUrl) + "\"");
		return false;
	}

	ot::JsonDocument gdsResponseDoc;
	if (!gdsResponseDoc.fromJson(gdsResponse)) {
		BACKINFO_LOGE("Failed to parse GDS response from GDS at \"" + QString::fromStdString(_gdsUrl) + "\".");
		return false;
	}

	_gdsInfo.setFromJsonObject(gdsResponseDoc.getConstObject());
	return true;
}

bool BackendInfo::ldsLoad(const std::string& _ldsUrl, ot::LDSDebugInfo& _ldsInfo) {
	BACKINFO_LOG("Gathering Local Directory Service information from \"" + QString::fromStdString(_ldsUrl) + "\"...");

	ot::JsonDocument ldsDoc;
	ldsDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetDebugInformation, ldsDoc.GetAllocator()), ldsDoc.GetAllocator());
	std::string ldsResponse;
	if (!ot::msg::send("", _ldsUrl, ot::EXECUTE, ldsDoc.toJson(), ldsResponse, ot::msg::defaultTimeout, ot::msg::NoRequestFlags)) {
		BACKINFO_LOGE("Failed to get LDS debug information from LDS at \"" + QString::fromStdString(_ldsUrl) + "\"");
		return false;
	}

	if (ldsResponse.empty()) {
		BACKINFO_LOGE("Received empty response from LDS at \"" + QString::fromStdString(_ldsUrl) + "\"");
		return false;
	}

	ot::JsonDocument ldsResponseDoc;
	if (!ldsResponseDoc.fromJson(ldsResponse)) {
		BACKINFO_LOGE("Failed to parse LDS response from LDS at \"" + QString::fromStdString(_ldsUrl) + "\".");
		return false;
	}

	_ldsInfo.setFromJsonObject(ldsResponseDoc.getConstObject());
	return true;
}

void BackendInfo::serviceLoad(const std::string& _serviceName, const std::string& _serviceId, const std::string& _serviceUrl) {
	BACKINFO_LOG("Gathering Service information { \"ServiceName\": \"" + QString::fromStdString(_serviceName) + "\", \"Url\": \"" + QString::fromStdString(_serviceUrl) + "\" }...");
	
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetDebugInformation, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_CallbackAction, ot::JsonString(c_addServiceDebugInfoAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(AppBase::instance()->url().toStdString(), doc.GetAllocator()), doc.GetAllocator());

	std::string responseStr;
	if (!ot::msg::send("", _serviceUrl, ot::EXECUTE, doc.toJson(), responseStr, ot::msg::defaultTimeout, ot::msg::DefaultFlags)) {
		BACKINFO_LOGE("Failed to get Service debug information from Service at \"" + QString::fromStdString(_serviceUrl) + "\"");
		return;
	}

	ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);
	if (!response.isOk()) {
		BACKINFO_LOGE("Service at \"" + QString::fromStdString(_serviceUrl) + "\" returned error: " + QString::fromStdString(response.getWhat()));
		return;
	}

	// Check JSON received, queue might have queued the message and returned an generic OK response
	if (response.getWhat().empty() || response.getWhat().at(0) != '{') {
		return;
	}
	
	QMetaObject::invokeMethod(this, &BackendInfo::slotAddService, Qt::QueuedConnection, _serviceName, _serviceId, _serviceUrl, response.getWhat());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Callbacks

void BackendInfo::handleAddServiceDebugInfo(ot::JsonDocument& _doc) {
	if (!m_sectionsLayout) {
		return;
	}

	std::string name = ot::json::getString(_doc, OT_ACTION_PARAM_SERVICE_NAME);
	std::string id = ot::json::getString(_doc, OT_ACTION_PARAM_SERVICE_ID);
	std::string url = ot::json::getString(_doc, OT_ACTION_PARAM_SERVICE_URL);
	std::string jsonData = ot::json::getString(_doc, OT_ACTION_PARAM_Data);

	if (jsonData.empty()) {
		BACKINFO_LOGE("Received empty debug information from Service { \"ServiceName\": \"" + QString::fromStdString(name) + "\", \"Url\": \"" + QString::fromStdString(url) + "\" }");
		return;
	}

	QMetaObject::invokeMethod(this, &BackendInfo::slotAddService, Qt::QueuedConnection, name, id, url, jsonData);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Widget helper

ot::Table* BackendInfo::createGssSessionTable(const std::list<ot::GSSDebugInfo::SessionData>& _sessionData, QWidget* _parent) {
	Qt::ItemFlags cellFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	ot::Table* table = new ot::Table(_sessionData.size(), 3, _parent);
	table->verticalHeader()->setVisible(false);
	table->setHorizontalHeaderLabels({ "Session ID", "User Name", "Flags" });
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	int r = 0;
	for (const auto& session : _sessionData) {
		table->addItem(r, 0, QString::fromStdString(session.sessionID))->setFlags(cellFlags);
		table->addItem(r, 1, QString::fromStdString(session.userName))->setFlags(cellFlags);
		std::string flagsStr;
		for (const std::string f : session.flags) {
			if (!flagsStr.empty()) {
				flagsStr.append("\n");
			}
			flagsStr.append(f);
		}
		table->addItem(r++, 2, QString::fromStdString(flagsStr))->setFlags(cellFlags);
	}

	return table;
}

ot::Table* BackendInfo::createLssServicesTable(const std::list<ot::LSSDebugInfo::ServiceInfo>& _services, QWidget* _parent) {
	QStringList headers = { "ID", "Name", "Type", "Url", "Websocket Url", "Debug", 
		"Requested", "Alive", "Running", "Shutting", "Hidden", "Connected Type" };

	ot::Table* table = new ot::Table(_services.size(), headers.count(), _parent);
	table->verticalHeader()->setVisible(false);
	table->setHorizontalHeaderLabels(headers);
	table->setSelectionBehavior(ot::Table::SelectRows);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	int r = 0;
	Qt::ItemFlags cellFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	for (const auto& service : _services) {
		int c = 0;
		table->addItem(r, c++, QString::number(service.id))->setFlags(cellFlags);
		table->addItem(r, c++, QString::fromStdString(service.name))->setFlags(cellFlags);
		table->addItem(r, c++, QString::fromStdString(service.type))->setFlags(cellFlags);
		table->addItem(r, c++, QString::fromStdString(service.url))->setFlags(cellFlags);
		table->addItem(r, c++, QString::fromStdString(service.websocketUrl))->setFlags(cellFlags);
		table->setCellWidget(r, c++, this->checkBox(service.isDebug, table));
		table->setCellWidget(r, c++, this->checkBox(service.isRequested, table));
		table->setCellWidget(r, c++, this->checkBox(service.isAlive, table));
		table->setCellWidget(r, c++, this->checkBox(service.isRunning, table));
		table->setCellWidget(r, c++, this->checkBox(service.isShuttingDown, table));
		table->setCellWidget(r, c++, this->checkBox(service.isHidden, table));
		table->addItem(r, c++, QString::fromStdString(service.connectedType))->setFlags(cellFlags);
		r++;
	}

	int defaultRowHeight = table->verticalHeader()->defaultSectionSize();
	r += 2;
	table->setMinimumHeight(r * defaultRowHeight);

	return table;
}

ot::Table* BackendInfo::createGdsServicesTable(const std::list<ot::GDSDebugInfo::ServiceInfo>& _services, QWidget* _parent) {
	QStringList headers = { "ID", "Name", "Type", "Session ID", "LSS Url" };
	ot::Table* table = new ot::Table(_services.size(), headers.count(), _parent);
	table->verticalHeader()->setVisible(false);
	table->setHorizontalHeaderLabels(headers);
	table->setSelectionBehavior(ot::Table::SelectRows);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	int r = 0;
	Qt::ItemFlags cellFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	for (const auto& service : _services) {
		int c = 0;
		table->addItem(r, c++, QString::number(service.serviceID))->setFlags(cellFlags);
		table->addItem(r, c++, QString::fromStdString(service.serviceName))->setFlags(cellFlags);
		table->addItem(r, c++, QString::fromStdString(service.serviceType))->setFlags(cellFlags);
		table->addItem(r, c++, QString::fromStdString(service.sessionID))->setFlags(cellFlags);
		table->addItem(r, c++, QString::fromStdString(service.lssUrl))->setFlags(cellFlags);
		r++;
	}

	int defaultRowHeight = table->verticalHeader()->defaultSectionSize();
	r += 2;
	table->setMinimumHeight(r * defaultRowHeight);
	return table;
}

int BackendInfo::createLDSServiceInfo(const ot::LDSDebugInfo::SessionInfo& _sessionInfo, const std::list<ot::LDSDebugInfo::ServiceInfo>& _info, LDSServiceInfoMode _mode, QLayout* _layout, QWidget* _parent) {
	bool additionalInfo = false;
	QStringList headers;
	switch (_mode) {
	case BackendInfo::LDSServiceInfoMode::InitializingServices:
	case BackendInfo::LDSServiceInfoMode::FailedServices:
	case BackendInfo::LDSServiceInfoMode::NewStoppingServices:
	case BackendInfo::LDSServiceInfoMode::StoppingServices:
	case BackendInfo::LDSServiceInfoMode::RequestedServices:
		headers = { "ID", "Name", "Type", "Session ID", "LSS Url", "Start Counter", "Initialize Attempt", "Max Crash Restarts", "Max Startup Restarts" };
		break;
	case BackendInfo::LDSServiceInfoMode::AliveServices:
		additionalInfo = true;
		headers = { "ID", "Name", "Type", "Url", "Websocket Url", "Is Shutdown", "LSS Url", "Start Counter", "Initialize Attempt", "Max Crash Restarts", "Max Startup Restarts" };
		break;
	default:
		BACKINFO_LOGE("Unknown mode (" + QString::number(static_cast<int>(_mode)) + ")");
		return 0;
	}

	QString expanderTitle;
	switch (_mode) {
	case BackendInfo::LDSServiceInfoMode::RequestedServices:
		expanderTitle = "Requested Services (" + QString::number(_info.size()) + ")";
		break;
	case BackendInfo::LDSServiceInfoMode::InitializingServices:
		expanderTitle = "Initializing Services (" + QString::number(_info.size()) + ")";
		break;
	case BackendInfo::LDSServiceInfoMode::AliveServices:
		expanderTitle = "Session { \"ID\": \"" + QString::fromStdString(_sessionInfo.sessionID) + "\", \"Service Count\": " + QString::number(_info.size()) + " }";
		break;
	case BackendInfo::LDSServiceInfoMode::FailedServices:
		expanderTitle = "Failed Services (" + QString::number(_info.size()) + ")";
		break;
	case BackendInfo::LDSServiceInfoMode::NewStoppingServices:
		expanderTitle = "New Stopping Services (" + QString::number(_info.size()) + ")";
		break;
	case BackendInfo::LDSServiceInfoMode::StoppingServices:
		expanderTitle = "Stopping Services (" + QString::number(_info.size()) + ")";
		break;
	default:
		BACKINFO_LOGE("Unknown mode (" + QString::number(static_cast<int>(_mode)) + ")");
		return 0;
	}

	ot::ExpanderWidget* expander = new ot::ExpanderWidget(expanderTitle, _parent);

	ot::Table* table = new ot::Table(_info.size(), headers.count(), expander);
	//table->verticalHeader()->setVisible(false);
	table->setHorizontalHeaderLabels(headers);
	table->setSelectionBehavior(ot::Table::SelectRows);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	int r = 0;
	Qt::ItemFlags cellFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	for (const auto& service : _info) {
		int c = 0;
		table->addItem(r, c++, QString::number(service.id))->setFlags(cellFlags);
		table->addItem(r, c++, QString::fromStdString(service.name))->setFlags(cellFlags);
		table->addItem(r, c++, QString::fromStdString(service.type))->setFlags(cellFlags);
		
		if (additionalInfo) {
			table->addItem(r, c++, QString::fromStdString(service.url))->setFlags(cellFlags);
			table->addItem(r, c++, QString::fromStdString(service.websocketUrl))->setFlags(cellFlags);
			table->setCellWidget(r, c++, this->checkBox(service.isShuttingDown, table));
		}
		else {
			table->addItem(r, c++, QString::fromStdString(service.sessionID))->setFlags(cellFlags);
		}
		
		table->addItem(r, c++, QString::fromStdString(service.lssUrl))->setFlags(cellFlags);
		table->addItem(r, c++, QString::number(service.startCounter))->setFlags(cellFlags);
		table->addItem(r, c++, QString::number(service.iniAttempt))->setFlags(cellFlags);
		table->addItem(r, c++, QString::number(service.maxCrashRestarts))->setFlags(cellFlags);
		table->addItem(r, c++, QString::number(service.maxStartupRestarts))->setFlags(cellFlags);
		r++;
	}

	int defaultRowHeight = table->verticalHeader()->defaultSectionSize() + 5;
	r += 2;
	int minHeight = r * defaultRowHeight;
	table->setMinimumHeight(minHeight);

	minHeight += 50;

	
	//expander->setMinExpandedHeight(minHeight);
	expander->setWidget(table);
	_layout->addWidget(expander);

	return minHeight + 50;
}

ot::LineEdit* BackendInfo::lineEdit(uint16_t _value, QWidget* _parent) {
	return this->lineEdit(QString::number(_value), _parent);
}

ot::LineEdit* BackendInfo::lineEdit(const std::string& _text, QWidget* _parent) {
	return this->lineEdit(QString::fromStdString(_text), _parent);
}

ot::LineEdit* BackendInfo::lineEdit(const QString& _text, QWidget* _parent) {
	ot::LineEdit* edit = new ot::LineEdit(_text, _parent);
	edit->setReadOnly(true);
	return edit;
}

ot::IndicatorWidget* BackendInfo::checkBox(bool _checked, QWidget* _parent) {
	ot::IndicatorWidget* check = new ot::IndicatorWidget(_checked, _parent);
	check->setEnabled(false);
	if (_checked) {
		check->setSuccessForeground();
	}
	else {
		check->setErrorForeground();
	}
	return check;
}
