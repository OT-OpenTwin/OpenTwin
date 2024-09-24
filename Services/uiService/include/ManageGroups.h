#pragma once

// uiCore header
#include <akCore/akCore.h>
#include <akGui/aDialog.h>
#include <akGui/aColor.h>

#include <qobject.h>
#include <qtablewidget.h>

#include "OldWelcomeScreen.h"

namespace ak {
	class aLogInDialog;
	class aLabelWidget;
	class aComboButtonWidget;
	class aTableWidget;
	class aPushButtonWidget;
	class aLineEditWidget;
	class aCheckBoxWidget;
	class aPropertyGridWidget;
}

class ManageGroupsTable : public QTableWidget {
	Q_OBJECT
public:
	ManageGroupsTable();
	ManageGroupsTable(int _row, int _column);
	virtual ~ManageGroupsTable();

	void addRow(
		const std::array<QTableWidgetItem *, 2> &				_columns
	);

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

class QVBoxLayout;
class QHBoxLayout;

class addGroupDialog : public QDialog {
	Q_OBJECT
public:
	addGroupDialog(const std::string &authServerURL);
	virtual ~addGroupDialog();

	inline QString groupName(void) const;

	bool wasConfirmed(void) const { return my_confirmed; }

Q_SIGNALS:
	void isClosing(void);

private Q_SLOTS:
	void slotButtonConfirmPressed();
	void slotButtonCancelPressed();
	void slotReturnPressed();

private:
	bool hasError(const std::string &response);
	void Close(void);

	bool			my_confirmed;

	ak::aPushButtonWidget *	my_buttonConfirm;
	ak::aPushButtonWidget *	my_buttonCancel;
	ak::aLineEditWidget *	my_input;
	ak::aLabelWidget *		my_label;

	QVBoxLayout *	my_layout;

	QHBoxLayout *	my_layoutButtons;
	QWidget *		my_widgetButtons;

	QHBoxLayout *	my_layoutInput;
	QWidget *		my_widgetInput;

	std::string		m_authServerURL;

	addGroupDialog() = delete;
	addGroupDialog(const addGroupDialog &) = delete;
	addGroupDialog & operator = (addGroupDialog &) = delete;
};

class renameGroupDialog : public QDialog {
	Q_OBJECT
public:
	renameGroupDialog(const std::string &groupName, const std::string &authServerURL);
	virtual ~renameGroupDialog();

	inline QString groupName(void) const;

	bool wasConfirmed(void) const { return my_confirmed; }

Q_SIGNALS:
	void isClosing(void);

private Q_SLOTS:
	void slotButtonConfirmPressed();
	void slotButtonCancelPressed();
	void slotReturnPressed();

private:
	bool hasSuccessful(const std::string &response);
	void Close(void);

	bool			my_confirmed;
	bool			my_cancelClose;
	std::string     my_groupToRename;

	ak::aPushButtonWidget *	my_buttonConfirm;
	ak::aPushButtonWidget *	my_buttonCancel;
	ak::aLineEditWidget *	my_input;
	ak::aLabelWidget *		my_label;

	QVBoxLayout *	my_layout;

	QHBoxLayout *	my_layoutButtons;
	QWidget *		my_widgetButtons;

	QHBoxLayout *	my_layoutInput;
	QWidget *		my_widgetInput;

	std::string		m_authServerURL;

	renameGroupDialog() = delete;
	renameGroupDialog(const renameGroupDialog &) = delete;
	renameGroupDialog & operator = (renameGroupDialog &) = delete;
};

class ManageGroups : public ak::aDialog {
	Q_OBJECT

public:
	ManageGroups(const std::string &authServerURL);
	virtual ~ManageGroups();

	// ####################################################################################################

//	ak::aTableWidget * table(void) { return m_table; }

	// ####################################################################################################

	// Event handling

	//virtual void closeEvent(QCloseEvent * _event) override;

	//void clearInvalidEntries(void);

public Q_SLOTS:
	void slotClose(void);
	void slotAddGroup(void);
	void slotRenameGroup(void);
	void slotChangeGroupOwner(void);
	void slotDeleteGroup(void);
	void slotShowMembersOnly(void);
	void slotGroupFilter(void);
	void slotMemberFilter(void);
	void slotGroupsSelection(void);
	void slotMembersSelection(void);
	void slotMemberCheckBoxChanged(bool state, int row);

private:
	void fillGroupsList(void);
	void fillMembersList(void);
	void readUserList(void);
	std::string tolower(std::string s);
	bool hasSuccessful(const std::string &response);

	QVBoxLayout *								m_centralLayout;
	QHBoxLayout *								m_listFrameLayout;
	QVBoxLayout *								m_listLeftLayout;
	QVBoxLayout *								m_listRightLayout;
	QHBoxLayout *								m_groupLabelLayout;
	QHBoxLayout *								m_memberLabelLayout;
	QHBoxLayout *								m_buttonLabelLayout;

	QWidget *									m_listFrameLayoutW;
	QWidget *									m_listLeftLayoutW;
	QWidget *									m_listRightLayoutW;
	QWidget *									m_groupLabelLayoutW;
	QWidget *									m_memberLabelLayoutW;
	QWidget *									m_buttonLabelLayoutW;

	ak::aPushButtonWidget *						m_btnClose;
	ak::aPushButtonWidget *						m_btnAdd;
	ak::aPushButtonWidget *						m_btnRename;
	ak::aPushButtonWidget *						m_btnOwner;
	ak::aPushButtonWidget *						m_btnDelete;
	ak::aLabelWidget *							m_labelGroups;
	ak::aLabelWidget *							m_labelMembers;
	ak::aLineEditWidget *						m_filterGroups;
	ak::aLineEditWidget *						m_filterMembers;
	ManageGroupsTable *							m_groupsList;
	ManageGroupsTable *							m_membersList;
	ak::aCheckBoxWidget *						m_showMembersOnly;

	std::string									m_authServerURL;

	std::list<std::string>						m_userList;
	std::map<std::string, bool>					m_userInGroup;

	ManageGroups() = delete;
	ManageGroups(ManageGroups&) = delete;
	ManageGroups& operator = (ManageGroups&) = delete;
};

