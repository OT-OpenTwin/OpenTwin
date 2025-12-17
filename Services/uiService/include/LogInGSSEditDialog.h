// @otlicense
// File: LogInGSSEditDialog.h
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

// Frontend header
#include "LogInGSSEntry.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/Dialog.h"

// Qt header
#include <QtWidgets/qheaderview.h>

// std header
#include <vector>

class QTableWidget;
class QTableWidgetItem;
class LogInGSSEditDialog;

namespace ot {
	class Label;
	class LineEdit;
}

class LogInGSSEditDialogTableHeader : public QHeaderView {
	OT_DECL_NOCOPY(LogInGSSEditDialogTableHeader)
	OT_DECL_NOMOVE(LogInGSSEditDialogTableHeader)
	OT_DECL_NODEFAULT(LogInGSSEditDialogTableHeader)
public:
	LogInGSSEditDialogTableHeader(QWidget* _parent);

protected:
	QSize sectionSizeFromContents(int _logicalIndex) const override;
};

class LogInGSSEditDialogEntry : public QObject {
	Q_OBJECT
	OT_DECL_NOCOPY(LogInGSSEditDialogEntry)
	OT_DECL_NOMOVE(LogInGSSEditDialogEntry)
	OT_DECL_NODEFAULT(LogInGSSEditDialogEntry)
public:
	explicit LogInGSSEditDialogEntry(const LogInGSSEntry& _entry, LogInGSSEditDialog* _dialog);
	virtual ~LogInGSSEditDialogEntry();

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
	OT_DECL_NOMOVE(LogInGSSEditDialog)
	OT_DECL_NODEFAULT(LogInGSSEditDialog)
public:
	enum class TableColumn {
		Name,
		Url,
		Port,
		Delete,
		TableColumnCount
	};

	explicit LogInGSSEditDialog(const std::vector<LogInGSSEntry>& _entries, QWidget* _parent);
	virtual ~LogInGSSEditDialog() {};

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

	bool m_dataChanged;
	QTableWidget* m_table;
	std::vector<LogInGSSEditDialogEntry*> m_entries;

};