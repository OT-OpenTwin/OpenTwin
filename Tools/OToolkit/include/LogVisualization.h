#pragma once

// Toolkit header
#include "AbstractTool.h"

// OT header
#include "OpenTwinCore/Logger.h"		// LogMessage

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qlist.h>

class QSplitter;
class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QLabel;
class QTimer;
class QCheckBox;
class QListWidget;
class QLineEdit;
class QAction;

class LogVisualization : public QObject, public OToolkitAPI::AbstractTool {
	Q_OBJECT
public:
	LogVisualization();
	virtual ~LogVisualization();

	//! @brief Returns the unique name of this tool
	//! @note If another tool with the same name was registered before, the instance of this tool will be destroyed
	virtual QString toolName(void) const override;

	virtual QList<QWidget *> statusBarWidgets(void) const override;

	virtual void createMenuBarEntries(QMenuBar * _menuBar) override;

	void appendLogMessage(const ot::LogMessage& _msg);
	void appendLogMessages(const QList<ot::LogMessage>& _messages);

	QWidget * widget(void);

public slots:
	void slotConnect(void);

	void slotClear(void);
	void slotClearAll(void);
	void slotFilterChanged(void);
	void slotSelectAllServices(void);
	void slotDeselectAllServices(void);
	void slotAutoScrollToBottomChanged(void);
	void slotUpdateCheckboxColors(void);

private:
	void iniTableItem(int _row, int _column, QTableWidgetItem * _itm);
	void checkEntryFilter(int _row);
	void updateCountLabels(void);

	// Data

	std::list<ot::LogMessage>	m_messages;
	int							m_errorCount;
	int							m_warningCount;

	bool						m_filterLock;

	QTimer *					m_filterTimer;

	QSplitter *					m_splitter;

	QWidget *					m_centralLayoutW;
	QVBoxLayout *				m_centralLayout;

	QHBoxLayout *				m_filterLayout;
	QVBoxLayout *				m_filterByMessageTypeLayout;
	QGroupBox *					m_filterByMessageTypeBox;
	QVBoxLayout *				m_filterByServiceLayout;
	QGroupBox *					m_filterByServiceBox;

	QCheckBox *					m_msgTypeFilterDetailed;
	QCheckBox *					m_msgTypeFilterInfo;
	QCheckBox *					m_msgTypeFilterWarning;
	QCheckBox *					m_msgTypeFilterError;
	QCheckBox *					m_msgTypeFilterMsgIn;
	QCheckBox *					m_msgTypeFilterMsgOut;

	QListWidget *				m_serviceFilter;
	QPushButton *				m_btnSelectAllServices;
	QPushButton *				m_btnDeselectAllServices;

	QHBoxLayout *				m_buttonLayout;

	QLabel *					m_messageFilterL;
	QLineEdit *					m_messageFilter;
	QCheckBox *					m_ignoreNewMessages;
	QCheckBox *					m_autoScrollToBottom;
	QPushButton *				m_btnClear;
	QPushButton *				m_btnClearAll;
	QTableWidget *				m_table;

	// Status Bar
	QLabel *					m_errorCountLabel;
	QLabel *					m_warningCountLabel;
	QLabel *					m_messageCountLabel;

	// Tool Bar
	QAction *					m_connectButton;
};