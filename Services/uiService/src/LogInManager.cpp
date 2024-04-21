#include "LogInManager.h"
#include "AppBase.h"
#include "ExternalServicesComponent.h"
#include "UserManagement.h"

#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"

#include <akAPI/uiAPI.h>
#include <akDialogs/aLogInDialog.h>
#include <akWidgets/aPushButtonWidget.h>
#include <akWidgets/aTableWidget.h>
#include <akWidgets/aLabelWidget.h>
#include <akWidgets/aComboButtonWidget.h>
#include <akWidgets/aLineEditWidget.h>

#include <qevent.h>
#include <qlayout.h>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include <qjsonobject.h>

using namespace ak;

enum tableColumn {
	tcName,
	tcIP,
	tcPort,
	tcDelete,
	tcCount
};

void threadConnectToSessionService(LogInManager * _notifier, ExternalServicesComponent * _externalServicesComponent, const std::string& _sessionServiceUrl) {
	std::string response;

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetDBandAuthServerUrl, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString(OT_INFO_MESSAGE_LogIn, doc.GetAllocator()), doc.GetAllocator());
	if (!_externalServicesComponent->sendHttpRequest(ExternalServicesComponent::EXECUTE, _sessionServiceUrl, doc.toJson(), response)) {
		QMetaObject::invokeMethod(_notifier, "slotConnectToSessionServiceFail", Qt::QueuedConnection, Q_ARG(const QString&, "Failed to connect to the session service at url: " + QString::fromStdString(_sessionServiceUrl))); return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		QMetaObject::invokeMethod(_notifier, "slotConnectToSessionServiceFail", Qt::QueuedConnection, Q_ARG(const QString&, response.c_str())); return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		QMetaObject::invokeMethod(_notifier, "slotConnectToSessionServiceFail", Qt::QueuedConnection, Q_ARG(const QString&, response.c_str())); return;
	}
	QMetaObject::invokeMethod(_notifier, "slotConnectToSessionServiceSuccess", Qt::QueuedConnection, Q_ARG(const QString&, response.c_str())); return;
}

void threadConnectToSessionServiceRegister(LogInManager * _notifier, ExternalServicesComponent * _externalServicesComponent, const std::string& _sessionServiceUrl) {
	std::string response;

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetDBandAuthServerUrl, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString(OT_INFO_MESSAGE_LogIn, doc.GetAllocator()), doc.GetAllocator());
	if (!_externalServicesComponent->sendHttpRequest(ExternalServicesComponent::EXECUTE, _sessionServiceUrl, doc.toJson(), response)) {
		QMetaObject::invokeMethod(_notifier, "slotConnectToSessionServiceFail", Qt::QueuedConnection, Q_ARG(const QString&, "Failed to connect to the session service at url: " + QString::fromStdString(_sessionServiceUrl))); return;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		QMetaObject::invokeMethod(_notifier, "slotConnectToSessionServiceFail", Qt::QueuedConnection, Q_ARG(const QString&, response.c_str())); return;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		QMetaObject::invokeMethod(_notifier, "slotConnectToSessionServiceFail", Qt::QueuedConnection, Q_ARG(const QString&, response.c_str())); return;
	}
	QMetaObject::invokeMethod(_notifier, "slotConnectToSessionServiceRegisterSuccess", Qt::QueuedConnection, Q_ARG(const QString&, response.c_str())); return;
}

void threadConnectToDatabase(LogInManager * _notifier, ExternalServicesComponent * _externalServicesComponent, const std::string& _databaseUrl, const std::string& _authUrl, const std::string& _username, const std::string& _password, bool _isEncryptedPassword, int * _logInAttempt) {
	UserManagement uManager;
	uManager.setAuthServerURL(_authUrl);
	uManager.setDatabaseURL(_databaseUrl);
	if (!uManager.checkConnectionAuthorizationService()) {
		QMetaObject::invokeMethod(_notifier, "slotConnectToDatabaseFail", Qt::QueuedConnection, Q_ARG(const QString&, "Failed to connect to the authorization service."));
		return;
	}

	// Check the username, password combination
	std::string validPassword, validEncryptedPassword;

	if (!uManager.checkPassword(_username, _password, _isEncryptedPassword, validPassword, validEncryptedPassword)) {
		*_logInAttempt = *_logInAttempt + 1;
		if (*_logInAttempt > 5) {
			QMetaObject::invokeMethod(_notifier, &LogInManager::slotCancelLogIn, Qt::QueuedConnection);
			return;
		}
		else {
			QMetaObject::invokeMethod(_notifier, "slotConnectToDatabaseFail", Qt::QueuedConnection, Q_ARG(const QString&, "Invalid username and password combination"));
			return;
		}
	}
	else {

		if (!uManager.checkConnectionDataBase(_username, validPassword)) {
			QMetaObject::invokeMethod(_notifier, "slotConnectToDatabaseFail", Qt::QueuedConnection, Q_ARG(const QString&, "Failed to connect to the data base."));
			return;
		}

		QMetaObject::invokeMethod(_notifier, "slotConnectToDatabaseSuccess", Qt::QueuedConnection, 
								  Q_ARG(const QString&, _databaseUrl.c_str()), Q_ARG(const QString&, _authUrl.c_str()), 
								  Q_ARG(const QString&, _username.c_str()), Q_ARG(const QString&, validPassword.c_str()), Q_ARG(const QString&, validEncryptedPassword.c_str()));
	}
}

// #######################################################################################################################################

// #######################################################################################################################################

// #######################################################################################################################################

LogInSessionServiceEntry::LogInSessionServiceEntry(const QString& _name, const QString& _ip, const QString& _port)
	: m_name(_name), m_ip(_ip), m_port(_port) {}

LogInSessionServiceEntry::LogInSessionServiceEntry(const LogInSessionServiceEntry& _other)
	: m_name(_other.m_name), m_ip(_other.m_ip), m_port(_other.m_port) {}

LogInSessionServiceEntry::~LogInSessionServiceEntry() {}

LogInSessionServiceEntry& LogInSessionServiceEntry::operator = (const LogInSessionServiceEntry& _other) {
	m_name = _other.m_name;
	m_ip = _other.m_ip;
	m_port = _other.m_port;
	return *this;
}

// ####################################################################################################

// Getter

QString LogInSessionServiceEntry::url(void) const { return m_ip + ":" + m_port; }

QString LogInSessionServiceEntry::displayText(void) const { return m_name + ": " + url(); }

bool LogInSessionServiceEntry::isValid(void) const {
	bool ipOk{ false };
	if (m_ip == "localhost") { ipOk = true; }
	else if (m_ip.length() > 6) {
		QStringList lst = m_ip.split(".", Qt::SkipEmptyParts);
		if (lst.count() == 4) {
			ipOk = true;
			for (QString str : lst) {
				for (QChar c : str) {
					if (!c.isDigit()) { ipOk = false; break; }
				}
				if (!ipOk) break;
			}
		}
	}

	bool portOk{ true };
	if (m_port.isEmpty()) { portOk = false; }
	else {
		for (auto c : m_port) {
			if (!c.isDigit()) { portOk = false; break; }
		}
	}

	return ipOk && portOk && !m_name.isEmpty();
}

bool LogInSessionServiceEntry::isEmpty(void) const {
	return m_name.isEmpty() && m_ip.isEmpty() && m_port.isEmpty();
}

// #######################################################################################################################################

// #######################################################################################################################################

// #######################################################################################################################################

LogInManager::LogInManager() 
	: m_ignoreEvents(false), m_logInAttempt(0)
{
	QString lastUser = uiAPI::settings::getString("LastUsername", "");
	QString lastPassword = uiAPI::settings::getString("LastPassword", "");
	QString lastSessionServiceURL = uiAPI::settings::getString("SessionServiceURL", "");
	QByteArray sessionServiceJSON = uiAPI::settings::getByteArray("SessionServiceJSON");
	bool lastSavePassword = uiAPI::settings::getBool("LastSavePassword", false);

	AppBase * app = AppBase::instance();
	m_dialog = new aLogInDialog(true, uiAPI::getPixmap("OpenTwin"), lastUser, lastPassword);
	m_dialog->setReturnHashedPassword(false);
	m_dialog->setWindowIcon(uiAPI::getApplicationIcon("OpenTwin"));
	m_dialog->setWaitingAnimationDelay(500);
	connect(m_dialog, &aLogInDialog::logInRequested, this, &LogInManager::slotLogIn);
	connect(m_dialog, &aLogInDialog::registerRequested, this, &LogInManager::slotRegister);

#ifdef OT_USE_GSS
	m_sessionServiceL = new aLabelWidget("Global session service:");
#else
	m_sessionServiceL = new aLabelWidget("Session service:");
#endif
	m_sessionService = new aComboButtonWidget;
	m_sessionService->setObjectName("LogInDialogSessionServiceUrl");
	m_sessionService->setMinimumWidth(200);
	
	if (!sessionServiceJSON.isEmpty()) {
		QJsonDocument doc = QJsonDocument::fromJson(sessionServiceJSON);
		if (doc.isArray()) {
			QJsonArray arr = doc.array();
			for (int i = 0; i < arr.count(); i++) {
				if (arr[i].isObject()) {
					QJsonObject obj = arr[i].toObject();
					if (!obj.contains("Name")) { assert(0); break; }
					if (!obj.contains("IP")) { assert(0); break; }
					if (!obj.contains("Port")) { assert(0); break; }
					if (!obj["Name"].isString()) { assert(0); break; }
					if (!obj["IP"].isString()) { assert(0); break; }
					if (!obj["Port"].isString()) { assert(0); break; }

					m_entries.push_back(LogInSessionServiceEntry(obj["Name"].toString(), obj["IP"].toString(), obj["Port"].toString()));
					if (m_entries.back().displayText() == lastSessionServiceURL) {
						m_sessionService->setText(lastSessionServiceURL);
					}
				}
				else {
					assert(0); // Entry is not an object
				}
			}
		}
		else {
			assert(0); // Document is not an array
		}
	}

	rebuildSessionServiceSelection();

	aLogInDialogInputField * newField = new aLogInDialogInputField(m_sessionService, m_sessionServiceL);
	m_dialog->addCustomInput(newField);

	connect(m_sessionService, &aComboButtonWidget::changed, this, &LogInManager::slotSessionServiceChanged);
}

LogInManager::~LogInManager() {
	delete m_dialog;
}

// ####################################################################################################

// Setter

LogInSessionServiceEntry LogInManager::currentEntry(void) const {
	if (m_sessionService->text() == "<Edit>") { return LogInSessionServiceEntry("", "", ""); }
	for (auto e : m_entries) {
		if (e.displayText() == m_sessionService->text()) {
			return e;
		}
	}
	assert(0);
	return LogInSessionServiceEntry("", "", "");
}

QString LogInManager::username(void) {
	return m_dialog->username();
}
QString LogInManager::password(void) {
	return m_dialog->password();
}

bool LogInManager::showDialog(void) {
	if (m_dialog->showDialog() != resultOk) { return false; }
	return true;
}

void LogInManager::hideWaitingAnimation(void) {
	m_dialog->hideWaitingAnimation();
}

// ####################################################################################################

// Slots

void LogInManager::slotLogIn(void) {
	// Check the connection to the sessionService
	QString SessionServiceURL = currentEntry().url();
	if (SessionServiceURL.isEmpty() || SessionServiceURL == "<Edit>") {
		assert(0);
		return;
	}

	OT_LOG_I("Attempting to log-in to Global Session Service at \"" + SessionServiceURL.toStdString() + "\"");

 	m_dialog->showWaitingAnimation(uiAPI::getMovie("OpenTwinLoading"));

	std::thread t1(threadConnectToSessionService, this, AppBase::instance()->getExternalServicesComponent(), SessionServiceURL.toStdString());
	t1.detach();
}

void LogInManager::slotRegister(void) 
{
	// Check the connection to the sessionService
	QString SessionServiceURL = currentEntry().url();
	if (SessionServiceURL.isEmpty() || SessionServiceURL == "<Edit>") {
		assert(0);
		return;
	}

	OT_LOG_I("Attempting to register at Global Session Service at \"" + SessionServiceURL.toStdString() + "\"");

	m_dialog->showWaitingAnimation(uiAPI::getMovie("OpenTwinLoading"));

	std::thread t1(threadConnectToSessionServiceRegister, this, AppBase::instance()->getExternalServicesComponent(), SessionServiceURL.toStdString());
	t1.detach();
}

void LogInManager::slotSessionServiceChanged(void) {
	if (m_sessionService->text() == "<Edit>") {
		SessionSeviceEditDialog dia(m_entries);
		if (dia.showDialog() == resultOk) {
			m_entries = dia.entries();

			QJsonArray arr;
			for (auto e : m_entries) {
				QJsonObject obj;
				obj["Name"] = e.name();
				obj["IP"] = e.ip();
				obj["Port"] = e.port();
				arr.push_back(obj);
			}
			QJsonDocument doc(arr);
			uiAPI::settings::setByteArray("SessionServiceJSON", doc.toJson());

			rebuildSessionServiceSelection();
		}
		if (m_entries.empty()) {
			m_sessionService->setText("");
		}
		else {
			for (auto entry : m_entries) {
				if (m_lastUrl == entry.url()) {
					// Display the last entry before the edit
					m_lastUrl = entry.url();
					m_sessionService->setText(entry.displayText());
					return;
				}
			}
			// Display the first entry
			m_sessionService->setText(m_entries.at(0).displayText());
		}
	}
	else {
		for (auto entry : m_entries) {
			if (m_sessionService->text() == entry.displayText()) {
				m_lastUrl = entry.url();
				uiAPI::settings::setString("SessionServiceURL", entry.displayText());
				return;
			}
		}
		assert(0);
	}
}

void LogInManager::slotConnectToSessionServiceSuccess(const QString& _dbAndAuthUrl) {

	ot::JsonDocument doc;
	doc.fromJson(_dbAndAuthUrl.toStdString());
	std::string databaseUrl = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_DBURL);
	std::string authUrl = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_AUTHURL);

	LogInSessionServiceEntry entry = currentEntry();
	uiAPI::settings::setString("SessionServiceURL", entry.displayText());
#ifdef OT_USE_GSS
	AppBase::instance()->setGlobalSessionServiceURL(entry.url().toStdString());
#else
	AppBase::instance()->setSessionServiceURL(entry.url().toStdString());
#endif // OT_USE_GSS
	OT_LOG_I("SessionService URL successfully set to: " + entry.url().toStdString());
	std::thread t1(threadConnectToDatabase, this, AppBase::instance()->getExternalServicesComponent(), databaseUrl, authUrl, m_dialog->username().toStdString(), m_dialog->password().toStdString(), m_dialog->encryptedPassword(), &m_logInAttempt);
	t1.detach();
}

void LogInManager::slotConnectToSessionServiceRegisterSuccess(const QString& _dbAndAuthUrl) 
{
	ot::JsonDocument doc;
	doc.fromJson(_dbAndAuthUrl.toStdString());
	std::string databaseUrl = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_DBURL);
	std::string authUrl = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_AUTHURL);

	QMetaObject::invokeMethod(this, "slotConnectToDatabaseNewUser", Qt::QueuedConnection, Q_ARG(const QString&, databaseUrl.c_str()), Q_ARG(const QString&, authUrl.c_str()), Q_ARG(const QString&, m_dialog->username()), Q_ARG(const QString&, m_dialog->password()));
}

void LogInManager::slotConnectToSessionServiceFail(const QString& _message) {
	OT_LOG_E("Failed to connect to session service: " + _message.toStdString());

	m_dialog->hideWaitingAnimation();
	AppBase::instance()->showErrorPrompt(_message, "Connection error");
}

void LogInManager::slotConnectToDatabaseSuccess(const QString& _databaseUrl, const QString& _authURL, const QString& _userName, const QString& _password, const QString& _encryptedPassword) {
	OT_LOG_I("Connected to Database");

	AppBase::instance()->setDataBaseURL(_databaseUrl.toStdString());
	AppBase::instance()->setAuthorizationServiceURL(_authURL.toStdString());
	AppBase::instance()->setUserNamePassword(_userName.toStdString(), _password.toStdString(), _encryptedPassword.toStdString());

	// Store password
	if (m_dialog->savePassword())
	{
		uiAPI::settings::setString("LastUsername", _userName);
		uiAPI::settings::setString("LastPassword", _encryptedPassword);
	}

	m_dialog->hideWaitingAnimation();
	AppBase::instance()->logInSuccessfull();
	m_dialog->Close(resultOk);
}

void LogInManager::slotConnectToDatabaseFail(const QString& _message) {
	OT_LOG_E("Failed to connect to database: " + _message.toStdString());

	m_dialog->hideWaitingAnimation();
	AppBase::instance()->showErrorPrompt(_message, "Connection Failure");
}

void LogInManager::slotConnectToDatabaseNewUser(const QString& _databaseUrl, const QString& _authURL, const QString& _username, const QString& _hashedPassword) {
	m_dialog->hideWaitingAnimation();

	createNewAccountDialog newAccount;
	newAccount.setPassword(_hashedPassword);
	newAccount.exec();

	if (newAccount.wasConfirmed())
	{
		UserManagement uManager;
		uManager.setAuthServerURL(_authURL.toStdString());
		uManager.setDatabaseURL(_databaseUrl.toStdString());
		uManager.checkConnection();

		if (!uManager.addUser(_username.toStdString(), _hashedPassword.toStdString()))
		{
			OT_LOG_E("Failed to create new user");
			AppBase::instance()->showErrorPrompt("Your account could not be created (maybe the specified user name is already in use).", "Error");
		}
		else
		{
			OT_LOG_I("New user \"" + _username.toStdString() + "\" created");
			AppBase::instance()->showInfoPrompt("Your account has been created successfully. You can now log in with your new account.", "Success");
		}
	}
}

void LogInManager::slotCancelLogIn(void) {
	AppBase::instance()->cancelLogIn();
	m_dialog->Close(resultCancel);
}

// ####################################################################################################

// Private functions

void LogInManager::rebuildSessionServiceSelection(void) {
	m_sessionService->clearItems();

	for (auto entry : m_entries) {
		m_sessionService->addItem(entry.displayText());
	}

	m_sessionService->addItem("<Edit>");
}

// #######################################################################################################################################

// #######################################################################################################################################

// #######################################################################################################################################

SessionSeviceEditDialogEntry::SessionSeviceEditDialogEntry(SessionSeviceEditDialog * _owner)
	: m_owner(_owner), m_data("", "", ""), m_isError(false) { createControls(); }

SessionSeviceEditDialogEntry::SessionSeviceEditDialogEntry(SessionSeviceEditDialog * _owner, const LogInSessionServiceEntry& _data)
	: m_owner(_owner), m_data(_data), m_isError(false) { createControls(); }

SessionSeviceEditDialogEntry::~SessionSeviceEditDialogEntry() {
	delete m_name;
	delete m_ip;
	delete m_port;
	delete m_delete;
}

// ####################################################################################################

// Setter

void SessionSeviceEditDialogEntry::setErrorState(bool _error) {
	if (_error == m_isError) { return; }

	m_isError = _error;
	if (m_isError) {
		m_name->setStyleSheet("QLineEdit { color: #ff0000; }");
		m_ip->setStyleSheet("QLineEdit { color: #ff0000; }");
		m_port->setStyleSheet("QLineEdit { color: #ff0000; }");
	}
	else {
		m_name->setStyleSheet("QLineEdit { color: #000000; }");
		m_ip->setStyleSheet("QLineEdit { color: #000000; }");
		m_port->setStyleSheet("QLineEdit { color: #000000; }");
	}
}

// ####################################################################################################

// Slots

void SessionSeviceEditDialogEntry::slotItemClicked(QTableWidgetItem * _item) {
	if (_item == m_delete) {
		QMetaObject::invokeMethod(m_owner, "deleteItem", Qt::QueuedConnection, Q_ARG(int, m_delete->row()));
	}
}

void SessionSeviceEditDialogEntry::slotNameChanged(const QString& _text) {
	m_data.setName(m_name->text());
	checkErrorState();
}
void SessionSeviceEditDialogEntry::slotIpChanged(const QString& _text) {
	m_data.setIp(m_ip->text());
	checkErrorState();
}

void SessionSeviceEditDialogEntry::slotPortChanged(const QString& _text) {
	m_data.setPort(m_port->text());
	checkErrorState();
}

// ####################################################################################################

// Private functions

void SessionSeviceEditDialogEntry::checkErrorState(void) {
	setErrorState(!m_data.isValid());
}

void SessionSeviceEditDialogEntry::createControls(void) {
	ak::aTableWidget * table = m_owner->table();
	int r = table->rowCount();
	table->insertRow(r);
	
	m_name = new aLineEditWidget(m_data.name());
	m_name->setPlaceholderText("Entry name (e.g.: server name)");

	m_ip = new aLineEditWidget(m_data.ip());
	m_ip->setPlaceholderText("Service address (e.g.: 127.0.0.1)");
	
	m_port = new aLineEditWidget(m_data.port());
	m_port->setPlaceholderText("Service port (e.g.: 8091)");

	m_delete = new QTableWidgetItem(uiAPI::getIcon("Delete", "Table"), "");
	m_delete->setFlags(m_delete->flags() ^ Qt::ItemIsEditable ^ Qt::ItemIsSelectable);

	table->setCellWidget(r, tcName, m_name);
	table->setCellWidget(r, tcIP, m_ip);
	table->setCellWidget(r, tcPort, m_port);
	table->setItem(r, tcDelete, m_delete);

	checkErrorState();

	connect(table, &aTableWidget::itemClicked, this, &SessionSeviceEditDialogEntry::slotItemClicked);
	connect(m_name, &aLineEditWidget::textChanged, this, &SessionSeviceEditDialogEntry::slotNameChanged);
	connect(m_ip, &aLineEditWidget::textChanged, this, &SessionSeviceEditDialogEntry::slotIpChanged);
	connect(m_port, &aLineEditWidget::textChanged, this, &SessionSeviceEditDialogEntry::slotPortChanged);
}

// #######################################################################################################################################

// #######################################################################################################################################

// #######################################################################################################################################

SessionSeviceEditDialog::SessionSeviceEditDialog(const std::vector<LogInSessionServiceEntry>& _data) {
	// Create layouts
	m_centralLayout = new QVBoxLayout(this);

	m_buttonLayoutW = new QWidget;
	m_buttonLayout = new QHBoxLayout(m_buttonLayoutW);
	m_buttonLayout->setContentsMargins(0, 0, 0, 0);

	// Create controls
	m_btnAdd = new aPushButtonWidget("Add");
	m_btnClose = new aPushButtonWidget("Close");
	
	m_table = new aTableWidget(0, tcCount);

	// Setup controls
	{
		QTableWidgetItem * hItm = new QTableWidgetItem("Name");
		auto flags = hItm->flags();
		flags.setFlag(Qt::ItemFlag::ItemIsEditable, false);
		flags.setFlag(Qt::ItemFlag::ItemIsSelectable, false);
		hItm->setFlags(flags);
		m_table->setHorizontalHeaderItem(tcName, hItm);
	}
	{
		QTableWidgetItem * hItm = new QTableWidgetItem("IP");
		auto flags = hItm->flags();
		flags.setFlag(Qt::ItemFlag::ItemIsEditable, false);
		flags.setFlag(Qt::ItemFlag::ItemIsSelectable, false);
		hItm->setFlags(flags);
		m_table->setHorizontalHeaderItem(tcIP, hItm);
	}
	{
		QTableWidgetItem * hItm = new QTableWidgetItem("Port");
		auto flags = hItm->flags();
		flags.setFlag(Qt::ItemFlag::ItemIsEditable, false);
		flags.setFlag(Qt::ItemFlag::ItemIsSelectable, false);
		hItm->setFlags(flags);
		m_table->setHorizontalHeaderItem(tcPort, hItm);

	}
	{
		QTableWidgetItem * hItm = new QTableWidgetItem("");
		auto flags = hItm->flags();
		flags.setFlag(Qt::ItemFlag::ItemIsEditable, false);
		flags.setFlag(Qt::ItemFlag::ItemIsSelectable, false);
		hItm->setFlags(flags);
		m_table->setHorizontalHeaderItem(tcDelete, hItm);
	}
	m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	m_table->horizontalHeader()->setSectionResizeMode(tcDelete, QHeaderView::ResizeToContents);
	m_table->verticalHeader()->setVisible(false);

	// Setup layouts
	m_centralLayout->addWidget(m_buttonLayoutW);
	m_centralLayout->addWidget(m_table);

	m_buttonLayout->addStretch(1);
	m_buttonLayout->addWidget(m_btnAdd);
	m_buttonLayout->addWidget(m_btnClose);

	for (auto e : _data) {
		m_entries.push_back(new SessionSeviceEditDialogEntry(this, e));
	}

	setWindowTitle("Session service");
	hideInfoButton();
	setMinimumSize(600, 300);

	connect(m_btnAdd, &aPushButtonWidget::clicked, this, &SessionSeviceEditDialog::slotAdd);
	connect(m_btnClose, &aPushButtonWidget::clicked, this, &SessionSeviceEditDialog::slotClose);
}

SessionSeviceEditDialog::~SessionSeviceEditDialog() {
	m_entries.clear();
	delete m_table;
	delete m_btnAdd;
	delete m_btnClose;
	delete m_buttonLayout;
	delete m_buttonLayoutW;
	delete m_centralLayout;
}

// ####################################################################################################

// Getter

std::vector<LogInSessionServiceEntry> SessionSeviceEditDialog::entries(void) const {
	std::vector<LogInSessionServiceEntry> ret;

	for (auto e : m_entries) {
		ret.push_back(e->data());
	}

	return ret;
}

// ####################################################################################################

// Event handling

void SessionSeviceEditDialog::closeEvent(QCloseEvent * _event) {
	// Clear empty entries
	int i{ 0 };
	bool erased{ true };
	while (erased) {
		erased = false;
		for (; i < m_entries.size(); i++) {
			if (m_entries[i]->data().isEmpty()) {
				deleteItem(i);
				erased = true;
				break;
			}
		}
	}

	for (size_t it{ 0 }; it < m_entries.size(); it++) {
		if (!m_entries[it]->data().isValid() || m_entries[it]->isErrorState()) {
			if (uiAPI::promptDialog::show("You have invalid entries (highlighted in red). Do you wish to remove all invalid entries and continue?",
				"Invalid entries", promptYesNoIconLeft, "DialogWarning", "Default", this) == resultYes) {

				clearInvalidEntries();
				break;
			}
			else {
				_event->accept();
				return;
			}
		}
	}

	bool dupOk{ true };

	for (size_t i{ 0 }; i < m_entries.size(); i++) {
		for (size_t ii{ i + 1 }; ii < m_entries.size(); ii++) {
			if (m_entries[i]->data().url() == m_entries[ii]->data().url()) {
				dupOk = false;
				m_entries[i]->setErrorState(true);
				m_entries[ii]->setErrorState(true);
			}
		}
	}

	m_result = resultOk;
	aDialog::closeEvent(_event);
}

void SessionSeviceEditDialog::clearInvalidEntries(void) {
	bool erased{ true };
	size_t i{ 0 };
	while (erased) {
		erased = false;
		for (; i < m_entries.size(); i++) {
			if (!m_entries[i]->data().isValid() || m_entries[i]->isErrorState()) {
				delete m_entries[i];
				m_table->removeRow(i);
				erased = true;
				break;
			}
		}
	}
}

// ####################################################################################################

// Slots

void SessionSeviceEditDialog::slotAdd(void) {
	m_entries.push_back(new SessionSeviceEditDialogEntry(this));
}

void SessionSeviceEditDialog::slotClose(void) {
	Close(resultCancel);
}

void SessionSeviceEditDialog::deleteItem(int _logicalIndex) {
	if (_logicalIndex < 0 || _logicalIndex >= m_entries.size()) {
		assert(0);
		return;
	}

	SessionSeviceEditDialogEntry * entry = m_entries[_logicalIndex];
	if (entry == nullptr) {
		assert(0);
		return;
	}
	LogInSessionServiceEntry data = entry->data();
	if (!data.name().isEmpty() || !data.ip().isEmpty() || !data.port().isEmpty()) {
		QString msg{ "Do you really want to delete the session service (" };
		if (!data.name().isEmpty()) {
			msg.append(data.name());
			if (!data.ip().isEmpty()) { msg.append(": "); }
		}
		if (!data.ip().isEmpty()) {
			msg.append(data.ip());
			if (!data.port().isEmpty()) {
				msg.append(":").append(data.port());
			}
		}

		msg.append(")? This operation can not be undone.");
		if (uiAPI::promptDialog::show(msg, "Delete Entry", promptYesNoIconLeft, "DialogWarning", "Default", this) != resultYes) { return; }
	}

	delete m_entries[_logicalIndex];
	m_entries.erase(m_entries.begin() + _logicalIndex);
	m_table->removeRow(_logicalIndex);
}

void SessionSeviceEditDialog::slotItemChanged(void) {

}
