#pragma once

// uiCore header
#include <akCore/akCore.h>
#include <akGui/aDialog.h>
#include <akGui/aColor.h>

#include <qobject.h>
#include <qtablewidget.h>

#include "welcomeScreen.h"

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

class ManageAccessTable : public QTableWidget {
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

	QColor			my_colorBack;
	QColor			my_colorFront;
	QColor			my_colorFocusBack;
	QColor			my_colorFocusFront;
	QColor			my_colorSelectedBack;
	QColor			my_colorSelectedFront;

	int				my_selectedRow;
};

class QVBoxLayout;
class QHBoxLayout;

class ManageAccess : public ak::aDialog {
	Q_OBJECT

public:
	ManageAccess(const std::string &authServerURL, const std::string &projectName);
	virtual ~ManageAccess();

public Q_SLOTS:
	void slotClose(void);
	void slotShowGroupsWithAccessOnly(void);
	void slotGroupsFilter(void);
	void slotGroupsSelection(void);
	void slotGroupCheckBoxChanged(bool state, int row);

private:
	void fillGroupsList(void);
	std::string tolower(std::string s);
	bool hasSuccessful(const std::string &response);
	void readGroupsList(void);

	QVBoxLayout *								m_centralLayout;
	QHBoxLayout *								m_groupLabelLayout;
	QHBoxLayout *								m_buttonLabelLayout;

	QWidget *									m_groupLabelLayoutW;
	QWidget *									m_buttonLabelLayoutW;

	ak::aPushButtonWidget *						m_btnClose;
	ak::aLabelWidget *							m_labelGroups;
	ak::aLineEditWidget *						m_filterGroups;
	ManageAccessTable *							m_groupsList;
	ak::aCheckBoxWidget *						m_showGroupsWithAccessOnly;

	std::string									m_authServerURL;
	std::string									m_projectName;

	std::list<std::string>						m_groupList;
	std::map<std::string, bool>					m_groupHasAccess;

	ManageAccess() = delete;
	ManageAccess(ManageAccess&) = delete;
	ManageAccess& operator = (ManageAccess&) = delete;
};
