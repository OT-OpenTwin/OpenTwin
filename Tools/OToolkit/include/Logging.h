//! @file Logging.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// API header
#include "OToolkitAPI/Tool.h"

// OT header
#include "OTCore/Logger.h"		// LogMessage

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qlist.h>
#include <QtWidgets/qdialog.h>

class LogModeSetter;
class LoggingFilterView;

class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QCheckBox;
class QPushButton;
class QPlainTextEdit;
class QTableWidget;
class QTableWidgetItem;
class QLabel;

class QAction;
class QShortcut;

class Logging : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	// Static functions

	static QString logMessageTypeString(const ot::LogMessage& _msg);

	// ###########################################################################################################################################################################################################################################################################################################################

	Logging();
	virtual ~Logging();

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

	// Log Visualization

	void appendLogMessage(const ot::LogMessage& _msg);
	void appendLogMessages(const QList<ot::LogMessage>& _messages);

public Q_SLOTS:
	void slotConnect(void);
	void slotAutoConnect(void);
	void slotImport(void);
	void slotExport(void);

	void slotClear(void);
	void slotClearAll(void);
	void slotFilterChanged(void);
	void slotAutoScrollToBottomChanged(void);
	void slotUpdateCheckboxColors(void);
	void slotToggleAutoConnect(void);
	void slotViewCellContent(QTableWidgetItem* _itm);

private:
	void iniTableItem(int _row, int _column, QTableWidgetItem* _itm);
	void updateCountLabels(void);
	void connectToLogger(bool _isAutoConnect);
	bool disconnectFromLogger(void);

	// Data

	LogModeSetter* m_logModeSetter;
	LoggingFilterView* m_filterView;

	std::list<ot::LogMessage>	m_messages;
	int							m_errorCount;
	int							m_warningCount;

	std::string                 m_loggerUrl;

	ot::WidgetView* m_root;

	QCheckBox* m_ignoreNewMessages;
	QCheckBox* m_autoScrollToBottom;
	QTableWidget* m_table;

	// Status Bar
	QLabel* m_errorCountLabel;
	QLabel* m_warningCountLabel;
	QLabel* m_messageCountLabel;

	// Tool Bar
	QAction* m_connectButton;
	QAction* m_autoConnect;
	QAction* m_importButton;
	QAction* m_exportButton;
};
