//! @file Terminal.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// API header
#include "OToolkitAPI/Tool.h"

// OT header
#include "OTCommunication/CommunicationTypes.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qlist.h>
#include <QtCore/qjsonobject.h>
#include <QtWidgets/qtreewidget.h>
#include <QtWidgets/qdialog.h>

class Terminal;
class JSONEditor;

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

namespace ot { class Splitter; }

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

	bool merge(TerminalCollectionFilter* _newData, bool _isFirst);

private:
	bool createChildFromMerge(TerminalCollectionItem* _itemToMerge);

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

class Terminal : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	Terminal();
	virtual ~Terminal();

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString toolName(void) const override;

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	virtual void restoreToolSettings(QSettings& _settings) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(QSettings& _settings) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	void notifyItemDeleted(TerminalCollectionItem * _item);

	void setEndpointFromMessageType(ot::MessageType _type);
	ot::MessageType endpointToMessageType(void) const;

	// ################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void slotSendMessage(void);
	void slotMessageSendSuccessful(const QByteArray& _response);
	void slotMessageSendFailed(const QString& _errorString);
	void slotServiceNameChanged(void);
	void slotSelectionChanged(void);
	void slotShowNavigationContextMenu(const QPoint& _pt);
	void slotNavigationItemDoubleClicked(QTreeWidgetItem* _item, int _column);
	void slotNavigationItemChanged(QTreeWidgetItem* _item, int _column);
	void slotUpdateCurrent(void);
	void slotRenameCurrent(void);
	void slotDeleteCurrent(void);
	void slotCloneCurrent(void);

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

	void exportToFile(TerminalCollectionFilter* _filter);
	void importFromFile(TerminalCollectionFilter* _filter);

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

	ot::WidgetView*     m_root;

	QWidget *			m_leftLayoutW;
	QVBoxLayout *		m_leftLayout;

	QGridLayout *		m_buttonLayout;

	QHBoxLayout *		m_navigationLayout;
	QTreeWidget *		m_navigation;

	QWidget *			m_rightLayoutW;
	QVBoxLayout *		m_rightLayout;
	ot::Splitter*		m_rightSplitter;
	
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
	QShortcut* m_shortcutDelete;
	QShortcut* m_shortcutClone;

};
