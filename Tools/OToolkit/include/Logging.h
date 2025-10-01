//! @file Logging.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// API header
#include "OToolkitAPI/Tool.h"

// OT header
#include "OTCore/LogDispatcher.h"		// LogMessage

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qlist.h>
#include <QtWidgets/qdialog.h>

// std header
#include <list>

class LogModeSetter;
class LoggingFilterView;
class LogServiceDebugInfo;

class QTimer;
class QLabel;
class QAction;
class QWidget;
class QShortcut;
class QGroupBox;
class QCheckBox;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QTableWidget;
class QPlainTextEdit;
class QTableWidgetItem;


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
	virtual QString getToolName() const override;

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	virtual void restoreToolSettings(QSettings& _settings) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(QSettings& _settings) override;

	virtual bool eventFilter(QObject* _obj, QEvent* _event) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Log Visualization

	void appendLogMessage(const ot::LogMessage& _msg);
	void appendLogMessages(const std::list<ot::LogMessage>& _messages);

public Q_SLOTS:
	void slotUpdateColumnWidth();

	void slotImportFileLogs();
	void runQuickExport();
	void slotConnect();
	void slotAutoConnect();
	void slotImport();
	void slotExport();

	void slotRefillData();
	void slotClear();
	void slotClearAll();
	void slotFilterChanged();
	void slotAutoScrollToBottomChanged();
	void slotUpdateCheckboxColors();
	void slotToggleAutoConnect();
	void slotViewCellContent(QTableWidgetItem* _itm);
	void slotScrollToItem(int _row);

private:
	void iniTableItem(int _row, int _column, QTableWidgetItem* _itm);
	void updateCountLabels();
	void connectToLogger(bool _isAutoConnect);
	bool disconnectFromLogger();

	// Data

	LogModeSetter* m_logModeSetter;
	LoggingFilterView* m_filterView;
	LogServiceDebugInfo* m_serviceDebugInfo;

	std::list<ot::LogMessage>	m_messages;
	int							m_errorCount;
	int							m_warningCount;

	std::string                 m_loggerUrl;

	QString m_columnWidthTmp;
	QTimer* m_columnWidthTimer;

	ot::WidgetView* m_root;

	QCheckBox* m_convertToLocalTime;
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
