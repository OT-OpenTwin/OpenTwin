// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Table.h"
#include "OTWidgets/Dialog.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot { class LineEdit; }
namespace ot { class CheckBox; }

class ManageAccessTable : public ot::Table {
	Q_OBJECT
public:
	ManageAccessTable();
	ManageAccessTable(int _row, int _column);
	virtual ~ManageAccessTable();

	void addRow(const std::array<QTableWidgetItem *, 2> &_columns);

	void Clear(void);

	virtual void mouseMoveEvent(QMouseEvent * _event) override;

	virtual void leaveEvent(QEvent * _event) override;

	void getSelectedItems(QTableWidgetItem *&first, QTableWidgetItem *&second);

Q_SIGNALS:
	void selectionChanged();

private Q_SLOTS:
	void slotSelectionChanged();

private:
	std::vector<std::array<QTableWidgetItem *, 2>>	my_dataRowItems;

	int				my_selectedRow;
};

class ManageAccess : public ot::Dialog {
	Q_OBJECT

public:
	ManageAccess(const std::string &authServerURL, const std::string &projectName);
	virtual ~ManageAccess();

public Q_SLOTS:
	void slotShowGroupsWithAccessOnly(void);
	void slotGroupsFilter(void);
	void slotGroupsSelection(void);
	void slotGroupCheckBoxChanged(bool state, int row);

private:
	void fillGroupsList(void);
	std::string tolower(std::string s);
	bool hasSuccessful(const std::string &response);
	void readGroupsList(void);

	ot::LineEdit*								m_filterGroups;
	ManageAccessTable *							m_groupsList;
	ot::CheckBox *								m_showGroupsWithAccessOnly;

	std::string									m_authServerURL;
	std::string									m_projectName;

	std::list<std::string>						m_groupList;
	std::map<std::string, bool>					m_groupHasAccess;

	ManageAccess() = delete;
	ManageAccess(ManageAccess&) = delete;
	ManageAccess& operator = (ManageAccess&) = delete;
};
