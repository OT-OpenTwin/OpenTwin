/*
 * welcomeScreen.h
 *
 *  Created on: October 30, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#pragma once

// AK header
#include <akWidgets/aWidget.h>

// Qt header
#include <qobject.h>
#include <qicon.h>
#include <qstring.h>
#include <qcolor.h>
#include <qlineedit.h>
#include <qtablewidget.h>
#include <qdialog.h>

#define TABLE_DATA_COLUMN_COUNT 9

class QLayout;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QWidget;
class QMouseEvent;
class QVBoxLayout;
class QHBoxLayout;
class QComboBox;

class ProjectManagement;
class UserManagement;
class recentsItem;
class lineEdit;
class table;
class copyProjectDialog;
class renameProjectDialog;

class AppBase;

class OldWelcomeScreen : public QObject, public ak::aWidget {
	Q_OBJECT
public:

	enum eventType
	{
		event_createClicked,
		event_openClicked,
		event_copyClicked,
		event_renameClicked,
		event_deleteClicked,
		event_exportClicked,
		event_accessClicked,
		event_rowDoubleClicked
	};

	OldWelcomeScreen(
		const QIcon &			_iconOpen,
		const QIcon &			_iconCopy,
		const QIcon &			_iconRename,
		const QIcon &			_iconDelete,
		const QIcon &			_iconExport,
		const QIcon &			_iconAccess,
		const QIcon &			_iconOwner,
		AppBase *				_manager
	);
	virtual ~OldWelcomeScreen();

	//! @brief Will return the widgets widget to display it
	virtual QWidget * widget(void) override;

	// #############################################################################################################

	void refreshRecent(void);

	void refreshProjectNames(void);

	QString getProjectName() const;

	QString getProjectName(
		int									_index
	) const;

	bool isShowingRecents() const { return my_isShowingRecent; }

	void lock(bool flag);
	void refreshList(void);

private Q_SLOTS:
	void slotProjectNameChanged(void);
	void slotCreateClicked(void);
	void slotRefreshClicked(void);
	void slotEditReturnPressed(void);
	void slotEditBackspacePressed(void);
	void slotDataTableCellClicked(QTableWidgetItem *);
	void slotDataTableCellDoubleClicked(QTableWidgetItem *);
	void slotCopyDialogClosing(void);
	void slotRenameDialogClosing(void);

private:
	
	struct layoutWidget
	{
		QWidget *			widget;
		QLayout *			layout;
	};

	layoutWidget createNewLayoutWidgetCombo(
		bool								_isHorizontal
	) const;

	layoutWidget			my_comboMain;
	layoutWidget			my_comboHeader;
	layoutWidget			my_comboSplit;
	layoutWidget			my_comboOpenMain;
	layoutWidget			my_comboInputs;
	layoutWidget			my_comboRecentsMain;

	QLabel *				my_labelWelcome;
	QLabel *				my_labelPlaceholder;
	QLabel *				my_labelItemsShowing;

	QLabel *				my_labelProjectName;
	lineEdit *				my_editProjectName;
	QPushButton *			my_buttonCreate;
	QPushButton *			my_buttonRefresh;
	table *					my_tableOpenNew;

	QWidget *				my_placeholderWidget;

	QIcon					my_iconOpen;
	QIcon					my_iconCopy;
	QIcon					my_iconRename;
	QIcon					my_iconDelete;
	QIcon					my_iconExport;
	QIcon					my_iconAccess;
	QIcon					my_iconOwner;

	bool					my_isShowingRecent;

	copyProjectDialog *		my_copyDialog;
	renameProjectDialog *	my_renameDialog;
	ProjectManagement *		my_projectManager;
	UserManagement *		my_userManager;
	AppBase *				my_app;

	int						my_lockCount;

	

	OldWelcomeScreen() = delete;
	OldWelcomeScreen(const OldWelcomeScreen&) = delete;


	/*
	
	#########################################################################################################################
	#	Title																											#
	#########################################################################################################################
	#																														#
	#  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  #
	#  +													+															 +  #
	#  +  ################################################  +  ########################################################	 +  #
	#  +  #	Txt - Edit - BTN							 #  +  #	Placeholder Label								  #	 +  #
	#  +  ################################################  +  ########################################################  +  #
	#  +  #	Table										 #  +  #	Placeholder widget								  #	 +  #
	#  +  ################################################  +  ########################################################	 +  #
	#  +													+															 +  #
	#  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  #
	#																														#
	#########################################################################################################################
	
	*/
};

// #############################################################################################################

// #############################################################################################################

// #############################################################################################################

class lineEdit : public QLineEdit {
	Q_OBJECT
public:
	lineEdit();
	lineEdit(
		const QString &			_initialText
	);
	virtual ~lineEdit();

	virtual void keyReleaseEvent(QKeyEvent *_event) override;

Q_SIGNALS:
	//void returnPressed(void);
	void backspacePressed(void);

private:
	lineEdit(const lineEdit&) = delete;
	lineEdit & operator = (const lineEdit&) = delete;
};

// #############################################################################################################

// #############################################################################################################

// #############################################################################################################

class table : public QTableWidget {
	Q_OBJECT
public:
	table();
	table(int _row, int _column);
	virtual ~table();

	void addRow(
		const std::array<QTableWidgetItem *,
		TABLE_DATA_COLUMN_COUNT> &				_columns
	);

	void Clear(void);

	QString projectName(
		int										_index
	) const;

	virtual void mouseMoveEvent(QMouseEvent * _event) override;

	virtual void leaveEvent(QEvent * _event) override;

private Q_SLOTS:
	void slotSelectionChanged();

private:

	std::vector<std::array<QTableWidgetItem *,
		TABLE_DATA_COLUMN_COUNT>>	my_dataRowItems;

	int				my_selectedRow;

};

// #############################################################################################################

// #############################################################################################################

// #############################################################################################################

class copyProjectDialog : public QDialog {
	Q_OBJECT
public:
	copyProjectDialog();
	virtual ~copyProjectDialog();

	inline QString projectName(void) const;

	bool wasConfirmed(void) const { return my_confirmed; }

	void setNotConfirmed(void) { my_confirmed = false; }

	void cancelClose(void) { my_cancelClose = true; }

	void reset(const QString & _projectToCopy);

Q_SIGNALS:
	void isClosing(void);

private Q_SLOTS:
	void slotButtonConfirmPressed();
	void slotButtonCancelPressed();
	void slotReturnPressed();
	
private:

	void Close(void);

	bool			my_confirmed;
	bool			my_cancelClose;

	QString			my_projectToCopy;

	QPushButton *	my_buttonConfirm;
	QPushButton *	my_buttonCancel;
	lineEdit *		my_input;
	QLabel *		my_label;
	QVBoxLayout *	my_layout;

	QHBoxLayout *	my_layoutButtons;
	QWidget *		my_widgetButtons;

	QHBoxLayout *	my_layoutInput;
	QWidget *		my_widgetInput;

	copyProjectDialog(const copyProjectDialog &) = delete;
	copyProjectDialog & operator = (copyProjectDialog &) = delete;
};

// #############################################################################################################

class renameProjectDialog : public QDialog {
	Q_OBJECT
public:
	renameProjectDialog();
	virtual ~renameProjectDialog();

	inline QString projectName(void) const;

	bool wasConfirmed(void) const { return my_confirmed; }

	void setNotConfirmed(void) { my_confirmed = false; }

	void cancelClose(void) { my_cancelClose = true; }

	void reset(const QString & _projectToCopy);

Q_SIGNALS:
	void isClosing(void);

private Q_SLOTS:
	void slotButtonConfirmPressed();
	void slotButtonCancelPressed();
	void slotReturnPressed();

private:

	void Close(void);

	bool			my_confirmed;
	bool			my_cancelClose;

	QString			my_projectToCopy;

	QPushButton *	my_buttonConfirm;
	QPushButton *	my_buttonCancel;
	lineEdit *		my_input;
	QLabel *		my_label;
	QVBoxLayout *	my_layout;

	QHBoxLayout *	my_layoutButtons;
	QWidget *		my_widgetButtons;

	QHBoxLayout *	my_layoutInput;
	QWidget *		my_widgetInput;

	renameProjectDialog(const renameProjectDialog &) = delete;
	renameProjectDialog & operator = (renameProjectDialog &) = delete;
};

// #############################################################################################################

class createNewAccountDialog : public QDialog {
	Q_OBJECT
public:
	createNewAccountDialog();
	virtual ~createNewAccountDialog();

	void setPassword(const QString &pwd) { my_password = pwd; }

	bool wasConfirmed(void) const { return my_confirmed; }

	void setNotConfirmed(void) { my_confirmed = false; }

	void cancelClose(void) { my_cancelClose = true; }

Q_SIGNALS:
	void isClosing(void);

private Q_SLOTS:
	void slotButtonConfirmPressed();
	void slotButtonCancelPressed();
	void slotReturnPressed();

private:

	void Close(void);

	bool			my_confirmed;
	bool			my_cancelClose;

	QString			my_password;

	QPushButton *	my_buttonConfirm;
	QPushButton *	my_buttonCancel;
	lineEdit *		my_input;
	QLabel *		my_label;
	QLabel *		my_text;
	QVBoxLayout *	my_layout;

	QHBoxLayout *	my_layoutButtons;
	QWidget *		my_widgetButtons;

	QHBoxLayout *	my_layoutInput;
	QWidget *		my_widgetInput;

	createNewAccountDialog(const createNewAccountDialog &) = delete;
	createNewAccountDialog & operator = (createNewAccountDialog &) = delete;
};
