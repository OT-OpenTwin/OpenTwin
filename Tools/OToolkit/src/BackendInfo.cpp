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
#include "OTWidgets/PushButton.h"
#include "OTWidgets/ExpanderWidget.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qheaderview.h>

#define BACKINFO_LOG(___message) OTOOLKIT_LOG("Backend Info", ___message)
#define BACKINFO_LOGW(___message) OTOOLKIT_LOGW("Backend Info", ___message)
#define BACKINFO_LOGE(___message) OTOOLKIT_LOGE("Backend Info", ___message)

BackendInfo::BackendInfo() 
	: m_sectionsLayout(nullptr), m_loadButton(nullptr), m_cancelButton(nullptr), 
	m_clearButton(nullptr), m_gssUrl(nullptr), m_loadThread(nullptr)
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

	m_sectionsLayout = new QVBoxLayout;
	scrollArea->setLayout(m_sectionsLayout);

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
	QScrollArea* mainArea = new QScrollArea;
	mainArea->setWidgetResizable(true);
	mainArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	mainArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

	QWidget* mainWidget = new QWidget;
	QHBoxLayout* mainLayout = new QHBoxLayout(mainWidget);

	QGridLayout* generalInfoLayout = new QGridLayout;
	int r = 0;
	generalInfoLayout->addWidget(new ot::Label("Url:"), r, 0);
	ot::LineEdit* edit = new ot::LineEdit(QString::fromStdString(_info.getUrl()));
	edit->setReadOnly(true);
	generalInfoLayout->addWidget(edit, r++, 1);

	generalInfoLayout->addWidget(new ot::Label("DB Url:"), r, 0);
	edit = new ot::LineEdit(QString::fromStdString(_info.getDatabaseUrl()));
	edit->setReadOnly(true);
	generalInfoLayout->addWidget(edit, r++, 1);

	generalInfoLayout->addWidget(new ot::Label("Auth Url:"), r, 0);
	edit = new ot::LineEdit(QString::fromStdString(_info.getAuthorizationUrl()));
	edit->setReadOnly(true);
	generalInfoLayout->addWidget(edit, r++, 1);

	generalInfoLayout->addWidget(new ot::Label("GDS Url:"), r, 0);
	edit = new ot::LineEdit(QString::fromStdString(_info.getGlobalDirectoryUrl()));
	edit->setReadOnly(true);
	generalInfoLayout->addWidget(edit, r++, 1);

	generalInfoLayout->addWidget(new ot::Label("LMS Url:"), r, 0);
	edit = new ot::LineEdit(QString::fromStdString(_info.getLibraryManagementUrl()));
	edit->setReadOnly(true);
	generalInfoLayout->addWidget(edit, r++, 1);

	generalInfoLayout->setRowStretch(r, 1);
	mainLayout->addLayout(generalInfoLayout);

	// Sessions
	Qt::ItemFlags cellFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	QVBoxLayout* sessionLayout = new QVBoxLayout;
	sessionLayout->addWidget(new ot::Label("Session To LSS:"));

	ot::Table* sessionTable = new ot::Table(_info.getSessionToLssList().size(), 2);
	sessionTable->verticalHeader()->setVisible(false);
	sessionTable->setHorizontalHeaderLabels({ "Session", "LSS ID" });
	r = 0;
	for (const auto& it : _info.getSessionToLssList()) {
		sessionTable->addItem(r, 0, QString::fromStdString(it.first))->setFlags(cellFlags);
		sessionTable->addItem(r++, 1, QString::number(it.second))->setFlags(cellFlags);
	}

	sessionLayout->addWidget(sessionTable);
	mainLayout->addLayout(sessionLayout);

	QScrollArea* lssArea = new QScrollArea;
	lssArea->setWidgetResizable(true);
	lssArea->setContentsMargins(0, 0, 0, 0);
	lssArea->setMinimumSize(400, 500);
	lssArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	lssArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

	QVBoxLayout* lssMainLayout = new QVBoxLayout;
	lssMainLayout->addWidget(new ot::Label("LSS Data:"));

	for (const auto& it : _info.getLSSList()) {
		QScrollArea* lssWidget = new QScrollArea;
		lssWidget->setWidgetResizable(true);
		lssWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
		lssWidget->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);

		QWidget* lssContent = new QWidget;
		lssWidget->setWidget(lssContent);

		QVBoxLayout* lssLayout = new QVBoxLayout(lssContent);

		QGridLayout* lssInfoLayout = new QGridLayout;
		r = 0;
		lssInfoLayout->addWidget(new ot::Label("ID:"), r, 0);
		edit = new ot::LineEdit(QString::number(it.id));
		edit->setReadOnly(true);
		lssInfoLayout->addWidget(edit, r++, 1);

		lssInfoLayout->addWidget(new ot::Label("Url:"), r, 0);
		edit = new ot::LineEdit(QString::fromStdString(it.url));
		edit->setReadOnly(true);
		lssInfoLayout->addWidget(edit, r++, 1);
		lssLayout->addLayout(lssInfoLayout);

		// Active sessions
		lssLayout->addWidget(new ot::Label("Active Sessions:"));
		lssLayout->addWidget(this->createGssSessionTable(it.activeSessions));

		// Initializing sessions
		lssLayout->addWidget(new ot::Label("Initializing Sessions:"));
		lssLayout->addWidget(this->createGssSessionTable(it.initializingSessions));
	
		ot::ExpanderWidget* expander = new ot::ExpanderWidget(QString::fromStdString("LSS { \"ID\": " + std::to_string(it.id) + ", \"URL\": \"" + it.url + "\" }"));
		expander->setMinExpandedHeight(450);
		expander->setWidget(lssWidget);
		lssMainLayout->addWidget(expander);
	}

	lssMainLayout->addStretch(1);

	lssArea->setLayout(lssMainLayout);
	mainLayout->addWidget(lssArea);

	mainArea->setWidget(mainWidget);

	ot::ExpanderWidget* mainExpander = new ot::ExpanderWidget("GSS { \"URL\": \"" + QString::fromStdString(_info.getUrl()) + "\" }");
	mainExpander->setMinExpandedHeight(500);
	mainExpander->setWidget(mainArea);

	m_sectionsLayout->addWidget(mainExpander);
	m_sections.push_back(mainExpander);
}

void BackendInfo::slotAddLSS(const ot::LSSDebugInfo& _info) {

}

void BackendInfo::slotAddGDS(const ot::GDSDebugInfo& _info) {

}

void BackendInfo::slotAddLDS(const ot::LDSDebugInfo& _info) {

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
	if (!this->gssLoad(_gssUrl)) {
		QMetaObject::invokeMethod(this, &BackendInfo::loadWorkerFinished, Qt::QueuedConnection);
		return;
	}



	QMetaObject::invokeMethod(this, &BackendInfo::loadWorkerFinished, Qt::QueuedConnection);
}

bool BackendInfo::gssLoad(const std::string& _gssUrl) {
	BACKINFO_LOG("Gathering Global Session Service information...");

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

	ot::GSSDebugInfo gssInfo;
	gssInfo.setFromJsonObject(gssResponseDoc.getConstObject());
	
	m_gssInfos.push_back(gssInfo);

	QMetaObject::invokeMethod(this, "slotAddGSS", Qt::QueuedConnection, Q_ARG(const ot::GSSDebugInfo&, gssInfo));
	return true;
}

ot::Table* BackendInfo::createGssSessionTable(const std::list<ot::GSSDebugInfo::SessionData>& _sessionData) {
	Qt::ItemFlags cellFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	ot::Table* table = new ot::Table(_sessionData.size(), 3);
	table->verticalHeader()->setVisible(false);
	table->setHorizontalHeaderLabels({ "Session ID", "User Name", "Flags" });
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
