#pragma once

// Toolkit header
#include "AbstractTool.h"

// OT header
#include "OpenTwinCommunication/CommunicationTypes.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qlist.h>
#include <QtCore/qjsonobject.h>
#include <QtWidgets/qtreewidget.h>

class Terminal;
class JSONEditor;

class QSplitter;
class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QComboBox;
class QLineEdit;
class QGroupBox;
class QLabel;
class QPushButton;
class QProgressBar;
class QShortcut;

class TerminalCollectionItem : public QTreeWidgetItem {
public:
	TerminalCollectionItem(Terminal * _owner, const QString& _title);
	virtual ~TerminalCollectionItem();

	static TerminalCollectionItem * createFromJsonObject(Terminal * _owner, const QJsonObject& _object);

	virtual bool setFromJsonObject(const QJsonObject& _object) = 0;
	virtual void addToJsonObject(QJsonObject& _object) const = 0;

	void setTitle(const QString& _title);
	const QString& title(void) const { return m_title; };

	Terminal * ownerTerminal(void) { return m_owner; };

private:
	Terminal *		m_owner;
	QString			m_title;

	TerminalCollectionItem() = delete;
	TerminalCollectionItem(TerminalCollectionItem&) = delete;
	TerminalCollectionItem& operator = (TerminalCollectionItem&) = delete;
};

// #####################################################################################################################################################################################

// #####################################################################################################################################################################################

// #####################################################################################################################################################################################

class TerminalCollectionFilter : public TerminalCollectionItem {
public:
	TerminalCollectionFilter(Terminal * _owner, const QString& _title);
	virtual ~TerminalCollectionFilter();

	virtual bool setFromJsonObject(const QJsonObject& _object) override;
	virtual void addToJsonObject(QJsonObject& _object) const override;

	bool hasDirectChildWith(const QString& _title) const;
	bool hasAnyChildWith(const QString& _title) const;
	bool hasChild(TerminalCollectionItem * _item) const;

private:
	
	TerminalCollectionFilter() = delete;
	TerminalCollectionFilter(TerminalCollectionFilter&) = delete;
	TerminalCollectionFilter& operator = (TerminalCollectionFilter&) = delete;
};

// #####################################################################################################################################################################################

// #####################################################################################################################################################################################

// #####################################################################################################################################################################################

class TerminalRequest : public TerminalCollectionItem {
public:
	TerminalRequest(Terminal * _owner, const QString& _title);
	virtual ~TerminalRequest(void);

	virtual bool setFromJsonObject(const QJsonObject& _object) override;
	virtual void addToJsonObject(QJsonObject& _object) const override;

	void setUrl(const QString& _url) { m_url = _url; };
	const QString& url(void) const { return m_url; };

	void setMessageBody(const QString& _messageBody) { m_messageBody = _messageBody; };
	const QString& messageBody(void) const { return m_messageBody; };

	void setEndpoint(ot::MessageType _endpoint) { m_endpoint = _endpoint; };
	ot::MessageType endpoint(void) const { return m_endpoint; };

private:
	QString				m_url;
	QString				m_messageBody;
	ot::MessageType		m_endpoint;

	TerminalRequest() = delete;
	TerminalRequest(TerminalRequest&) = delete;
	TerminalRequest& operator = (TerminalRequest&) = delete;
};

// #####################################################################################################################################################################################

// #####################################################################################################################################################################################

// #####################################################################################################################################################################################

class Terminal : public QObject, public OToolkitAPI::AbstractTool {
	Q_OBJECT
public:
	Terminal();
	virtual ~Terminal();

	//! @brief Returns the unique name of this tool
	//! @note If another tool with the same name was registered before, the instance of this tool will be destroyed
	virtual QString toolName(void) const override;

	QWidget * widget(void);

	void notifyItemDeleted(TerminalCollectionItem * _item);

	void setEndpointFromMessageType(ot::MessageType _type);
	ot::MessageType endpointToMessageType(void) const;

	// ################################################################################################################################

	// Private: Slots

private slots:
	void slotSendMessage(void);
	void slotMessageSendSuccessful(const QByteArray& _response);
	void slotMessageSendFailed(const QString& _errorString);
	void slotServiceNameChanged(void);
	void slotShowNavigationContextMenu(const QPoint& _pt);
	void slotNavigationItemDoubleClicked(QTreeWidgetItem* _item, int _column);
	void slotNavigationItemChanged(QTreeWidgetItem* _item, int _column);

	void slotLoadRequestCollection(void);
	void slotSaveRequestCollection(void);

private:
	// ################################################################################################################################

	// Private: Helper

	void initializeServices(void);
	void initializeDefaultServices(void);
	void setWaitingMode(bool _isWaiting);
	
	// ################################################################################################################################

	// Private: Context Menu handling

	void handleContextFilter(const QPoint& _pt, TerminalCollectionFilter * _filter);
	void handleContextRequest(const QPoint& _pt, TerminalRequest * _request);
	
	void addNewFilter(TerminalCollectionFilter * _parentFilter);
	void removeFilter(TerminalCollectionFilter * _filter);

	void addNewRequestFromCurrent(TerminalCollectionFilter * _parentFilter);
	void updateRequestFromCurrent(TerminalRequest * _request);
	void removeRequest(TerminalRequest * _request);
	void applyRequest(TerminalRequest* _request);
	void applyAndSendRequest(TerminalRequest* _request);

	// ################################################################################################################################

	// Private: Async worker functions

	void workerSendMessage(const std::string& _receiverUrl, ot::MessageType _messageType, const QByteArray& _data);

	struct ServiceInfo {
		QString name;
		QString shortName;
		QString url;
	};

	// Data
	QList<ServiceInfo>			m_services;
	TerminalCollectionFilter *	m_requestsRootFilter;
	bool						m_exportLock;

	// Layouts

	QSplitter *			m_splitter;

	QWidget *			m_leftLayoutW;
	QVBoxLayout *		m_leftLayout;

	QGridLayout *		m_buttonLayout;

	QHBoxLayout *		m_navigationLayout;
	QTreeWidget *		m_navigation;

	QWidget *			m_rightLayoutW;
	QVBoxLayout *		m_rightLayout;
	QSplitter *			m_rightSplitter;
	
	QGroupBox *			m_receiverBox;
	QGridLayout *		m_receiverLayout;
	
	QGroupBox *			m_messageBox;
	QVBoxLayout *		m_messageLayout;

	QGroupBox *			m_responseBox;
	QVBoxLayout *		m_responseLayout;
	
	// Controls

	QLabel *			m_receiverNameL;
	QComboBox *			m_receiverName;
	QLabel *			m_receiverUrlL;
	QLineEdit *			m_receiverUrl;

	QLabel *			m_endpointL;
	QComboBox *			m_endpoint;

	QPushButton *		m_btnSend;

	JSONEditor *		m_messageEdit;

	JSONEditor *		m_responseEdit;
	QLabel *			m_responseLength;

	QProgressBar *		m_progressBar;

	// Shortcuts

	QShortcut* m_shortcutSave;
	QShortcut* m_shortcutSend;
	QShortcut* m_shortcutRename;

};