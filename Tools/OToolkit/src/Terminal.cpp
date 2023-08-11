#include "Terminal.h"
#include "AppBase.h"
#include "JSONEditor.h"
#include "StatusBar.h"

// OT header
#include "OpenTwinCommunication/Msg.h"
#include "OpenTwinCore/otAssert.h"

// Qt header
#include <QtCore/qjsondocument.h>
#include <QtCore/qsettings.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsondocument.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qplaintextedit.h>
#include <QtWidgets/qprogressbar.h>
#include <QtWidgets/qtreewidget.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qfiledialog.h>

// Std header
#include <thread>

#define OT_JSON_COLLECTION_Version "Version"
#define OT_JSON_COLLECTION_Data "Data"
#define OT_JSON_COLLECTION_ItemType "ItemType"
#define OT_JSON_COLLECTION_ItemName "ItemName"
#define OT_JSON_COLLECTION_Childs "Childs"
#define OT_JSON_REQUEST_Url "URL"
#define OT_JSON_REQUEST_Message "Message"
#define OT_JSON_REQUEST_Endpoint "Endpoint"

#define INFO_COLLECTION_VERSION "1.0"
#define INFO_ITEM_TYPE_Filter "Filter"
#define INFO_ITEM_TYPE_Request "Request"

#define TERMINAL_TXT_RECEIVER_MANUAL "<Manual>"
#define TERMINAL_TXT_RECEIVER_EDIT "<Edit>"

#define TERMINAL_TXT_ENDPOINT_EXECUTE "Execute"
#define TERMINAL_TXT_ENDPOINT_QUEUE "Queue"
#define TERMINAL_TXT_ENDPOINT_EXECUTE_OW_TLS "Execute One Way TLS"

#define TERMINAL_TXT_RESPONSE_LENGTH_PREFIX "Response legnth: "

#define TERMINAL_LOG(___message) OTOOLKIT_LOG("OTerminal", ___message)
#define TERMINAL_LOGW(___message) OTOOLKIT_LOGW("OTerminal", ___message)
#define TERMINAL_LOGE(___message) OTOOLKIT_LOGE("OTerminal", ___message)

#define TERMINAL_JSON_MEM_CHECK_EXISTS(___jsonObject, ___memberName, ___errorReturnCase) if (!___jsonObject.contains(___memberName)) { TERMINAL_LOGE(QString("JSON object member \"") + ___memberName + "\" is missing"); ___errorReturnCase; }
#define TERMINAL_JSON_MEM_CHECK_TYPE(___jsonObject, ___memberName, ___memberType, ___errorReturnCase)  if (!___jsonObject[___memberName].is##___memberType()) { TERMINAL_LOGE(QString("JSON object member \"") + ___memberName + "\" is not a " + #___memberType); ___errorReturnCase; }
#define TERMINAL_JSON_MEM_CHECK(___jsonObject, ___memberName, ___memberType, ___errorReturnCase) TERMINAL_JSON_MEM_CHECK_EXISTS(___jsonObject, ___memberName, ___errorReturnCase); TERMINAL_JSON_MEM_CHECK_TYPE(___jsonObject, ___memberName, ___memberType, ___errorReturnCase)

namespace intern {
	const Qt::ItemFlags FilterFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	const Qt::ItemFlags RequestFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

TerminalCollectionItem::TerminalCollectionItem(Terminal * _owner, const QString& _title) : m_owner(_owner) {
	setTitle(_title);
}

TerminalCollectionItem::~TerminalCollectionItem() {
	if (m_owner) {
		m_owner->notifyItemDeleted(this);
	}
}

TerminalCollectionItem * TerminalCollectionItem::createFromJsonObject(Terminal * _owner, const QJsonObject& _object) {
	TERMINAL_JSON_MEM_CHECK(_object, OT_JSON_COLLECTION_ItemType, String, return nullptr);
	QString type = _object[OT_JSON_COLLECTION_ItemType].toString();
	TerminalCollectionItem * newItm = nullptr;

	if (type == INFO_ITEM_TYPE_Filter) {
		newItm = new TerminalCollectionFilter(_owner, "");
		if (!newItm->setFromJsonObject(_object)) {
			delete newItm;
			newItm = nullptr;
			TERMINAL_LOGE("Creating empty terminal collection filter failed");
		}
	}
	else if (type == INFO_ITEM_TYPE_Request) {
		newItm = new TerminalRequest(_owner, "");
		if (!newItm->setFromJsonObject(_object)) {
			delete newItm;
			newItm = nullptr;
			TERMINAL_LOGE("Creating empty terminal collection request failed");
		}
	}
	else {
		TERMINAL_LOGE("Unknown terminal collection item type");
		otAssert(0, "Unknown item type");
	}
	return newItm;
}

void TerminalCollectionItem::setTitle(const QString& _title) {
	m_title = _title;
	setText(0, m_title);
}

// #####################################################################################################################################################################################

// #####################################################################################################################################################################################

// #####################################################################################################################################################################################

TerminalCollectionFilter::TerminalCollectionFilter(Terminal * _owner, const QString& _title)
	: TerminalCollectionItem(_owner, _title) 
{
	setIcon(0, QIcon(":/images/Folder.png"));
	setFlags(intern::FilterFlags);
}

TerminalCollectionFilter::~TerminalCollectionFilter() {}

bool TerminalCollectionFilter::setFromJsonObject(const QJsonObject& _object) {
	TERMINAL_JSON_MEM_CHECK(_object, OT_JSON_COLLECTION_ItemType, String, return false);
	TERMINAL_JSON_MEM_CHECK(_object, OT_JSON_COLLECTION_ItemName, String, return false);
	TERMINAL_JSON_MEM_CHECK(_object, OT_JSON_COLLECTION_Childs, Array, return false);

	if (_object[OT_JSON_COLLECTION_ItemType].toString() != INFO_ITEM_TYPE_Filter) return false;
	setTitle(_object[OT_JSON_COLLECTION_ItemName].toString());

	QJsonArray childArr = _object[OT_JSON_COLLECTION_Childs].toArray();
	for (int i = 0; i < childArr.count(); i++) {
		if (!childArr[i].isObject()) return false;
		QJsonObject obj = childArr[i].toObject();
		if (!obj.contains(OT_JSON_COLLECTION_ItemType)) {
			TERMINAL_LOGE("Terminal collection item type member is missing");
			return false;
		}
		if (!obj[OT_JSON_COLLECTION_ItemType].isString()) {
			TERMINAL_LOGE("Terminal collection item type member invalid type");
			return false;
		}
		if (obj[OT_JSON_COLLECTION_ItemType].toString() != INFO_ITEM_TYPE_Filter &&
			obj[OT_JSON_COLLECTION_ItemType].toString() != INFO_ITEM_TYPE_Request) 
		{
			TERMINAL_LOGE("Unknown terminal colellection item");
			return false;
		}
		TerminalCollectionItem* newChild = TerminalCollectionItem::createFromJsonObject(ownerTerminal(), obj);
		if (newChild) {
			addChild(newChild);
		}
		else {
			TERMINAL_LOGE("Failed to create terminal collection item");
		}
	}
	return true;
}

void TerminalCollectionFilter::addToJsonObject(QJsonObject& _object) const {
	_object[OT_JSON_COLLECTION_ItemType] = QString(INFO_ITEM_TYPE_Filter);
	_object[OT_JSON_COLLECTION_ItemName] = text(0);

	QJsonArray childArr;
	for (int i = 0; i < childCount(); i++) {
		TerminalCollectionItem * itm = dynamic_cast<TerminalCollectionItem *>(child(i));
		if (itm) {
			QJsonObject obj;
			itm->addToJsonObject(obj);
			childArr.push_back(obj);
		}
		else {
			TERMINAL_LOGE("Invalid treewidget item, cast to terminal collectio item failed");
			otAssert(0, "Invalid item");
		}
	}
	_object[OT_JSON_COLLECTION_Childs] = childArr;
}

bool TerminalCollectionFilter::hasDirectChildWith(const QString& _title) const {
	for (int i = 0; i < childCount(); i++) {
		if (child(i)->text(0) == _title) return true;
	}
	return false;
}

bool TerminalCollectionFilter::hasAnyChildWith(const QString& _title) const {
	for (int i = 0; i < childCount(); i++) {
		QTreeWidgetItem * childItm = child(i);
		if (childItm->text(0) == _title) return true;

		TerminalCollectionFilter * childFilter = dynamic_cast<TerminalCollectionFilter *>(childItm);
		if (childFilter) {
			if (childFilter->hasAnyChildWith(_title)) return true;
		}
	}
	return false;
}

bool TerminalCollectionFilter::hasChild(TerminalCollectionItem * _item) const {
	for (int i = 0; i < childCount(); i++) {
		if (child(i) == _item) return true;
		TerminalCollectionFilter * childFilter = dynamic_cast<TerminalCollectionFilter *>(child(i));
		if (childFilter) {
			if (childFilter->hasChild(_item)) return true;
		}
	}
	return false;
}

// #####################################################################################################################################################################################

// #####################################################################################################################################################################################

// #####################################################################################################################################################################################

TerminalRequest::TerminalRequest(Terminal * _owner, const QString& _title)
	: TerminalCollectionItem(_owner, _title), m_endpoint(ot::EXECUTE)
{
	setIcon(0, QIcon(":/images/Run.png"));
	setFlags(intern::RequestFlags);
}

TerminalRequest::~TerminalRequest(void) {}

bool TerminalRequest::setFromJsonObject(const QJsonObject& _object) {
	TERMINAL_JSON_MEM_CHECK(_object, OT_JSON_COLLECTION_ItemType, String, return false);
	TERMINAL_JSON_MEM_CHECK(_object, OT_JSON_COLLECTION_ItemName, String, return false);

	TERMINAL_JSON_MEM_CHECK(_object, OT_JSON_REQUEST_Url, String, return false);
	TERMINAL_JSON_MEM_CHECK(_object, OT_JSON_REQUEST_Message, String, return false);
	TERMINAL_JSON_MEM_CHECK(_object, OT_JSON_REQUEST_Endpoint, String, return false);
	
	if (_object[OT_JSON_COLLECTION_ItemType].toString() != INFO_ITEM_TYPE_Request) return false;
	setTitle(_object[OT_JSON_COLLECTION_ItemName].toString());

	m_url = _object[OT_JSON_REQUEST_Url].toString();
	m_messageBody = _object[OT_JSON_REQUEST_Message].toString();
	m_endpoint = ot::stringToMessageTypeFlag(_object[OT_JSON_REQUEST_Endpoint].toString().toStdString());

	return true;
}

void TerminalRequest::addToJsonObject(QJsonObject& _object) const {
	_object[OT_JSON_COLLECTION_ItemType] = QString(INFO_ITEM_TYPE_Request);
	_object[OT_JSON_COLLECTION_ItemName] = text(0);

	_object[OT_JSON_REQUEST_Url] = m_url;
	_object[OT_JSON_REQUEST_Message] = m_messageBody;
	_object[OT_JSON_REQUEST_Endpoint] = QString::fromStdString(ot::toString(m_endpoint));
}

// #####################################################################################################################################################################################

// #####################################################################################################################################################################################

// #####################################################################################################################################################################################

Terminal::Terminal() : m_exportLock(false) {
	TERMINAL_LOG("Initializing OTerminal...");

	// Create layouts
	m_splitter = new QSplitter;
	m_splitter->setObjectName("OToolkit_Terminal_MainSplitter");

	m_leftLayoutW = new QWidget;
	m_leftLayout = new QVBoxLayout(m_leftLayoutW);

	m_buttonLayout = new QGridLayout;
	m_navigationLayout = new QHBoxLayout;

	m_rightLayoutW = new QWidget;
	m_rightLayout = new QVBoxLayout(m_rightLayoutW);

	m_rightSplitter = new QSplitter;
	
	m_receiverBox = new QGroupBox("Configuration");
	m_receiverLayout = new QGridLayout;
	
	m_messageBox = new QGroupBox;
	m_messageLayout = new QVBoxLayout;

	m_responseBox = new QGroupBox;
	m_responseLayout = new QVBoxLayout;

	// Create controls
	m_navigation = new QTreeWidget;

	m_receiverNameL = new QLabel("Receiver Name:");
	m_receiverName = new QComboBox;

	m_receiverUrlL = new QLabel("Receiver URL:");
	m_receiverUrl = new QLineEdit;

	m_endpointL = new QLabel("Enpoint:");
	m_endpoint = new QComboBox;

	m_btnSend = new QPushButton("Send");
	
	m_messageEdit = new JSONEditor;

	m_responseEdit = new JSONEditor;
	m_responseLength = new QLabel(TERMINAL_TXT_RESPONSE_LENGTH_PREFIX "0");

	m_progressBar = new QProgressBar;

	// Setup controls
	m_navigation->setHeaderHidden(true);
	m_navigation->setContextMenuPolicy(Qt::CustomContextMenu);

	QFont rFont = m_receiverName->font();
	rFont.setFixedPitch(true);
	rFont.setFamily("Consolas");
	m_receiverName->setFont(rFont);

	initializeServices(); // this call will add the last service information
	m_receiverName->addItems({ TERMINAL_TXT_RECEIVER_MANUAL, TERMINAL_TXT_RECEIVER_EDIT });
	m_receiverName->setCurrentText(m_receiverName->itemText(m_receiverName->count() - 2));

	m_endpoint->addItems({ TERMINAL_TXT_ENDPOINT_EXECUTE, TERMINAL_TXT_ENDPOINT_QUEUE, TERMINAL_TXT_ENDPOINT_EXECUTE_OW_TLS });
	m_endpoint->setCurrentIndex(0);

	m_progressBar->setStyleSheet("QProgressBar {border: none; text-align: center; } " 
		"QProgressBar::chunk {background-color: #FFE61D; width: 1px; }");

	m_progressBar->setMaximumHeight(5);

	m_btnSend->setMinimumWidth(50);

	// Setup layouts
	m_splitter->addWidget(m_leftLayoutW);
	m_splitter->addWidget(m_rightLayoutW);

	m_leftLayout->addLayout(m_buttonLayout);
	m_leftLayout->addLayout(m_navigationLayout, 1);

	m_navigationLayout->addWidget(m_navigation);

	m_rightLayout->addWidget(m_rightSplitter, 1);
	m_rightLayout->addWidget(m_progressBar);

	m_rightSplitter->setOrientation(Qt::Vertical);

	m_rightSplitter->addWidget(m_receiverBox);
	m_rightSplitter->addWidget(m_messageBox);
	m_rightSplitter->addWidget(m_responseBox);

	m_rightSplitter->setStretchFactor(1, 1);

	m_receiverBox->setLayout(m_receiverLayout);
	m_messageBox->setLayout(m_messageLayout);
	m_responseBox->setLayout(m_responseLayout);

	m_receiverLayout->addWidget(m_receiverNameL, 0, 0);
	m_receiverLayout->addWidget(m_receiverName, 0, 1);
	m_receiverLayout->addWidget(m_receiverUrlL, 1, 0);
	m_receiverLayout->addWidget(m_receiverUrl, 1, 1);
	m_receiverLayout->addWidget(m_endpointL, 0, 2);
	m_receiverLayout->addWidget(m_endpoint, 0, 3);
	m_receiverLayout->addWidget(m_btnSend, 1, 5);

	m_receiverLayout->setColumnStretch(4, 1);

	m_messageLayout->addWidget(m_messageEdit);

	m_responseLayout->addWidget(m_responseEdit, 1);
	m_responseLayout->addWidget(m_responseLength, 0);

	// Restore settings
	QSettings s("OpenTwin", applicationName());
	m_receiverUrl->setText(s.value("Terminal.Receiver", "127.0.0.1:XXXX").toString());
	m_messageEdit->setPlainText(s.value("Terminal.Message", "{\n\t\"action\": \"Ping\"\n}").toString());

	// Setup navigation
	m_requestsRootFilter = new TerminalCollectionFilter(this, "Requests");
	m_requestsRootFilter->setFlags(m_requestsRootFilter->flags() & ~(Qt::ItemIsEditable));
	m_navigation->addTopLevelItem(m_requestsRootFilter);
	
	slotLoadRequestCollection();

	// Connect signals
	connect(m_navigation, &QTreeWidget::customContextMenuRequested, this, &Terminal::slotShowNavigationContextMenu);
	connect(m_navigation, &QTreeWidget::itemDoubleClicked, this, &Terminal::slotNavigationItemDoubleClicked);
	connect(m_navigation, &QTreeWidget::itemChanged, this, &Terminal::slotNavigationItemChanged);
	connect(m_btnSend, &QPushButton::clicked, this, &Terminal::slotSendMessage);
	connect(m_receiverName, &QComboBox::currentTextChanged, this, &Terminal::slotServiceNameChanged);

	TERMINAL_LOG("Terminal initialization completed");
}

Terminal::~Terminal() {
	QSettings s("OpenTwin", applicationName());
	s.setValue("Terminal.Receiver", m_receiverUrl->text());
	s.setValue("Terminal.Message", m_messageEdit->toPlainText());
}

QString Terminal::toolName(void) const {
	return "OTerminal";
}

QWidget * Terminal::widget(void) {
	return m_splitter;
}

void Terminal::notifyItemDeleted(TerminalCollectionItem * _item) {

}

void Terminal::setEndpointFromMessageType(ot::MessageType _type) {
	switch (_type)
	{
	case ot::QUEUE: m_endpoint->setCurrentText(TERMINAL_TXT_ENDPOINT_QUEUE); break;
	case ot::EXECUTE: m_endpoint->setCurrentText(TERMINAL_TXT_ENDPOINT_EXECUTE); break;
	case ot::EXECUTE_ONE_WAY_TLS: m_endpoint->setCurrentText(TERMINAL_TXT_ENDPOINT_EXECUTE_OW_TLS); break;
	case ot::SECURE_MESSAGE_TYPES:
	case ot::ALL_MESSAGE_TYPES:
		otAssert(0, "Invalid message type");
		break;
	default:
		otAssert(0, "Unknown message type");
		break;
	}
}

ot::MessageType Terminal::endpointToMessageType(void) const {
	QString txt = m_endpoint->currentText();
	if (txt == TERMINAL_TXT_ENDPOINT_EXECUTE) return ot::EXECUTE;
	else if (txt == TERMINAL_TXT_ENDPOINT_EXECUTE_OW_TLS) return ot::EXECUTE_ONE_WAY_TLS;
	else if (txt == TERMINAL_TXT_ENDPOINT_QUEUE) return ot::QUEUE;
	else {
		otAssert(0, "Unknown endpoint");
		return ot::EXECUTE;
	}
}

// ################################################################################################################################

// Private: Slots

void Terminal::slotSendMessage(void) {
	// Check receiver input
	QString rec = m_receiverUrl->text();
	if (rec.isEmpty()) {
		displayErrorStatusText("Input error: No receiver URL provided");
		return;
	}

	// Check endpoint
	ot::MessageType messageType;
	QString msgTypeString = m_endpoint->currentText();
	if (msgTypeString == TERMINAL_TXT_ENDPOINT_EXECUTE) { messageType = ot::EXECUTE; }
	else if (msgTypeString == TERMINAL_TXT_ENDPOINT_QUEUE) { messageType = ot::QUEUE; }
	else if (msgTypeString == TERMINAL_TXT_ENDPOINT_EXECUTE_OW_TLS) { messageType = ot::EXECUTE_ONE_WAY_TLS; }
	else {
		assert(0);
		displayErrorStatusText("Input error: Unknown endpoint selected");
		return;
	}

	// Check message input
	QJsonParseError * err = new QJsonParseError;
	QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(m_messageEdit->toPlainText().toStdString()), err);
	if (err->error != QJsonParseError::NoError) {
		displayErrorStatusText("Invalid message format (expected JSON Object): " + err->errorString());
		delete err;
		return;
	}
	delete err;

	// Create compact message to send
	QByteArray data = doc.toJson(QJsonDocument::Compact);

	// Update controls
	setWaitingMode(true);

	m_responseEdit->clear();
	m_responseLength->setText(TERMINAL_TXT_RESPONSE_LENGTH_PREFIX "0");

	// Run parallel thread to send the message
	std::thread t(&Terminal::workerSendMessage, this, rec.toStdString(), messageType, data);
	t.detach();
}

void Terminal::slotMessageSendSuccessful(const QByteArray& _response) {
	setWaitingMode(false);

	m_responseLength->setText(TERMINAL_TXT_RESPONSE_LENGTH_PREFIX + QString::number(_response.count()));

	// Check if the response was in a JSON format
	QJsonParseError * err = new QJsonParseError;
	QJsonDocument doc = QJsonDocument::fromJson(_response, err);
	if (err->error == QJsonParseError::NoError) {
		m_responseEdit->setPlainText(doc.toJson(QJsonDocument::Indented));
	}
	else {
		m_responseEdit->setPlainText(_response);
	}
	delete err;

}

void Terminal::slotMessageSendFailed(const QString& _errorString) {
	setWaitingMode(false);

	displayErrorStatusText(_errorString);
}

void Terminal::slotServiceNameChanged(void) {
	QString txt = m_receiverName->currentText();
	if (txt == TERMINAL_TXT_RECEIVER_EDIT) {
		m_receiverName->setCurrentText(TERMINAL_TXT_RECEIVER_MANUAL);
	}
	else if (txt == TERMINAL_TXT_RECEIVER_MANUAL) {
		m_receiverUrl->setEnabled(true);
	}
	else {
		int ix = m_receiverName->currentIndex();
		if (ix < 0 || ix >= m_services.count()) {
			assert(0);
			displayErrorStatusText("Invalid receiver selected: \"" + txt + "\"");
			m_receiverName->setCurrentText(TERMINAL_TXT_RECEIVER_MANUAL);
			return;
		}
		m_receiverUrl->setEnabled(false);
		m_receiverUrl->setText(m_services[ix].url);
	}
}

void Terminal::slotShowNavigationContextMenu(const QPoint& _pt) {
	// Get the desired item
	QTreeWidgetItem * selectedItem = m_navigation->itemAt(_pt);
	if (selectedItem == nullptr) {
		// Right click NOT on an item
		return;
	}

	// Determine the root item (category)
	QTreeWidgetItem * rootItem = selectedItem;
	while (rootItem->parent()) {
		rootItem = rootItem->parent();
	}

	if (rootItem == m_requestsRootFilter) {
		TerminalCollectionFilter * filter = dynamic_cast<TerminalCollectionFilter *>(selectedItem);
		TerminalRequest * request = dynamic_cast<TerminalRequest *>(selectedItem);
		if (filter) {
			handleContextFilter(_pt, filter);
		}
		else if (request) {
			handleContextRequest(_pt, request);
		}
		else {
			otAssert(0, "Cast failed");
			OTOOLKIT_LOGE("Terminal", "Navigation item cast failed");
		}
	}
	else {
		otAssert(0, "Unknown root item");
		OTOOLKIT_LOGE("Terminal", "Unknown navigation root item");
	}
}

void Terminal::slotNavigationItemDoubleClicked(QTreeWidgetItem* _item, int _column) {
	TerminalRequest* request = dynamic_cast<TerminalRequest*>(_item);
	if (request) {
		applyRequest(request);
	}
}

void Terminal::slotNavigationItemChanged(QTreeWidgetItem* _item, int _column) {
	TerminalCollectionItem* itm = dynamic_cast<TerminalCollectionItem*>(_item);
	if (itm) { 
		if (itm->text(0) != itm->title()) {
			itm->setTitle(itm->text(0));
			slotSaveRequestCollection();
		} 
	}
	else {
		otAssert(0, "Unknown navigation item");
		TERMINAL_LOGE("Unknwon navitaion item");
	}
}

void Terminal::slotLoadRequestCollection(void) {
	TERMINAL_LOG("Loading OTerminal request collection...");

	QSettings s("OpenTwin", applicationName());

	QByteArray bArr = s.value("Terminal.RequestCollection", QByteArray()).toByteArray();
	if (bArr.isEmpty()) {
		TERMINAL_LOGW("Skipping load of Terminal request collection: No collection found");
		return;
	}
	QJsonDocument requestConfigDoc = QJsonDocument::fromJson(bArr);
	if (!requestConfigDoc.isObject()) {
		AppBase::instance()->sb()->setErrorInfo("Failed to load request collection");
		TERMINAL_LOGE("Terminal request collection load failed: JSON document is not an object");
		return;
	}

	// Check members
	QJsonObject obj = requestConfigDoc.object();

	TERMINAL_JSON_MEM_CHECK(obj, OT_JSON_COLLECTION_Version, String, return);
	TERMINAL_JSON_MEM_CHECK(obj, OT_JSON_COLLECTION_Data, Object, return);

	// Check version
	if (obj[OT_JSON_COLLECTION_Version].toString() != INFO_COLLECTION_VERSION) {
		AppBase::instance()->sb()->setErrorInfo("Failed to load request collection");
		TERMINAL_LOGE("Terminal request collection load failed: Invalid version");
		return;
	}

	QJsonObject rootObject = obj[OT_JSON_COLLECTION_Data].toObject();
	if (m_requestsRootFilter->setFromJsonObject(rootObject)) {
		TERMINAL_LOG("Terminal request collection loaded successfully.");
	}
	else {
		TERMINAL_LOGE("Terminal request collection load failed (Check previous messages).");
	}
}

void Terminal::slotSaveRequestCollection(void) {
	if (m_exportLock) return;

	TERMINAL_LOG("Saving OTerminal request collection...");

	QJsonObject obj;
	obj[OT_JSON_COLLECTION_Version] = INFO_COLLECTION_VERSION;

	QJsonObject rootObject;
	m_requestsRootFilter->addToJsonObject(rootObject);
	obj[OT_JSON_COLLECTION_Data] = rootObject;

	QJsonDocument doc(obj);

	QSettings s("OpenTwin", applicationName());
	s.setValue("Terminal.RequestCollection", doc.toJson());

	TERMINAL_LOG("Terminal request collection saved successfully.");
}

// ################################################################################################################################

// Private: Helper

void Terminal::initializeServices(void) {
	// todo: add import from last settings
	initializeDefaultServices();

	if (m_services.isEmpty()) { return; }

	// Find the longest short form
	int longestShort = 0;
	for (auto s : m_services) {
		if (s.shortName.length() > longestShort) {
			longestShort = s.shortName.length();
		}
	}

	// If no entry has a short form, dont increase the spacing
	if (longestShort > 0) { longestShort += 3; }
	
	// Add items in a nice readable way
	for (auto s : m_services) {
		QString txt;
		if (longestShort > 0) {
			txt = s.shortName;
			while (txt.length() < longestShort) { txt.append(' '); }
		}
		txt.append(s.name);
		m_receiverName->addItem(txt);
	}
}

void Terminal::initializeDefaultServices(void) {
	ServiceInfo logger;
	logger.name = "Logger Service";
	logger.shortName = "Log";
	logger.url = "127.0.0.1:8090";
	
	ServiceInfo auth;
	auth.name = "Authorization Service";
	auth.shortName = "Auth";
	auth.url = "127.0.0.1:8092";

	ServiceInfo gss;
	gss.name = "Global Session Service";
	gss.shortName = "GSS";
	gss.url = "127.0.0.1:8091";

	ServiceInfo lss;
	lss.name = "Local Session Service";
	lss.shortName = "LSS";
	lss.url = "127.0.0.1:8093";

	ServiceInfo gds;
	gds.name = "Global Directory Service";
	gds.shortName = "GDS";
	gds.url = "127.0.0.1:9094";

	ServiceInfo lds;
	lds.name = "Local Directory Service";
	lds.shortName = "LDS";
	lds.url = "127.0.0.1:9095";

	m_services.push_back(logger);
	m_services.push_back(auth);
	m_services.push_back(gss);
	m_services.push_back(lss);
	m_services.push_back(gds);
	m_services.push_back(lds);
}

void Terminal::setWaitingMode(bool _isWaiting) {
	m_btnSend->setEnabled(!_isWaiting);
	m_splitter->setEnabled(!_isWaiting);
	m_progressBar->setRange(0, (_isWaiting ? 0 : 1));
}

// ################################################################################################################################

// Private: Context Menu handling

void Terminal::handleContextFilter(const QPoint& _pt, TerminalCollectionFilter * _filter) {
	QMenu menu(m_navigation);

	// Add actions
	QAction * actionNewFilter = menu.addAction(QIcon(":/images/NewFolder.png"), "Add Filter");
	actionNewFilter->setToolTip("Add new child filter to this filter");

	QAction * actionRename = nullptr;
	QAction * actionRemove = nullptr;
	// Add optional actions
	if (_filter != m_requestsRootFilter) {
		QAction* actionRemove = menu.addAction(QIcon(":/images/Remove.png"), "Remove");
		actionRemove->setToolTip("Remove this filter and all of its childs");

		QAction* actionRename = menu.addAction(QIcon(":/images/Rename.png"), "Rename");
		actionRename->setToolTip("Enter rename mode");
	}

	menu.addSeparator();

	QAction* actionNewRequest = menu.addAction(QIcon(":/images/Add.png"), "New Request");

	menu.addSeparator();

	QAction* actionImport = menu.addAction(QIcon(":/images/Import.png"), "Import Request(s)");
	QAction* actionExport = menu.addAction(QIcon(":/images/Export.png"), "Export Request(s)");

	// Show context menu
	QAction * result = menu.exec(m_navigation->mapToGlobal(_pt));
	if (result == nullptr) {
		return;
	}
	
	// Check selected action
	if (result == actionNewFilter) {
		addNewFilter(_filter);
	}
	else if (result == actionRename) {
		m_navigation->editItem(_filter);
	}
	else if (result == actionRemove) {
		removeFilter(_filter);
	}
	else if (result == actionNewRequest) {
		addNewRequestFromCurrent(_filter);
	}
	else if (result == actionImport) {

	}
	else if (result == actionExport) {

	}
}

void Terminal::handleContextRequest(const QPoint& _pt, TerminalRequest * _request) {
	QMenu menu(m_navigation);

	// Add actions
	QAction* actionApply = menu.addAction(QIcon(":/images/Ok.png"), "Apply");
	actionApply->setToolTip("Set this as currently active configuration");

	QAction* actionApplyAndSend = menu.addAction(QIcon(":/images/Go.png"), "Apply and Send");
	actionApplyAndSend->setToolTip("Set this as currently active configuration and send the message");

	menu.addSeparator();
	QAction* actionReplace = menu.addAction(QIcon(":/images/DownPage.png"), "Set values from current");
	actionReplace->setToolTip("Set values from currently active configuration");

	menu.addSeparator();
	QAction* actionRemove = menu.addAction(QIcon(":/images/Remove.png"), "Remove");
	actionRemove->setToolTip("Remove this request");

	QAction* actionRename = menu.addAction(QIcon(":/images/Rename.png"), "Rename");
	actionRename->setToolTip("Enter rename mode");
	
	// Show context menu
	QAction * result = menu.exec(m_navigation->mapToGlobal(_pt));
	if (result == nullptr) {
		return;
	}

	// Check selected action
	if (result == actionApply) {
		applyRequest(_request);
	}
	else if (result == actionApplyAndSend) {
		applyAndSendRequest(_request);
	}
	else if (result == actionReplace) {
		updateRequestFromCurrent(_request);
	}
	else if (result == actionRename) {
		m_navigation->editItem(_request);
	}
	else if (result == actionRemove) {
		removeRequest(_request);
	}
}

void Terminal::addNewFilter(TerminalCollectionFilter * _parentFilter) {
	QString filterName = "New Filter";
	int filterNameCount = 0;
	while (_parentFilter->hasAnyChildWith(filterName)) {
		filterName = "New Filter " + QString::number(++filterNameCount);
	}

	TerminalCollectionFilter * newFilter = new TerminalCollectionFilter(this, filterName);

	// Add child and ensure visibility
	_parentFilter->addChild(newFilter);
	if (!_parentFilter->isExpanded()) {
		_parentFilter->setExpanded(true);
	}

	slotSaveRequestCollection();

	m_navigation->editItem(newFilter);
}

void Terminal::removeFilter(TerminalCollectionFilter * _filter) {
	if (_filter == m_requestsRootFilter) {
		TERMINAL_LOGE("Cannot remove root filter. Skipping.");
	}
	else {
		QTreeWidgetItem * par = _filter->parent();
		if (par) {
			m_exportLock = true;
			par->removeChild(_filter);
			delete _filter;
			m_exportLock = false;
		}
		else {
			TERMINAL_LOGE("Parent item not found to remove from");
		}
	}

	slotSaveRequestCollection();
}

void Terminal::addNewRequestFromCurrent(TerminalCollectionFilter * _parentFilter) {
	QString requestName = "New Request";
	int requestNameCount = 1;
	while (_parentFilter->hasAnyChildWith(requestName)) {
		requestName = "New Request " + QString::number(++requestNameCount);
	}

	TerminalRequest * newRequest = new TerminalRequest(this, requestName);

	// Add child and ensure visibility
	_parentFilter->addChild(newRequest);
	if (!_parentFilter->isExpanded()) {
		_parentFilter->setExpanded(true);
	}

	// This call will save the collection
	updateRequestFromCurrent(newRequest);

	m_navigation->editItem(newRequest);
}

void Terminal::updateRequestFromCurrent(TerminalRequest* _request) {
	_request->setUrl(m_receiverUrl->text());
	_request->setEndpoint(endpointToMessageType());
	_request->setMessageBody(m_messageEdit->toPlainText());

	slotSaveRequestCollection();
}

void Terminal::removeRequest(TerminalRequest * _request) {
	QTreeWidgetItem* par = _request->parent();
	if (par) {
		m_exportLock = true;
		par->removeChild(_request);
		delete _request;
		m_exportLock = false;
	}
	else {
		otAssert(0, "No parent item");
		TERMINAL_LOGE("Parent item not found");
	}

	slotSaveRequestCollection();
}

void Terminal::applyRequest(TerminalRequest* _request) {
	m_receiverName->setCurrentText(TERMINAL_TXT_RECEIVER_MANUAL);
	m_receiverUrl->setText(_request->url());
	m_messageEdit->setPlainText(_request->messageBody());
	setEndpointFromMessageType(_request->endpoint());
}

void Terminal::applyAndSendRequest(TerminalRequest* _request) {
	applyRequest(_request);
	slotSendMessage();
}

void Terminal::exportToFile(TerminalCollectionFilter* _filter) {
	if (m_exportLock) return;

	QJsonObject obj;
	obj[OT_JSON_COLLECTION_Version] = INFO_COLLECTION_VERSION;

	QJsonObject rootObject;
	_filter->addToJsonObject(obj);
	obj[OT_JSON_COLLECTION_Data] = rootObject;

	QSettings s("OpenTwin", APP_BASE_APP_NAME);
	QString fn = QFileDialog::getSaveFileName(widget(), "Export OTerminal Collection", s.value("Terminal.LastCollection", "").toString(), "OTerminal Collection (*.oterm.json)");
	if (fn.isEmpty()) {
		return;
	}

	QJsonObject docObj;
	QJsonDocument doc(docObj);
	QFile f(fn);
	if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		TERMINAL_LOGE("Failed to open file for writing. File:\n" + fn);
		return;
	}

	f.write(doc.toJson());
	f.close();

	TERMINAL_LOG("Collection exported successfully to file: " + fn);
}

void Terminal::importFromFile(TerminalCollectionFilter* _filter) {

}

// ################################################################################################################################

// Private: Async worker functions

void Terminal::workerSendMessage(const std::string& _receiverUrl, ot::MessageType _messageType, const QByteArray& _data) {
	std::string response;
	if (!ot::msg::send("", _receiverUrl, _messageType, _data.toStdString(), response)) {
		if (!QMetaObject::invokeMethod(this, "slotMessageSendFailed", Qt::QueuedConnection, Q_ARG(const QString&, QString(
			"Failed to send message to \"" + QString::fromStdString(_receiverUrl) + "\""
		)))) {
			OTOOLKIT_LOGE("Terminal Worker", "Failed to queue send failed call");
		}
	}
	else {
		if (!QMetaObject::invokeMethod(this, "slotMessageSendSuccessful", Qt::QueuedConnection, Q_ARG(const QByteArray&, QByteArray::fromStdString(response)))) {
			OTOOLKIT_LOGE("Terminal Worker", "Failed to queue send success call");
		}
	}
}