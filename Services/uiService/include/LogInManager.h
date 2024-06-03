#pragma once

// uiCore header
#include <akCore/akCore.h>
#include <akGui/aDialog.h>

#include <qobject.h>

#include <vector>

namespace ak {
	class aLogInDialog;
	class aLabelWidget;
	class aComboButtonWidget;
	class aTableWidget;
	class aPushButtonWidget;
	class aLineEditWidget;
}

class QWidget;
class QCloseEvent;
class QVBoxLayout;
class QHBoxLayout;
class QTableWidgetItem;
class ExternalServicesComponent;
class SessionSeviceEditDialog;

class LogInSessionServiceEntry {
public:
	LogInSessionServiceEntry(const QString& _name, const QString& _ip, const QString& _port);
	LogInSessionServiceEntry(const LogInSessionServiceEntry& _other);
	virtual ~LogInSessionServiceEntry();
	LogInSessionServiceEntry& operator = (const LogInSessionServiceEntry& _other);

	// ####################################################################################################

	// Setter

	void setName(const QString& _name) { m_name = _name; }
	void setIp(const QString& _ip) { m_ip = _ip; }
	void setPort(const QString& _port) { m_port = _port; }

	// ####################################################################################################

	// Getter

	const QString& name(void) const { return m_name; }
	const QString& ip(void) const { return m_ip; }
	const QString& port(void) const { return m_port; }
	QString url(void) const;
	QString displayText(void) const;
	bool isValid(void) const;
	bool isEmpty(void) const;

private:
	QString		m_name;
	QString		m_ip;
	QString		m_port;

	LogInSessionServiceEntry() = delete;
};

// #######################################################################################################################################

// #######################################################################################################################################

// #######################################################################################################################################

class LogInManager : public QObject {
	Q_OBJECT
public:
	LogInManager();
	virtual ~LogInManager();

	LogInSessionServiceEntry currentEntry(void) const;
	QString username(void);
	QString password(void);

	bool showDialog(void);
	ak::aLogInDialog * dialog(void) { return m_dialog; }

	void hideWaitingAnimation(void);

public Q_SLOTS:
	void slotLogIn(void);
	void slotRegister(void);
	void slotSessionServiceChanged(void);
	void slotConnectToSessionServiceSuccess(const QString& _dbAndAuthUrl);
	void slotConnectToSessionServiceRegisterSuccess(const QString& _dbAndAuthUrl);
	void slotConnectToSessionServiceFail(const QString& _message);
	void slotConnectToDatabaseSuccess(const QString& _databaseUrl, const QString& _authURL, const QString& _userName, const QString& _password, const QString& _encryptedPassword, const QString& _sessionUser, const QString& _sessionPassword);
	void slotConnectToDatabaseFail(const QString& _message);
	void slotConnectToDatabaseNewUser(const QString& _databaseUrl, const QString& _authURL, const QString& _username, const QString& _hashedPassword);
	void slotCancelLogIn(void);

private:
	
	void rebuildSessionServiceSelection(void);

	std::vector<LogInSessionServiceEntry>	m_entries;
	bool									m_ignoreEvents;
	int										m_logInAttempt;
	ak::aLogInDialog *						m_dialog;
	ak::aComboButtonWidget *				m_sessionService;
	ak::aLabelWidget *						m_sessionServiceL;
	QString									m_lastUrl;

	LogInManager(LogInManager&) = delete;
	LogInManager& operator = (LogInManager&) = delete;
};

// #######################################################################################################################################

// #######################################################################################################################################

// #######################################################################################################################################

class SessionSeviceEditDialogEntry : public QObject {
	Q_OBJECT
public:
	SessionSeviceEditDialogEntry(SessionSeviceEditDialog * _owner);
	SessionSeviceEditDialogEntry(SessionSeviceEditDialog * _owner, const LogInSessionServiceEntry& _data);
	virtual ~SessionSeviceEditDialogEntry();

	// ####################################################################################################

	// Getter

	const LogInSessionServiceEntry& data(void) const { return m_data; }
	bool isErrorState(void) const { return m_isError; }

	// ####################################################################################################

	// Setter

	void setErrorState(bool _error);

	// ####################################################################################################

private Q_SLOTS:
	void slotItemClicked(QTableWidgetItem * _item);
	void slotNameChanged(const QString& _text);
	void slotIpChanged(const QString& _text);
	void slotPortChanged(const QString& _text);

	// ####################################################################################################

private:

	inline void checkErrorState(void);
	void createControls(void);

	bool							m_isError;
	LogInSessionServiceEntry		m_data;
	SessionSeviceEditDialog *		m_owner;
	ak::aLineEditWidget *			m_name;
	ak::aLineEditWidget *			m_ip;
	ak::aLineEditWidget *			m_port;
	QTableWidgetItem *				m_delete;
};

// #######################################################################################################################################

// #######################################################################################################################################

// #######################################################################################################################################

class SessionSeviceEditDialog : public ak::aDialog {
	Q_OBJECT
public:
	SessionSeviceEditDialog(const std::vector<LogInSessionServiceEntry>& _data);
	virtual ~SessionSeviceEditDialog();

	// ####################################################################################################

	ak::aTableWidget * table(void) { return m_table; }

	std::vector<LogInSessionServiceEntry> entries(void) const;

	// ####################################################################################################

	// Event handling

	virtual void closeEvent(QCloseEvent * _event) override;

	void clearInvalidEntries(void);

public Q_SLOTS:
	void slotAdd(void);
	void slotClose(void);
	void deleteItem(int _logicalIndex);
	void slotItemChanged(void);

private:
	std::vector<SessionSeviceEditDialogEntry *>	m_entries;

	QVBoxLayout *								m_centralLayout;

	QWidget *									m_buttonLayoutW;
	QHBoxLayout *								m_buttonLayout;
	
	ak::aPushButtonWidget *						m_btnAdd;
	ak::aPushButtonWidget *						m_btnClose;

	ak::aTableWidget *							m_table;

	SessionSeviceEditDialog() = delete;
	SessionSeviceEditDialog(SessionSeviceEditDialog&) = delete;
	SessionSeviceEditDialog& operator = (SessionSeviceEditDialog&) = delete;
};
