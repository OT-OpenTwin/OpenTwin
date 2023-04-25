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

	void setColorStyle(ak::aColorStyle *_colorStyle);

	void getSelectedItems(QTableWidgetItem *&first, QTableWidgetItem *&second);

signals:
	void selectionChanged();

private slots:
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
class QCheckBox;

class ManageOwner : public ak::aDialog {
	Q_OBJECT

public:
	ManageOwner(const std::string &authServerURL, const std::string &assetType, const std::string &assetName, const std::string &ownerName);
	virtual ~ManageOwner();

	virtual void setColorStyle(ak::aColorStyle *_colorStyle) override;

public slots:
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

	ak::aPushButtonWidget *						m_btnClose;
	ak::aLabelWidget *							m_labelGroups;
	ak::aLineEditWidget *						m_filterGroups;
	ManageOwnerTable *							m_ownersList;

	std::string									m_authServerURL;
	std::string									m_assetType;
	std::string									m_assetName;
	std::string									m_assetOwner;
	QCheckBox *									m_ownerCheckBox;

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

