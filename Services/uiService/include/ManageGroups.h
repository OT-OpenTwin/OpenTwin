// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/Table.h"
#include "OTWidgets/Dialog.h"

namespace ot { class LineEdit; }
namespace ot { class CheckBox; }
namespace ot { class PushButton; }

class ManageGroupsTable : public ot::Table {
	Q_OBJECT
	OT_DECL_NOCOPY(ManageGroupsTable)
	OT_DECL_NODEFAULT(ManageGroupsTable)
public:
	ManageGroupsTable(int _row, int _column);
	virtual ~ManageGroupsTable();

	void addRow(const std::array<QTableWidgetItem *, 2>& _columns);

	void Clear(void);

	virtual void mouseMoveEvent(QMouseEvent* _event) override;

	virtual void leaveEvent(QEvent* _event) override;

	void getSelectedItems(QTableWidgetItem*& _first, QTableWidgetItem*& _second);

Q_SIGNALS:
	void selectionHasChanged();

private Q_SLOTS:
	void slotSelectionChanged();

private:
	std::vector<std::array<QTableWidgetItem *, 2>>	m_dataRowItems;

	int m_selectedRow;

};

class AddGroupDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(AddGroupDialog)
	OT_DECL_NODEFAULT(AddGroupDialog)
public:
	AddGroupDialog(const std::string &authServerURL);
	virtual ~AddGroupDialog();

	QString groupName(void) const;

private Q_SLOTS:
	void slotConfirm();

private:
	bool hasError(const std::string &response);
	
	ot::LineEdit* m_input;
	std::string m_authServerURL;
};

class RenameGroupDialog : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(RenameGroupDialog)
	OT_DECL_NODEFAULT(RenameGroupDialog)
public:
	RenameGroupDialog(const std::string &groupName, const std::string &authServerURL);
	virtual ~RenameGroupDialog();

	QString groupName(void) const;

Q_SIGNALS:
	void isClosing(void);

private Q_SLOTS:
	void slotConfirm();

private:
	bool hasSuccessful(const std::string& _response);

	bool			m_confirmed;
	bool			m_cancelClose;
	std::string     m_groupToRename;

	ot::LineEdit*	m_input;

	std::string		m_authServerURL;
};

class ManageGroups : public ot::Dialog {
	Q_OBJECT

public:
	ManageGroups(const std::string &authServerURL);
	virtual ~ManageGroups();

public Q_SLOTS:
	void slotAddGroup(void);
	void slotRenameGroup(void);
	void slotChangeGroupOwner(void);
	void slotDeleteGroup(void);
	void slotMemberCheckBoxChanged(bool state, int row);
	void slotFillGroupsList(void);
	void slotFillMembersList(void);

private:
	void readUserList(void);
	bool hasSuccessful(const std::string &response);

	ot::LineEdit*								m_filterGroups;
	ot::LineEdit*								m_filterMembers;
	ManageGroupsTable *							m_groupsList;
	ManageGroupsTable *							m_membersList;
	ot::CheckBox*								m_showMembersOnly;

	ot::PushButton*								m_btnRename;
	ot::PushButton*								m_btnOwner;
	ot::PushButton*								m_btnDelete;

	std::string									m_authServerURL;

	std::list<std::string>						m_userList;
	std::map<std::string, bool>					m_userInGroup;

	ManageGroups() = delete;
	ManageGroups(ManageGroups&) = delete;
	ManageGroups& operator = (ManageGroups&) = delete;
};

