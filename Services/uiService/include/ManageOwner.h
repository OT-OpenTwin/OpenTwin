#pragma once

// uiCore header
#include <akCore/akCore.h>
#include <akGui/aColor.h>

#include "OTWidgets/Dialog.h"

#include <qobject.h>
#include <qtablewidget.h>

class QVBoxLayout;
class QHBoxLayout;

namespace ot {
	class Label;
	class LineEdit;
	class CheckBox;
	class PushButton;
}

class ManageOwnerTable : public QTableWidget {
	Q_OBJECT
public:
	ManageOwnerTable();
	ManageOwnerTable(int _row, int _column);
	virtual ~ManageOwnerTable();

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

class ManageOwner : public ot::Dialog {
	Q_OBJECT

public:
	ManageOwner(const std::string &authServerURL, const std::string &assetType, const std::string &assetName, const std::string &ownerName);
	virtual ~ManageOwner();

public Q_SLOTS:
	void slotClose(void);
	void slotShowGroupsWithAccessOnly(void);
	void slotGroupsFilter(void);
	void slotGroupsSelection(void);
	virtual void slotGroupCheckBoxChanged(bool state, int row) = 0;

protected:
	void fillOwnerList(void);
	std::string tolower(std::string s);
	bool hasSuccessful(const std::string &response);
	bool hasError(const std::string &response);
	void readUserList(void);

	QVBoxLayout *								m_centralLayout;
	QHBoxLayout *								m_groupLabelLayout;
	QHBoxLayout *								m_buttonLabelLayout;

	QWidget *									m_groupLabelLayoutW;
	QWidget *									m_buttonLabelLayoutW;

	ot::PushButton*								m_btnClose;
	ot::Label *									m_labelGroups;
	ot::LineEdit *								m_filterGroups;
	ManageOwnerTable *							m_ownersList;

	std::string									m_authServerURL;
	std::string									m_assetType;
	std::string									m_assetName;
	std::string									m_assetOwner;
	ot::CheckBox*								m_ownerCheckBox;

	std::list<std::string>						m_userList;

	ManageOwner() = delete;
	ManageOwner(ManageOwner&) = delete;
	ManageOwner& operator = (ManageOwner&) = delete;
};

class ManageGroupOwner : public ManageOwner {
	Q_OBJECT

public:
	ManageGroupOwner(const std::string &authServerURL, const std::string &assetName, const std::string &ownerName) : ManageOwner(authServerURL, "Group", assetName, ownerName) {};
	virtual ~ManageGroupOwner() {};

	virtual void slotGroupCheckBoxChanged(bool state, int row);
};

class ManageProjectOwner : public ManageOwner {
	Q_OBJECT

public:
	ManageProjectOwner(const std::string &authServerURL, const std::string &assetName, const std::string &ownerName) : ManageOwner(authServerURL, "Project", assetName, ownerName) {};
	virtual ~ManageProjectOwner() {};

	virtual void slotGroupCheckBoxChanged(bool state, int row);
};

