// @otlicense
// File: Logging.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// API header
#include "OToolkitAPI/Tool.h"

// OT header
#include "OTCore/LogDispatcher.h"		// LogMessage
#include "OTCommunication/ActionHandler.h"

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

namespace ot { class ComboBox; }

class Logging : public QObject, public otoolkit::Tool, public ot::ActionHandler {
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

	void appendLogMessage(ot::LogMessage&& _msg);
	void appendLogMessages(std::list<ot::LogMessage>&& _messages);

	void newMessages(std::list<ot::LogMessage>&& _messages);

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
	void slotColumnWidthModeChanged(const QString& _text);
	void slotMessageLimitChanged(int _limit);
	void slotUseIntervalChanged();

	void slotIntervalTimeout();

private:
	void handleNewLog(ot::JsonDocument& _doc);

	void iniTableItem(int _row, int _column, QTableWidgetItem* _itm);
	void updateCountLabels();
	void connectToLogger(bool _isAutoConnect);
	bool disconnectFromLogger();
	void rebuildColumnWidthData();
	bool applyColumnWidthData(const std::list<int>& _widths);

	void resizeMessageBuffer(std::list<ot::LogMessage>& _buffer);
	void resizeNewMessageBuffer();

	// Data

	LogModeSetter* m_logModeSetter;
	LoggingFilterView* m_filterView;
	LogServiceDebugInfo* m_serviceDebugInfo;

	std::list<ot::LogMessage>	m_messages;
	int							m_errorCount;
	int							m_warningCount;

	QTimer*                     m_intervalTimer;
	std::list<ot::LogMessage>	m_newMessages;

	std::string                 m_loggerUrl;

	QString m_columnWidthTmp;
	QTimer* m_columnWidthTimer;

	ot::WidgetView* m_root;

	std::map<QString, std::list<int>> m_columnWidthData;

	ot::ComboBox* m_cellWidthMode;
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
