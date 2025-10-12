//! @file BackendInfo.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
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
#include "OTWidgets/IndicatorWidget.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qheaderview.h>
#include <QtWidgets/qlistwidget.h>

#define BACKINFO_LOG(___message) OTOOLKIT_LOG("Backend Info", ___message)
#define BACKINFO_LOGW(___message) OTOOLKIT_LOGW("Backend Info", ___message)
#define BACKINFO_LOGE(___message) OTOOLKIT_LOGE("Backend Info", ___message)

BackendInfo::BackendInfo() 
	: m_sectionsLayout(nullptr), m_loadButton(nullptr), m_cancelButton(nullptr),
	m_clearButton(nullptr), m_gssUrl(nullptr), m_loadThread(nullptr), m_stretchWidget(nullptr)
{

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
	m_gssUrl = new ot::LineEdit;
	m_loadButton = new ot::PushButton("Load");
	m_cancelButton = new ot::PushButton("Cancel");
	m_clearButton = new ot::PushButton("Clear");

	QHBoxLayout* controlLayout = new QHBoxLayout;
	controlLayout->addWidget(new ot::Label("GSS URL:"));
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
	
	m_sectionsLayout = new QVBoxLayout(scrollContent);
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

	if (m_stretchWidget) {
		m_sectionsLayout->removeWidget(m_stretchWidget);
		delete m_stretchWidget;
		m_stretchWidget = nullptr;
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
	QSplitter* mainSplitter = new QSplitter;
	mainSplitter->setMinimumHeight(400);

	QGroupBox* generalInfoGroup = new QGroupBox("General Info");
	mainSplitter->addWidget(generalInfoGroup);

	QGridLayout* generalInfoLayout = new QGridLayout(generalInfoGroup);
	int r = 0;
	generalInfoLayout->addWidget(new ot::Label("Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getUrl()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("DB Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getDatabaseUrl()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Auth Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getAuthorizationUrl()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("GDS Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getGlobalDirectoryUrl()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("LMS Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getLibraryManagementUrl()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Worker Running:"), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.isWorkerRunning()), r++, 1);

	generalInfoLayout->setRowStretch(r, 1);

	// Sessions
	Qt::ItemFlags cellFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	QGroupBox* sessionGroup = new QGroupBox("Sessions");
	mainSplitter->addWidget(sessionGroup);

	QVBoxLayout* sessionLayout = new QVBoxLayout(sessionGroup);
	sessionLayout->setContentsMargins(0, 10, 0, 0);

	ot::Table* sessionTable = new ot::Table(_info.getSessionToLssList().size(), 2);
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

	QGroupBox* lssInfoGroup = new QGroupBox("LSS Info");
	mainSplitter->addWidget(lssInfoGroup);

	QVBoxLayout* lssInfoGroupLayout = new QVBoxLayout(lssInfoGroup);
	lssInfoGroupLayout->setContentsMargins(0, 10, 0, 0);

	QScrollArea* lssArea = new QScrollArea;
	lssArea->setWidgetResizable(true);
	lssArea->setContentsMargins(0, 0, 0, 0);
	lssArea->setMinimumSize(400, 500);
	lssArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	lssArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	lssInfoGroupLayout->addWidget(lssArea);

	QWidget* lssMainLayoutWidget = new QWidget;
	QVBoxLayout* lssMainLayout = new QVBoxLayout(lssMainLayoutWidget);
	lssMainLayout->setContentsMargins(0, 0, 0, 0);
	lssArea->setWidget(lssMainLayoutWidget);

	for (const auto& it : _info.getLSSList()) {
		QScrollArea* lssWidget = new QScrollArea;
		lssWidget->setWidgetResizable(true);
		lssWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
		lssWidget->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
		lssWidget->setMinimumHeight(200);

		QWidget* lssContent = new QWidget;
		lssWidget->setWidget(lssContent);

		QVBoxLayout* lssLayout = new QVBoxLayout(lssContent);

		QGridLayout* lssInfoLayout = new QGridLayout;
		r = 0;
		lssInfoLayout->addWidget(new ot::Label("ID:"), r, 0);
		lssInfoLayout->addWidget(lineEdit(it.id), r++, 1);

		lssInfoLayout->addWidget(new ot::Label("Url:"), r, 0);
		lssInfoLayout->addWidget(lineEdit(it.url), r++, 1);

		lssLayout->addLayout(lssInfoLayout);

		// Active sessions
		lssLayout->addWidget(new ot::Label("Active Sessions:"));
		lssLayout->addWidget(this->createGssSessionTable(it.activeSessions));

		// Initializing sessions
		lssLayout->addWidget(new ot::Label("Initializing Sessions:"));
		lssLayout->addWidget(this->createGssSessionTable(it.initializingSessions));
	
		ot::ExpanderWidget* expander = new ot::ExpanderWidget(QString::fromStdString("LSS { \"ID\": " + std::to_string(it.id) + ", \"URL\": \"" + it.url + "\" }"));
		//expander->setMinExpandedHeight(450);
		expander->setWidget(lssWidget);
		lssMainLayout->addWidget(expander);
	}

	lssMainLayout->addStretch(1);

	ot::ExpanderWidget* mainExpander = new ot::ExpanderWidget("GSS { \"URL\": \"" + QString::fromStdString(_info.getUrl()) + "\" }");
	//mainExpander->setMinExpandedHeight(500);
	mainExpander->setWidget(mainSplitter);

	m_sectionsLayout->addWidget(mainExpander);
	m_sections.push_back(mainExpander);
}

void BackendInfo::slotAddLSS(const ot::LSSDebugInfo& _info) {
	QSplitter* mainSplitter = new QSplitter;
	mainSplitter->setMinimumHeight(400);

	// General Info

	QGroupBox* generalInfoGroup = new QGroupBox("General Info");
	mainSplitter->addWidget(generalInfoGroup);

	QVBoxLayout* generalInfoGroupLayout = new QVBoxLayout(generalInfoGroup);

	QScrollArea* infoArea = new QScrollArea;
	infoArea->setWidgetResizable(true);
	infoArea->setContentsMargins(0, 0, 0, 0);
	infoArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	infoArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	generalInfoGroupLayout->addWidget(infoArea);

	QWidget* infoAreaWidget = new QWidget;
	infoArea->setWidget(infoAreaWidget);
	QVBoxLayout* infoAreaLayout = new QVBoxLayout(infoAreaWidget);

	QWidget* generalInfoLayoutWidget = new QWidget;
	QGridLayout* generalInfoLayout = new QGridLayout(generalInfoLayoutWidget);

	int r = 0;
	generalInfoLayout->addWidget(new ot::Label("Worker Running:"), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getWorkerRunning()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getURL()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("ID:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getID()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("GSS Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getGSSUrl()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("GSS Connected:"), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getGSSConnected()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("GDS Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getGDSUrl()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("GDS Connected:"), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getGDSConnected()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Auth Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getAuthUrl()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Database Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getDataBaseUrl()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("LMS Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getLMSUrl()), r++, 1);

	infoAreaLayout->addWidget(generalInfoLayoutWidget);

	QWidget* mandatoryServicesWidget = new QWidget;
	QVBoxLayout* mandatoryServicesLayout = new QVBoxLayout(mandatoryServicesWidget);
	
	for (const auto& it : _info.getMandatoryServices()) {
		QListWidget* mandatoryWidget = new QListWidget;
		for (const auto& service : it.second) {
			QListWidgetItem* itm = new QListWidgetItem(QString::fromStdString(service));
			itm->setFlags(Qt::ItemIsEnabled);
			mandatoryWidget->addItem(itm);
		}

		ot::ExpanderWidget* expander = new ot::ExpanderWidget(QString::fromStdString("Madatory Services (" + it.first + ")"));
		expander->setWidget(mandatoryWidget);
		mandatoryServicesLayout->addWidget(expander);
	}

	infoAreaLayout->addWidget(mandatoryServicesWidget);

	infoAreaLayout->addStretch(1);

	// Additional Info

	QGroupBox* additionalInfoGroup = new QGroupBox("Additional Info");
	mainSplitter->addWidget(additionalInfoGroup);

	QVBoxLayout* additionalInfoGroupLayout = new QVBoxLayout(additionalInfoGroup);

	QScrollArea* additionalInfoArea = new QScrollArea;
	additionalInfoArea->setWidgetResizable(true);
	additionalInfoArea->setContentsMargins(0, 0, 0, 0);
	additionalInfoArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	additionalInfoArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	additionalInfoGroupLayout->addWidget(additionalInfoArea);

	QWidget* additionalInfoLayoutWidget = new QWidget;
	additionalInfoArea->setWidget(additionalInfoLayoutWidget);

	QVBoxLayout* additionalInfoLayout = new QVBoxLayout(additionalInfoLayoutWidget);

	QListWidget* debugServicesList = new QListWidget;
	for (const auto& it : _info.getDebugServices()) {
		QListWidgetItem* itm = new QListWidgetItem(QString::fromStdString(it));
		itm->setFlags(Qt::ItemIsEnabled);
		debugServicesList->addItem(itm);
	}
	ot::ExpanderWidget* debugServicesExpander = new ot::ExpanderWidget("Debug Services (" + QString::number(_info.getDebugServices().size()) + ")");
	debugServicesExpander->setWidget(debugServicesList);
	additionalInfoLayout->addWidget(debugServicesExpander);

	ot::TreeWidget* shutdownQueueTree = new ot::TreeWidget;
	shutdownQueueTree->setColumnCount(2);
	shutdownQueueTree->setHeaderLabels({ "SessionID", "Emergency" });
	for (const auto& it : _info.getShutdownQueue()) {
		ot::TreeWidgetItem* item = new ot::TreeWidgetItem;
		item->setText(0, QString::fromStdString(it.first));
		item->setText(1, it.second ? "true" : "");
		shutdownQueueTree->addTopLevelItem(item);
	}
	ot::ExpanderWidget* shutdownQueueExpander = new ot::ExpanderWidget("Shutdown Queue (" + QString::number(_info.getShutdownQueue().size()) + ")");
	shutdownQueueExpander->setWidget(shutdownQueueTree);
	additionalInfoLayout->addWidget(shutdownQueueExpander);

	QListWidget* shutdownCompletedList = new QListWidget;
	for (const auto& it : _info.getShutdownCompletedQueue()) {
		QListWidgetItem* itm = new QListWidgetItem(QString::fromStdString(it));
		itm->setFlags(Qt::ItemIsEnabled);
		shutdownCompletedList->addItem(itm);
	}
	ot::ExpanderWidget* shutdownCompletedExpander = new ot::ExpanderWidget("Shutdown Completed (" + QString::number(_info.getShutdownCompletedQueue().size()) + ")");
	shutdownCompletedExpander->setWidget(shutdownCompletedList);
	additionalInfoLayout->addWidget(shutdownCompletedExpander);

	QListWidget* blockedPortsList = new QListWidget;
	for (const auto& it : _info.getBlockedPorts()) {
		QListWidgetItem* itm = new QListWidgetItem(QString::number(it));
		itm->setFlags(Qt::ItemIsEnabled);
		blockedPortsList->addItem(itm);
	}
	ot::ExpanderWidget* blockedPortsExpander = new ot::ExpanderWidget("Blocked Ports (" + QString::number(_info.getBlockedPorts().size()) + ")");
	blockedPortsExpander->setWidget(blockedPortsList);
	additionalInfoLayout->addWidget(blockedPortsExpander);

	additionalInfoLayout->addStretch(1);

	// Sessions

	QGroupBox* sessionsGroup = new QGroupBox("Sessions");
	mainSplitter->addWidget(sessionsGroup);

	QVBoxLayout* sessionsGroupLayout = new QVBoxLayout(sessionsGroup);

	QScrollArea* sessionsArea = new QScrollArea;
	sessionsArea->setWidgetResizable(true);
	sessionsArea->setContentsMargins(0, 0, 0, 0);
	sessionsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	sessionsArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	sessionsGroupLayout->addWidget(sessionsArea);

	QWidget* sessionsAreaWidget = new QWidget;
	sessionsArea->setWidget(sessionsAreaWidget);

	QVBoxLayout* sessionsAreaLayout = new QVBoxLayout(sessionsAreaWidget);

	for (const auto& it : _info.getSessions()) {
		QScrollArea* sessionArea = new QScrollArea;
		sessionArea->setWidgetResizable(true);

		QWidget* sessionContent = new QWidget;
		sessionArea->setWidget(sessionContent);

		QVBoxLayout* sessionLayout = new QVBoxLayout(sessionContent);
		
		QGridLayout* sessionInfoLayout = new QGridLayout;
		r = 0;
		sessionInfoLayout->addWidget(new ot::Label("ID:"), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.id), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Type:"), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.type), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("User Name:"), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.userName), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Project Name:"), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.projectName), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Collection Name:"), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.collectionName), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("User Collection:"), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.userCollection), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("User Credentials Name:"), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.userCredName), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("User Credentials Password:"), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.userCredPassword), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Database Credentials Name:"), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.dataBaseCredName), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Database Credentials Password:"), r, 0);
		sessionInfoLayout->addWidget(lineEdit(it.dataBaseCredPassword), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Health Check Running:"), r, 0);
		sessionInfoLayout->addWidget(checkBox(it.isHealthCheckRunning), r++, 1);

		sessionInfoLayout->addWidget(new ot::Label("Shutting Down:"), r, 0);
		sessionInfoLayout->addWidget(checkBox(it.isShuttingDown), r++, 1);

		sessionLayout->addLayout(sessionInfoLayout);

		// Services

		sessionLayout->addWidget(new ot::Label("Services:"));

		sessionLayout->addWidget(this->createLssServicesTable(it.services));

		ot::ExpanderWidget* expander = new ot::ExpanderWidget(QString::fromStdString("Session { \"ID\": \"" + it.id + "\" }"));
		//expander->setMinExpandedHeight(800);
		expander->setWidget(sessionArea);
		sessionsAreaLayout->addWidget(expander);
	}

	ot::ExpanderWidget* mainExpander = new ot::ExpanderWidget("LSS { \"ID\": " + QString::number(_info.getID()) + ", \"URL\": \"" + QString::fromStdString(_info.getURL()) + "\" }");
	//mainExpander->setMinExpandedHeight(500);
	mainExpander->setWidget(mainSplitter);

	m_sectionsLayout->addWidget(mainExpander);
	m_sections.push_back(mainExpander);
}

void BackendInfo::slotAddGDS(const ot::GDSDebugInfo& _info) {
	QSplitter* mainSplitter = new QSplitter;
	mainSplitter->setMinimumHeight(400);

	QGroupBox* generalInfoGroup = new QGroupBox("General Info");
	mainSplitter->addWidget(generalInfoGroup);

	QGridLayout* generalInfoLayout = new QGridLayout(generalInfoGroup);
	int r = 0;
	generalInfoLayout->addWidget(new ot::Label("Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getURL()), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("GSS Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getGSSUrl()), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("Startup Worker Running:"), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getStartupWorkerRunning()), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("Startup Worker Stopping:"), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getStartupWorkerStopping()), r++, 1);
	generalInfoLayout->setRowStretch(r, 1);

	// Requested Services
	QGroupBox* requestedServicesGroup = new QGroupBox("Requested Services");
	mainSplitter->addWidget(requestedServicesGroup);

	QVBoxLayout* requestedServicesLayout = new QVBoxLayout(requestedServicesGroup);
	requestedServicesLayout->addWidget(this->createGdsServicesTable(_info.getRequestedServices()));

	// Local Directory Services Info
	QGroupBox* ldsInfoGroup = new QGroupBox("LDS Info");
	mainSplitter->addWidget(ldsInfoGroup);

	QVBoxLayout* ldsInfoGroupLayout = new QVBoxLayout(ldsInfoGroup);
	ldsInfoGroupLayout->setContentsMargins(0, 10, 0, 0);

	QScrollArea* ldsArea = new QScrollArea;
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
		
		ldsLayout->addWidget(new ot::Label("Supported Services:"));
		QListWidget* supportedServicesList = new QListWidget;
		for (const auto& service : it.supportedServices) {
			QListWidgetItem* itm = new QListWidgetItem(QString::fromStdString(service));
			itm->setFlags(Qt::ItemIsEnabled);
			supportedServicesList->addItem(itm);
		}
		ldsLayout->addWidget(supportedServicesList);

		ldsLayout->addWidget(new ot::Label("Services:"));
		ldsLayout->addWidget(this->createGdsServicesTable(it.services), 1);

		ot::ExpanderWidget* expander = new ot::ExpanderWidget(QString::fromStdString("LDS { \"ID\": " + std::to_string(it.serviceID) + ", \"URL\": \"" + it.url + "\" }"));
		//expander->setMinExpandedHeight(450);
		expander->setWidget(ldsWidget);
		ldsMainLayout->addWidget(expander);
	}
	ldsMainLayout->addStretch(1);
	
	ot::ExpanderWidget* mainExpander = new ot::ExpanderWidget("GDS { \"URL\": \"" + QString::fromStdString(_info.getURL()) + "\" }");
	//mainExpander->setMinExpandedHeight(500);
	mainExpander->setWidget(mainSplitter);

	m_sectionsLayout->addWidget(mainExpander);
	m_sections.push_back(mainExpander);
}

void BackendInfo::slotAddLDS(const ot::LDSDebugInfo& _info) {
	QSplitter* mainSplitter = new QSplitter;
	mainSplitter->setMinimumHeight(400);

	// General Info

	QGroupBox* generalInfoGroup = new QGroupBox("General Info");
	mainSplitter->addWidget(generalInfoGroup);

	QGridLayout* generalInfoLayout = new QGridLayout(generalInfoGroup);
	int r = 0;

	generalInfoLayout->addWidget(new ot::Label("Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getURL()), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("ID:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getId()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("GDS Url:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getGDSURL()), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("GDS Connected:"), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getGDSConnected()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Services IP:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getServicesIPAddress()), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("Last Error:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(_info.getLastError()), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Worker Running:"), r, 0);
	generalInfoLayout->addWidget(checkBox(_info.getWorkerRunning()), r++, 1);
	generalInfoLayout->addWidget(new ot::Label("Check Alive Frequency:"), r, 0);
	generalInfoLayout->addWidget(lineEdit(QString::number(_info.getServiceCheckAliveFrequency()) + "s"), r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Used Ports:"), r, 0);
	QListWidget* usedPortsList = new QListWidget;
	for (const auto& it : _info.getUsedPorts()) {
		QListWidgetItem* itm = new QListWidgetItem(QString::number(it));
		itm->setFlags(Qt::ItemIsEnabled);
		usedPortsList->addItem(itm);
	}
	generalInfoLayout->addWidget(usedPortsList, r++, 1);
	generalInfoLayout->setRowStretch(r, 1);

	// Config
	QGroupBox* configGroup = new QGroupBox("Config");
	mainSplitter->addWidget(configGroup);

	QVBoxLayout* configGroupLayout = new QVBoxLayout(configGroup);
	QScrollArea* configArea = new QScrollArea;
	configArea->setWidgetResizable(true);
	configArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	configArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	configGroupLayout->addWidget(configArea);

	QWidget* configContent = new QWidget;
	configArea->setWidget(configContent);
	QVBoxLayout* configLayout = new QVBoxLayout(configContent);

	QGridLayout* configInfoLayout = new QGridLayout;
	r = 0;
	configInfoLayout->addWidget(new ot::Label("Config Imported:"), r, 0);
	configInfoLayout->addWidget(checkBox(_info.getConfig().configImported), r++, 1);
	configInfoLayout->addWidget(new ot::Label("Launcher Path:"), r, 0);
	configInfoLayout->addWidget(lineEdit(_info.getConfig().launcherPath), r++, 1);
	configInfoLayout->addWidget(new ot::Label("Services Path:"), r, 0);
	configInfoLayout->addWidget(lineEdit(_info.getConfig().servicesLibraryPath), r++, 1);
	configInfoLayout->addWidget(new ot::Label("Default Startup Restarts:"), r, 0);
	configInfoLayout->addWidget(lineEdit(QString::number(_info.getConfig().defaultMaxStartupRestarts)), r++, 1);
	configInfoLayout->addWidget(new ot::Label("Default Crash Restarts:"), r, 0);
	configInfoLayout->addWidget(lineEdit(QString::number(_info.getConfig().defaultMaxCrashRestarts)), r++, 1);
	configLayout->addLayout(configInfoLayout);

	configLayout->addWidget(new ot::Label("Supported Services:"));
	QStringList supportedServicesHeader = { "Name", "Type", "Crash Restarts", "Startup Restarts" };
	ot::Table* supportedServicesTable = new ot::Table(_info.getConfig().supportedServices.size(), supportedServicesHeader.size());
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
	QGroupBox* servicesGroup = new QGroupBox("Service Manager");
	mainSplitter->addWidget(servicesGroup);

	QVBoxLayout* servicesGroupLayout = new QVBoxLayout(servicesGroup);
	QScrollArea* servicesArea = new QScrollArea;
	servicesArea->setWidgetResizable(true);
	servicesArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	servicesArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	servicesGroupLayout->addWidget(servicesArea);

	QWidget* servicesContent = new QWidget;
	servicesArea->setWidget(servicesContent);

	QVBoxLayout* servicesLayout = new QVBoxLayout(servicesContent);
	
	ot::LDSDebugInfo::SessionInfo dummySession;
	createLDSServiceInfo(dummySession, _info.getRequestedServices(), LDSServiceInfoMode::RequestedServices, servicesLayout);
	createLDSServiceInfo(dummySession, _info.getInitializingServices(), LDSServiceInfoMode::InitializingServices, servicesLayout);

	QWidget* aliveSessionsWidget = new QWidget;
	QVBoxLayout* aliveSessionsLayout = new QVBoxLayout(aliveSessionsWidget);
	int minHeight = 0;
	for (const auto& it : _info.getAliveSessions()) {
		minHeight += createLDSServiceInfo(it.first, it.second, LDSServiceInfoMode::AliveServices, aliveSessionsLayout);
	}
	ot::ExpanderWidget* aliveSessionsExpander = new ot::ExpanderWidget("Alive Sessions (" + QString::number(_info.getAliveSessions().size()) + ")");
	aliveSessionsExpander->setWidget(aliveSessionsWidget);
	//aliveSessionsExpander->setMinExpandedHeight(minHeight + 100);
	servicesLayout->addWidget(aliveSessionsExpander);

	createLDSServiceInfo(dummySession, _info.getFailedServices(), LDSServiceInfoMode::FailedServices, servicesLayout);
	createLDSServiceInfo(dummySession, _info.getNewStoppingServices(), LDSServiceInfoMode::NewStoppingServices, servicesLayout);
	createLDSServiceInfo(dummySession, _info.getStoppingServices(), LDSServiceInfoMode::StoppingServices, servicesLayout);

	servicesLayout->addStretch(1);
	
	ot::ExpanderWidget* mainExpander = new ot::ExpanderWidget("LDS { \"ID\": " + QString::number(_info.getId()) + ", \"URL\": \"" + QString::fromStdString(_info.getURL()) + "\" }");
	//mainExpander->setMinExpandedHeight(500);
	mainExpander->setWidget(mainSplitter);
	
	m_sectionsLayout->addWidget(mainExpander);
	m_sections.push_back(mainExpander);
	
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

	if (!m_stretchWidget) {
		m_stretchWidget = new QWidget;
		m_sectionsLayout->addWidget(m_stretchWidget, 1);
	}

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

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Widget helper

ot::Table* BackendInfo::createGssSessionTable(const std::list<ot::GSSDebugInfo::SessionData>& _sessionData) {
	Qt::ItemFlags cellFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	ot::Table* table = new ot::Table(_sessionData.size(), 3);
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

ot::Table* BackendInfo::createLssServicesTable(const std::list<ot::LSSDebugInfo::ServiceInfo>& _services) {
	QStringList headers = { "ID", "Name", "Type", "Url", "Websocket Url", "Debug", 
		"Requested", "Alive", "Running", "Shutting", "Hidden", "Connected Type" };

	ot::Table* table = new ot::Table(_services.size(), headers.count());
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
		table->setCellWidget(r, c++, this->checkBox(service.isDebug));
		table->setCellWidget(r, c++, this->checkBox(service.isRequested));
		table->setCellWidget(r, c++, this->checkBox(service.isAlive));
		table->setCellWidget(r, c++, this->checkBox(service.isRunning));
		table->setCellWidget(r, c++, this->checkBox(service.isShuttingDown));
		table->setCellWidget(r, c++, this->checkBox(service.isHidden));
		table->addItem(r, c++, QString::fromStdString(service.connectedType))->setFlags(cellFlags);
		r++;
	}

	int defaultRowHeight = table->verticalHeader()->defaultSectionSize();
	r += 2;
	table->setMinimumHeight(r * defaultRowHeight);

	return table;
}

ot::Table* BackendInfo::createGdsServicesTable(const std::list<ot::GDSDebugInfo::ServiceInfo>& _services) {
	QStringList headers = { "ID", "Name", "Type", "Session ID", "LSS Url" };
	ot::Table* table = new ot::Table(_services.size(), headers.count());
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

int BackendInfo::createLDSServiceInfo(const ot::LDSDebugInfo::SessionInfo& _sessionInfo, const std::list<ot::LDSDebugInfo::ServiceInfo>& _info, LDSServiceInfoMode _mode, QLayout* _layout) {
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

	ot::Table* table = new ot::Table(_info.size(), headers.count());
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
			table->setCellWidget(r, c++, this->checkBox(service.isShuttingDown));
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

	ot::ExpanderWidget* expander = new ot::ExpanderWidget(expanderTitle);
	//expander->setMinExpandedHeight(minHeight);
	expander->setWidget(table);
	_layout->addWidget(expander);

	return minHeight + 50;
}

ot::LineEdit* BackendInfo::lineEdit(uint16_t _value) {
	return this->lineEdit(QString::number(_value));
}

ot::LineEdit* BackendInfo::lineEdit(const std::string& _text) {
	return this->lineEdit(QString::fromStdString(_text));
}

ot::LineEdit* BackendInfo::lineEdit(const QString& _text) {
	ot::LineEdit* edit = new ot::LineEdit(_text);
	edit->setReadOnly(true);
	return edit;
}

ot::IndicatorWidget* BackendInfo::checkBox(bool _checked) {
	ot::IndicatorWidget* check = new ot::IndicatorWidget(_checked);
	check->setEnabled(false);
	if (_checked) {
		check->setSuccessForeground();
	}
	else {
		check->setErrorForeground();
	}
	return check;
}
