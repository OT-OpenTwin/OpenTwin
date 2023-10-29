//! @file LogVisualization.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// API header
#include "OToolkitAPI/Tool.h"

// OT header
#include "OpenTwinCore/Logger.h"		// LogMessage

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qlist.h>
#include <QtWidgets/qdialog.h>

class QSplitter;
class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QPushButton;
class QPlainTextEdit;
class QTableWidget;
class QTableWidgetItem;
class QLabel;
class QTimer;
class QCheckBox;
class QListWidget;
class QLineEdit;
class QAction;
class QShortcut;

class LogVisualization : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	// Static functions

	static QString logMessageTypeString(const ot::LogMessage& _msg);

	// ###########################################################################################################################################################################################################################################################################################################################

	LogVisualization();
	virtual ~LogVisualization();

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString toolName(void) const override;

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual ot::TabWidget* runTool(QMenu* _rootMenu) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(void) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Log Visualization

	void appendLogMessage(const ot::LogMessage& _msg);
	void appendLogMessages(const QList<ot::LogMessage>& _messages);

public slots:
	void slotConnect(void);
	void slotAutoConnect(void);
	void slotImport(void);
	void slotExport(void);

	void slotClear(void);
	void slotClearAll(void);
	void slotFilterChanged(void);
	void slotSelectAllServices(void);
	void slotDeselectAllServices(void);
	void slotAutoScrollToBottomChanged(void);
	void slotUpdateCheckboxColors(void);
	void slotToggleAutoConnect(void);
	void slotViewCellContent(QTableWidgetItem* _itm);

private:
	void iniTableItem(int _row, int _column, QTableWidgetItem * _itm);
	void checkEntryFilter(int _row);
	void updateCountLabels(void);
	void connectToLogger(bool _isAutoConnect);
	bool disconnectFromLogger(void);

	// Data

	std::list<ot::LogMessage>	m_messages;
	int							m_errorCount;
	int							m_warningCount;

	bool						m_filterLock;
	std::string                 m_loggerUrl;
	//bool                        m_isConnected;

	QTimer *					m_filterTimer;

	ot::TabWidget*              m_tabWidget;
	
	QCheckBox *					m_msgTypeFilterDetailed;
	QCheckBox *					m_msgTypeFilterInfo;
	QCheckBox *					m_msgTypeFilterWarning;
	QCheckBox *					m_msgTypeFilterError;
	QCheckBox *					m_msgTypeFilterMsgIn;
	QCheckBox *					m_msgTypeFilterMsgOut;

	QListWidget *				m_serviceFilter;
	
	QLineEdit *					m_messageFilter;
	QCheckBox *					m_ignoreNewMessages;
	QCheckBox *					m_autoScrollToBottom;
	QTableWidget *				m_table;

	// Status Bar
	QLabel *					m_errorCountLabel;
	QLabel *					m_warningCountLabel;
	QLabel *					m_messageCountLabel;

	// Tool Bar
	QAction *					m_connectButton;
	QAction*					m_autoConnect;
	QAction *					m_importButton;
	QAction *					m_exportButton;
};

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

class LogVisualizationItemViewDialog : public QDialog {
	Q_OBJECT
public:
	LogVisualizationItemViewDialog(const ot::LogMessage& _msg, size_t _index, QWidget * _parent);
	virtual ~LogVisualizationItemViewDialog();

	virtual void closeEvent(QCloseEvent* _event) override;

	virtual void mousePressEvent(QMouseEvent* _event) override;

	virtual bool eventFilter(QObject* _obj, QEvent* _event) override;

	virtual bool event(QEvent* _event) override;

private slots:
	void slotRecenter(void);
	void slotDisplayMessageText(int _state);

private:
	std::string findJsonSyntax(std::string _str);

	ot::LogMessage m_msg;

	QShortcut* m_closeShortcut;
	QShortcut* m_recenterShortcut;
	QVBoxLayout* m_centralLayout;
	QGridLayout* m_dataLayout;
	QVBoxLayout* m_bigVLayout;
	QHBoxLayout* m_messageTitleLayout;
	QHBoxLayout* m_buttonLayout;

	QLabel* m_timeL;
	QLineEdit* m_time;
	QLabel* m_timeLocalL;
	QLineEdit* m_timeLocal;
	QLabel* m_senderNameL;
	QLineEdit* m_senderName;
	QLabel* m_messageTypeL;
	QLineEdit* m_messageType;
	QCheckBox* m_findMessageSyntax;

	QLabel* m_functionL;
	QLineEdit* m_function;
	QLabel* m_messageL;
	QPlainTextEdit* m_message;

	QPushButton* m_okButton;
};