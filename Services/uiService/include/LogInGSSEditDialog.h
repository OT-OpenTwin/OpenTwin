// @otlicense

//! @file LogInGSSEditDialog.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Frontend header
#include "LogInGSSEntry.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/Dialog.h"

// std header
#include <vector>

class QTableWidget;
class QTableWidgetItem;
class LogInGSSEditDialog;

namespace ot {
	class Label;
	class LineEdit;
}

class LogInGSSEditDialogEntry : public QObject {
	Q_OBJECT
	OT_DECL_NOCOPY(LogInGSSEditDialogEntry)
	OT_DECL_NODEFAULT(LogInGSSEditDialogEntry)
public:
	LogInGSSEditDialogEntry(const LogInGSSEntry& _entry, LogInGSSEditDialog* _dialog);
	~LogInGSSEditDialogEntry();

	LogInGSSEntry createEntry(void) const;
	
	void setRow(int _row) { m_row = _row; };
	int getRow(void) const { return m_row; };
	bool getDataChanged(void) const { return m_dataChanged; };
	ot::LineEdit* getNameInput(void) const { return m_name; };
	ot::LineEdit* getUrlInput(void) const { return m_url; };
	ot::LineEdit* getPortInput(void) const { return m_port; };

	bool isEmpty(void) const;
	bool isValid(void) const;

	void prepareDestroy(void);

private Q_SLOTS:
	void slotDataChanged(void);
	void slotDeleteItem(QTableWidgetItem* _item);

private:
	void updateErrorState(void);
	bool isValidIpV4(void) const;
	bool isValidIpV6(void) const;
	bool isValidPort(void) const;

	int m_row;
	bool m_dataChanged;
	ot::LineEdit* m_name;
	ot::LineEdit* m_url;
	ot::LineEdit* m_port;
	QTableWidgetItem* m_delete;
	LogInGSSEditDialog* m_dialog;

};

class LogInGSSEditDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(LogInGSSEditDialog)
	OT_DECL_NODEFAULT(LogInGSSEditDialog)
public:
	LogInGSSEditDialog(const std::vector<LogInGSSEntry>& _entries);
	~LogInGSSEditDialog() {};

	std::vector<LogInGSSEntry> getEntries(void) const;

	bool hasChangedData(void) const;

private Q_SLOTS:
	void slotAdd(void);
	void slotSave(void);
	void slotDeleteEntry(int _row);

private:
	friend class LogInGSSEditDialogEntry;

	void addEntry(const LogInGSSEntry& _entry);

	void requestDeleteEntry(int _row);

	QTableWidget* getTable(void) const { return m_table; };

	enum class TableColumn {
		Name,
		Url,
		Port,
		Delete,
		TableColumnCount
	};

	bool m_dataChanged;
	QTableWidget* m_table;
	std::vector<LogInGSSEditDialogEntry*> m_entries;

};