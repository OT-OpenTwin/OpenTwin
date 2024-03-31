/*
 * welcomeScreen.cpp
 *
 *  Created on: October 30, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

// uiService header
#include "welcomeScreen.h"				// Corresponding header
#include "ProjectManagement.h"
#include "UserManagement.h"
#include "ManageOwner.h"
#include "AppBase.h"
#include "ExternalServicesComponent.h"

#include "OTCommunication/ActionTypes.h"

// AK header
#include <akAPI/uiAPI.h>

// Qt header
#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtablewidget.h>
#include <qtablewidget.h>
#include <qpushbutton.h>
#include <qlistwidget.h>
#include <qcombobox.h>
#include <qfont.h>
#include <qstringlist.h>
#include <qheaderview.h>
#include <qevent.h>
#include <qcryptographichash.h>
#include <qopenglwidget.h>

#define MY_LABELSIZE_WELCOME 24
#define MY_LABELSIZE_HEADER 24
#define MY_RECENTITEM_SIZE 14

// Note: also update TABLE_DATA_COLUMN_COUNT in welcomeScreen.h to reflect the number of columns
#define TABLE_DATA_INDEX_OPEN 0
#define TABLE_DATA_INDEX_COPY 1
#define TABLE_DATA_INDEX_RENAME 2
#define TABLE_DATA_INDEX_DELETE 3
#define TABLE_DATA_INDEX_EXPORT 4
#define TABLE_DATA_INDEX_ACCESS 5
#define TABLE_DATA_INDEX_NAME 6
#define TABLE_DATA_INDEX_AUTHOR 7
#define TABLE_DATA_INDEX_OWNER 8

#define EDIT_PLACEHOLDER_RECENTS "[Recent projects] (Press Return to show all projects)"
#define EDIT_PLACEHOLDER_ALL "[All projects] (Press Backspace to show recent projects)"

welcomeScreen::welcomeScreen(
	const std::string &			_username,
	const std::string &			_databaseUrl,
	const std::string &			_authUrl,
	const QIcon &				_iconOpen,
	const QIcon &				_iconCopy,
	const QIcon &				_iconRename,
	const QIcon &				_iconDelete,
	const QIcon &				_iconExport,
	const QIcon &				_iconAccess,
	const QIcon &				_iconOwner,
	AppBase *					_manager
)
	: ak::aWidget{ ak::otNone },
	my_username{ _username }, my_iconCopy{ _iconCopy },  my_iconRename{ _iconRename }, my_iconDelete{ _iconDelete }, my_iconExport{ _iconExport }, 
	my_iconAccess{ _iconAccess }, my_iconOpen{ _iconOpen }, my_iconOwner{ _iconOwner }, my_app{ _manager },
	my_isShowingRecent{ false }, my_copyDialog{ nullptr }, my_renameDialog{ nullptr }, my_lockCount{ 0 }, my_authURL{_authUrl}, my_databaseUrl{_databaseUrl}
{
	assert(my_username.length() != 0); // No username provided
	assert(my_app != nullptr); // Nullptr provided

	QOpenGLWidget* glDummy = new QOpenGLWidget;
	glDummy->setMaximumSize(1, 1);

	// Create layout widget combos
	my_comboInputs = createNewLayoutWidgetCombo(true);
	my_comboHeader = createNewLayoutWidgetCombo(true);
	//my_comboHeader.layout->setContentsMargins(0, 0, 0, 0);
	my_comboInputs.layout->setContentsMargins(0, 0, 0, 0);
	my_comboMain = createNewLayoutWidgetCombo(false);
	//my_comboMain.layout->setContentsMargins(0, 0, 0, 0);
	my_comboOpenMain = createNewLayoutWidgetCombo(false);
	my_comboOpenMain.layout->setContentsMargins(0, 0, 0, 0);
	my_comboRecentsMain = createNewLayoutWidgetCombo(false);
	my_comboRecentsMain.layout->setContentsMargins(0, 0, 0, 0);
	my_comboSplit = createNewLayoutWidgetCombo(true);
	my_comboSplit.layout->setContentsMargins(0, 0, 0, 0);

	// Create refresh button
	my_buttonRefresh = new QPushButton("");
	my_buttonRefresh->setIcon( ak::uiAPI::getIcon("Refresh", "Default"));

	// Create welcome label
	my_labelWelcome = new QLabel("Manage Projects");
	QFont font = my_labelWelcome->font();
	font.setPointSize(MY_LABELSIZE_WELCOME);
	my_labelWelcome->setFont(font);
	my_comboHeader.layout->addWidget(my_labelWelcome);
	dynamic_cast<QHBoxLayout*>(my_comboHeader.layout)->addStretch(0);
	dynamic_cast<QHBoxLayout*>(my_comboHeader.layout)->addWidget(my_buttonRefresh, 0, Qt::AlignRight);

	my_comboMain.layout->addWidget(my_comboHeader.widget);
	my_comboMain.layout->addWidget(my_comboSplit.widget);
	my_comboMain.layout->addWidget(glDummy);

	my_comboSplit.layout->addWidget(my_comboOpenMain.widget);
	my_comboSplit.layout->addWidget(my_comboRecentsMain.widget);
	
	// Create open/new objects
	my_editProjectName = new lineEdit("");
	my_editProjectName->setPlaceholderText(EDIT_PLACEHOLDER_RECENTS);
	my_labelProjectName = new QLabel("Name:");
	my_labelProjectName->setBuddy(my_editProjectName);
	my_buttonCreate = new QPushButton("Create");
	my_buttonCreate->setEnabled(false);
	my_tableOpenNew = new table(0, TABLE_DATA_COLUMN_COUNT);
	my_tableOpenNew->verticalHeader()->setVisible(false);
	my_tableOpenNew->horizontalHeader()->setSectionResizeMode(TABLE_DATA_INDEX_NAME, QHeaderView::ResizeMode::Stretch);
	//my_tableOpenNew->horizontalHeader()->setSectionResizeMode(TABLE_DATA_INDEX_AUTHOR, QHeaderView::ResizeMode::Stretch);
	//my_tableOpenNew->horizontalHeader()->setSectionResizeMode(TABLE_DATA_INDEX_LASTSAVE, QHeaderView::ResizeMode::Stretch);
	my_tableOpenNew->horizontalHeader()->setSectionResizeMode(TABLE_DATA_INDEX_OPEN, QHeaderView::ResizeMode::ResizeToContents);
	my_tableOpenNew->horizontalHeader()->setSectionResizeMode(TABLE_DATA_INDEX_COPY, QHeaderView::ResizeMode::ResizeToContents);
	my_tableOpenNew->horizontalHeader()->setSectionResizeMode(TABLE_DATA_INDEX_RENAME, QHeaderView::ResizeMode::ResizeToContents);
	my_tableOpenNew->horizontalHeader()->setSectionResizeMode(TABLE_DATA_INDEX_DELETE, QHeaderView::ResizeMode::ResizeToContents);
	my_tableOpenNew->horizontalHeader()->setSectionResizeMode(TABLE_DATA_INDEX_EXPORT, QHeaderView::ResizeMode::ResizeToContents);
	my_tableOpenNew->horizontalHeader()->setSectionResizeMode(TABLE_DATA_INDEX_ACCESS, QHeaderView::ResizeMode::ResizeToContents);
	my_tableOpenNew->horizontalHeader()->setSectionResizeMode(TABLE_DATA_INDEX_OWNER, QHeaderView::ResizeMode::ResizeToContents);
	my_tableOpenNew->horizontalHeader()->setDefaultAlignment(Qt::AlignmentFlag::AlignLeft);

	QStringList labels;
	labels.push_back("Open");
	labels.push_back("Copy");
	labels.push_back("Rename");
	labels.push_back("Delete");
	labels.push_back("Export");
	labels.push_back("Access");
	labels.push_back("Project name");
	labels.push_back("Owner");
	labels.push_back("");
	my_tableOpenNew->setHorizontalHeaderLabels(labels);
	my_tableOpenNew->setAlternatingRowColors(true);

	my_labelItemsShowing = new QLabel("No projects found");

	my_comboInputs.layout->addWidget(my_labelProjectName);
	my_comboInputs.layout->addWidget(my_editProjectName);
	my_comboInputs.layout->addWidget(my_buttonCreate);
	my_comboOpenMain.layout->addWidget(my_comboInputs.widget);
	my_comboOpenMain.layout->addWidget(my_tableOpenNew);
	my_comboOpenMain.layout->addWidget(my_labelItemsShowing);

	// Create recents objects
	my_labelPlaceholder = nullptr; /* = new QLabel("Recent projects");
	font = my_labelRecents->font();
	font.setPointSize(MY_LABELSIZE_HEADER);
	my_labelRecents->setFont(font);
	my_listRecents = new QListWidget();
	*/
	my_placeholderWidget = nullptr;

	//my_comboRecentsMain.layout->addWidget(my_labelPlaceholder);
	//my_comboRecentsMain.layout->addWidget(my_placeholderWidget);

	my_projectManager = new ProjectManagement();
	my_projectManager->setDataBaseURL(_databaseUrl);
	my_projectManager->setAuthServerURL(_authUrl);

	bool success = my_projectManager->InitializeConnection();
	assert(success);	// Failed to create connection

	my_userManager = new UserManagement();
	my_userManager->setAuthServerURL(_authUrl);
	my_userManager->setDatabaseURL(_databaseUrl);
	success = my_userManager->checkConnection();		
	assert(success);	// Failed to create connection

	my_copyDialog = new copyProjectDialog;
	my_renameDialog = new renameProjectDialog;

	connect(my_editProjectName, SIGNAL(textChanged(const QString&)), this, SLOT(slotProjectNameChanged()));
	connect(my_buttonCreate, SIGNAL(clicked()), this, SLOT(slotCreateClicked()));
	connect(my_buttonRefresh, SIGNAL(clicked()), this, SLOT(slotRefreshClicked()));
	connect(my_editProjectName, SIGNAL(returnPressed()), this, SLOT(slotEditReturnPressed()));
	connect(my_editProjectName, SIGNAL(backspacePressed()), this, SLOT(slotEditBackspacePressed()));
	connect(my_tableOpenNew, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(slotDataTableCellClicked(QTableWidgetItem *)));
	connect(my_tableOpenNew, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(slotDataTableCellDoubleClicked(QTableWidgetItem *)));
	connect(my_copyDialog, SIGNAL(isClosing()), this, SLOT(slotCopyDialogClosing()));
	connect(my_renameDialog, SIGNAL(isClosing()), this, SLOT(slotRenameDialogClosing()));

	//slotProjectNameChanged();
}

welcomeScreen::~welcomeScreen() {
}

QWidget * welcomeScreen::widget(void) {
	return my_comboMain.widget;
}

// #############################################################################################################

void welcomeScreen::refreshRecent(void) {
	std::list<std::string> recent;
	my_userManager->getListOfRecentProjects(recent);
	my_projectManager->readProjectAuthor(recent);

	my_isShowingRecent = true;

	my_tableOpenNew->Clear();

	AppBase * app{ AppBase::instance() };
	std::string currentUser = app->getCredentialUserName();

	for (auto itm : recent) {
		std::array<QTableWidgetItem *, TABLE_DATA_COLUMN_COUNT> entry;
		std::string editorName("Unknown");
		my_projectManager->getProjectAuthor(itm, editorName);

		entry[TABLE_DATA_INDEX_AUTHOR] = new QTableWidgetItem(editorName.c_str());
		entry[TABLE_DATA_INDEX_COPY] = new QTableWidgetItem(my_iconCopy, "");
		entry[TABLE_DATA_INDEX_EXPORT] = new QTableWidgetItem(my_iconExport, "");
		entry[TABLE_DATA_INDEX_NAME] = new QTableWidgetItem(itm.c_str());
		entry[TABLE_DATA_INDEX_OPEN] = new QTableWidgetItem(my_iconOpen, "");

		if (editorName == currentUser)
		{
			entry[TABLE_DATA_INDEX_RENAME] = new QTableWidgetItem(my_iconRename, "");
			entry[TABLE_DATA_INDEX_DELETE] = new QTableWidgetItem(my_iconDelete, "");
			entry[TABLE_DATA_INDEX_ACCESS] = new QTableWidgetItem(my_iconAccess, "");
			entry[TABLE_DATA_INDEX_OWNER] = new QTableWidgetItem(my_iconOwner, "");
		}
		else
		{
			entry[TABLE_DATA_INDEX_RENAME] = new QTableWidgetItem("");
			entry[TABLE_DATA_INDEX_DELETE] = new QTableWidgetItem("");
			entry[TABLE_DATA_INDEX_ACCESS] = new QTableWidgetItem("");
			entry[TABLE_DATA_INDEX_OWNER] = new QTableWidgetItem("");
		}

		my_tableOpenNew->addRow(entry);
	}

	if (my_tableOpenNew->rowCount() == 0) {
		my_labelItemsShowing->setText("No projects found");
	}
	else {
		my_labelItemsShowing->setText(QString::number(my_tableOpenNew->rowCount()).append(" projects found"));
	}

}

void welcomeScreen::refreshProjectNames(void) {

	my_tableOpenNew->Clear();

	std::list<std::string> result;
	bool resultExceeded = false;

	my_isShowingRecent = false;

	my_projectManager->findProjectNames(my_editProjectName->text().toStdString(), 100, result, resultExceeded);

	AppBase * app{ AppBase::instance() };
	std::string currentUser = app->getCredentialUserName();

	for (auto itm : result) {
		std::array<QTableWidgetItem *, TABLE_DATA_COLUMN_COUNT> entry;
		std::string editorName("Unknown");
		my_projectManager->getProjectAuthor(itm, editorName);
		
		entry[TABLE_DATA_INDEX_AUTHOR] = new QTableWidgetItem(editorName.c_str());
		entry[TABLE_DATA_INDEX_COPY] = new QTableWidgetItem(my_iconCopy, "");
		entry[TABLE_DATA_INDEX_EXPORT] = new QTableWidgetItem(my_iconExport, "");
		entry[TABLE_DATA_INDEX_NAME] = new QTableWidgetItem(itm.c_str());
		entry[TABLE_DATA_INDEX_OPEN] = new QTableWidgetItem(my_iconOpen, "");


		if (editorName == currentUser)
		{
			entry[TABLE_DATA_INDEX_RENAME] = new QTableWidgetItem(my_iconRename, "");
			entry[TABLE_DATA_INDEX_DELETE] = new QTableWidgetItem(my_iconDelete, "");
			entry[TABLE_DATA_INDEX_ACCESS] = new QTableWidgetItem(my_iconAccess, "");
			entry[TABLE_DATA_INDEX_OWNER] = new QTableWidgetItem(my_iconOwner, "");
		}
		else
		{
			entry[TABLE_DATA_INDEX_RENAME] = new QTableWidgetItem("");
			entry[TABLE_DATA_INDEX_DELETE] = new QTableWidgetItem("");
			entry[TABLE_DATA_INDEX_ACCESS] = new QTableWidgetItem("");
			entry[TABLE_DATA_INDEX_OWNER] = new QTableWidgetItem("");
		}

		my_tableOpenNew->addRow(entry);
	}

	if (my_tableOpenNew->rowCount() == 0) {
		my_labelItemsShowing->setText("No projects found");
	}
	else if (resultExceeded) {
		my_labelItemsShowing->setText("Showing the first " + QString::number(my_tableOpenNew->rowCount()).append(" projects found"));
	}
	else {
		my_labelItemsShowing->setText(QString::number(my_tableOpenNew->rowCount()).append(" projects found"));
	}

}

QString welcomeScreen::getProjectName(
	int									_index
) const { return my_tableOpenNew->projectName(_index); }

QString welcomeScreen::getProjectName() const { return my_editProjectName->text(); }

void welcomeScreen::refreshList(void) { 

	lock(true);

	if (isShowingRecents()) 
	{ 
		refreshRecent(); 
	}
	else 
	{ 
		refreshProjectNames(); 
	}

	lock(false);
}
// #############################################################################################################

// Slots

void welcomeScreen::slotProjectNameChanged(void) {

	QString txt = my_editProjectName->text();
	my_editProjectName->setPlaceholderText(EDIT_PLACEHOLDER_RECENTS);
	if (txt.isEmpty()) {
		refreshRecent();
		my_buttonCreate->setEnabled(false);
	}
	else {
		refreshProjectNames();
		my_buttonCreate->setEnabled(true);
	}
}

void welcomeScreen::slotEditReturnPressed(void) {

	refreshProjectNames();
	if (my_editProjectName->text().length() == 0) {
		my_editProjectName->setPlaceholderText(EDIT_PLACEHOLDER_ALL);
	}
}

void welcomeScreen::slotEditBackspacePressed(void) {

	if (my_editProjectName->text().length() == 0 && !my_isShowingRecent) {
		refreshRecent();
		my_editProjectName->setPlaceholderText(EDIT_PLACEHOLDER_RECENTS);
	}
}

void welcomeScreen::slotCreateClicked(void) { 
	lock(true);

	my_app->welcomeScreenEventCallback(event_createClicked, -1);

	lock(false);
}

void welcomeScreen::slotRefreshClicked(void) { 

	refreshList();
}

void welcomeScreen::slotDataTableCellClicked(QTableWidgetItem * _item) {
	lock(true);

	AppBase * app{ AppBase::instance() };
	std::string currentUser = app->getCredentialUserName();
	std::string owner = my_tableOpenNew->item(_item->row(), TABLE_DATA_INDEX_AUTHOR)->text().toStdString();

	bool userIsOwner = (owner == currentUser);

	switch (_item->column())
	{
	case TABLE_DATA_INDEX_OPEN: my_app->welcomeScreenEventCallback(event_openClicked, _item->row()); break;
	case TABLE_DATA_INDEX_DELETE: if (userIsOwner) { my_app->welcomeScreenEventCallback(event_deleteClicked, _item->row()); }; break;
	case TABLE_DATA_INDEX_EXPORT: my_app->welcomeScreenEventCallback(event_exportClicked, _item->row()); break;
	case TABLE_DATA_INDEX_ACCESS: if (userIsOwner) { my_app->welcomeScreenEventCallback(event_accessClicked, _item->row()); }; break;
	case TABLE_DATA_INDEX_COPY:
	{
		my_copyDialog->reset(my_tableOpenNew->projectName(_item->row())); my_copyDialog->exec();
		if (my_copyDialog->wasConfirmed()) {
			my_app->welcomeScreenEventCallback(event_copyClicked, _item->row(), my_copyDialog->projectName());
			if (my_isShowingRecent) { refreshRecent(); }
			else { refreshProjectNames(); }
		}
		break;
	}
	case TABLE_DATA_INDEX_RENAME:
	{
		if (userIsOwner)
		{
			my_renameDialog->reset(my_tableOpenNew->projectName(_item->row())); my_renameDialog->exec();
			if (my_renameDialog->wasConfirmed()) {
				my_app->welcomeScreenEventCallback(event_renameClicked, _item->row(), my_renameDialog->projectName());
				if (my_isShowingRecent) { refreshRecent(); }
				else { refreshProjectNames(); }
			}
		}
		break;
	}
	case TABLE_DATA_INDEX_OWNER:
	{
		if (userIsOwner)
		{
			std::string projectName = my_tableOpenNew->projectName(_item->row()).toStdString();
			ManageProjectOwner ownerManager(my_authURL, projectName, currentUser);

			ownerManager.showDialog();

			// Now we need to check whether we have no longer access to the project
			std::string projectCollection = my_projectManager->getProjectCollection(projectName);
			bool hasAccess = my_projectManager->canAccessProject(projectCollection);

			if (!hasAccess)
			{
				// We need to close the project if it is currently open
				if (my_app->getCurrentProjectName() == projectName)
				{
					my_app->getExternalServicesComponent()->closeProject(false);
				}

				// Remove the project from the recent project list
				UserManagement uManager;
				uManager.setAuthServerURL(my_authURL);
				uManager.setDatabaseURL(my_databaseUrl);
				bool checkConnection = uManager.checkConnection(); assert(checkConnection); // Connect and check
				uManager.removeRecentProject(projectName);
			}

			if (my_isShowingRecent) { refreshRecent(); }
			else { refreshProjectNames(); }
		}
		break;
	}
	}

	lock(false);
}

void welcomeScreen::slotDataTableCellDoubleClicked(QTableWidgetItem * _item) {
	lock(true);

	switch (_item->column())
	{
	case TABLE_DATA_INDEX_OPEN: break;
	case TABLE_DATA_INDEX_COPY: break;
	case TABLE_DATA_INDEX_RENAME: break;
	case TABLE_DATA_INDEX_DELETE: break;
	case TABLE_DATA_INDEX_EXPORT: break;
	case TABLE_DATA_INDEX_ACCESS: break;
	case TABLE_DATA_INDEX_OWNER: break;
	default: my_app->welcomeScreenEventCallback(event_rowDoubleClicked, _item->row());
	}

	lock(false);
}

void welcomeScreen::slotCopyDialogClosing(void) {
	lock(true);

	assert(my_copyDialog != nullptr); // Slot called without a copy dialog
	assert(my_projectManager != nullptr); // This should not happen
	if (my_copyDialog->wasConfirmed()) {
		QString projName{ my_copyDialog->projectName() };
		bool canBeDeleted = false;
		if (my_projectManager->projectExists(projName.toStdString(), canBeDeleted)) 
		{
			if (!canBeDeleted)
			{
				// Notify that the project already exists and can not be deleted
				QString msg{ "A project with the name \"" };
				msg.append(projName).append("\" does already exist and belongs to another owner.");
				ak::uiAPI::promptDialog::show(msg, "Copy project", ak::promptOkIconLeft, "DialogError", "Default", my_copyDialog);
				my_copyDialog->setNotConfirmed(); 
			}
			else
			{
				// Create prompt
				QString msg{ "A project with the name \"" };
				msg.append(projName).append("\" does already exist.\nDo you want to replace it?");
				ak::dialogResult result = ak::uiAPI::promptDialog::show(msg, "Copy project", ak::promptYesNoCancelIconLeft, "DialogWarning", "Default", my_copyDialog);
				if (result == ak::resultYes) {
					// Get current project name and close it if this is the project that should be replaced
					QString currentProjName(my_app->getCurrentProjectName().c_str());
					if (currentProjName == projName) {
						my_app->getExternalServicesComponent()->closeProject(false);
					}

					// Delete Project
					my_projectManager->deleteProject(projName.toStdString());
				}
				else if (result == ak::resultCancel) { my_copyDialog->setNotConfirmed(); }
				else { my_copyDialog->cancelClose(); }
			}
		}
	}

	lock(false);
}

void welcomeScreen::slotRenameDialogClosing(void) {
	lock(true);

	assert(my_renameDialog != nullptr); // Slot called without a copy dialog
	assert(my_projectManager != nullptr); // This should not happen
	if (my_renameDialog->wasConfirmed()) {
		QString projName{ my_renameDialog->projectName() };
		bool canBeDeleted = false;
		if (my_projectManager->projectExists(projName.toStdString(), canBeDeleted))
		{
			if (!canBeDeleted)
			{
				// Notify that the project already exists and can not be deleted
				QString msg{ "A project with the name \"" };
				msg.append(projName).append("\" does already exist and belongs to another owner.");
				ak::uiAPI::promptDialog::show(msg, "Rename project", ak::promptOkIconLeft, "DialogError", "Default", my_renameDialog);
				my_renameDialog->setNotConfirmed();
			}
			else
			{
				// Create prompt
				QString msg{ "A project with the name \"" };
				msg.append(projName).append("\" does already exist.\nDo you want to replace it?");
				ak::dialogResult result = ak::uiAPI::promptDialog::show(msg, "Rename project", ak::promptYesNoCancelIconLeft, "DialogWarning", "Default", my_renameDialog);
				if (result == ak::resultYes) {
					// Get current project name and close it if this is the project that should be replaced
					QString currentProjName(my_app->getCurrentProjectName().c_str());
					if (currentProjName == projName) {
						my_app->getExternalServicesComponent()->closeProject(false);
					}

					// Delete Project
					my_projectManager->deleteProject(projName.toStdString());
				}
				else if (result == ak::resultCancel) { my_renameDialog->setNotConfirmed(); }
				else { my_renameDialog->cancelClose(); }
			}
		}
	}

	lock(false);
}

void welcomeScreen::lock(bool flag)
{
	if (flag)
	{
		if (my_lockCount == 0)
		{
			widget()->setEnabled(false);
		}
		my_lockCount++;
	}
	else
	{
		assert(my_lockCount > 0);
		if (my_lockCount == 0) return;

		my_lockCount--;
		if (my_lockCount == 0)
		{
			widget()->setEnabled(true);
		}
	}
}

// #############################################################################################################

// Private members

welcomeScreen::layoutWidget welcomeScreen::createNewLayoutWidgetCombo(
	bool								_isHorizontal
) const {
	layoutWidget ret;
	ret.widget = nullptr;
	ret.layout = nullptr;
	ret.widget = new QWidget();
	if (_isHorizontal) { ret.layout = new QHBoxLayout(ret.widget); }
	else { ret.layout = new QVBoxLayout(ret.widget); }
	return ret;
}

// #############################################################################################################

// #############################################################################################################

// #############################################################################################################

lineEdit::lineEdit(
	const QString &			_initialText
) : QLineEdit{ _initialText }
{}

lineEdit::lineEdit()
{}

lineEdit::~lineEdit()
{}

void lineEdit::keyReleaseEvent(QKeyEvent *_event) {
	if (_event->key() == Qt::Key::Key_Return) {
		Q_EMIT returnPressed();
	}
	else if (_event->key() == Qt::Key::Key_Backspace) {
		Q_EMIT backspacePressed();
	}
}

// #############################################################################################################

// #############################################################################################################

// #############################################################################################################

table::table()
	: QTableWidget(), my_selectedRow(-1),
	my_colorBack(255, 255, 255), my_colorFocusBack(0, 0, 255), my_colorFocusFront(0, 0, 0),
	my_colorFront(0, 0, 0), my_colorSelectedBack(0, 255, 0), my_colorSelectedFront(0, 0, 0)
{
	setMouseTracking(true);
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
}

table::table(int _rows, int _columns)
	: QTableWidget(_rows, _columns), my_selectedRow(-1),
	my_colorBack(255, 255, 255), my_colorFocusBack(0, 0, 255), my_colorFocusFront(0, 0, 0),
	my_colorFront(0, 0, 0), my_colorSelectedBack(0, 255, 0), my_colorSelectedFront(0, 0, 0)
{
	setMouseTracking(true);
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
}

table::~table() {

}

void table::addRow(
	const std::array<QTableWidgetItem *, TABLE_DATA_COLUMN_COUNT> & _columns
) {
	insertRow(rowCount());
	for (int c = 0; c < TABLE_DATA_COLUMN_COUNT; c++) {
		_columns[c]->setBackground(my_colorBack);
		_columns[c]->setForeground(my_colorFront);
		setItem(rowCount() - 1, c, _columns[c]);
		Qt::ItemFlags f = _columns[c]->flags();
		f.setFlag(Qt::ItemFlag::ItemIsEditable, false);
		_columns[c]->setFlags(f);
	}
	my_dataRowItems.push_back(_columns);
}

void table::Clear() {
	for (auto itm : my_dataRowItems) {
		for (auto cell : itm) {
			QTableWidgetItem * actualCell = cell;
			delete actualCell;
		}
	}
	my_dataRowItems.clear();
	setRowCount(0);
	my_selectedRow = -1;
}

QString table::projectName(
	int										_index
) const {
	assert(_index >= 0 && _index < rowCount()); // Index out of range
	return my_dataRowItems.at(_index)[TABLE_DATA_INDEX_NAME]->text();
}

void table::mouseMoveEvent(QMouseEvent * _event) {
	QTableWidgetItem * itm = itemAt(_event->pos());
	if (itm == nullptr) {
		for (auto r : my_dataRowItems) {
			for (auto c : r) {
				if (c->row() == my_selectedRow) {
					c->setBackground(my_colorSelectedBack);
					c->setForeground(my_colorSelectedFront);
				}
				else {
					c->setBackground(my_colorBack);
					c->setForeground(my_colorFront);
				}
			}
		}
	}
	else {
		for (auto r : my_dataRowItems) {
			for (auto c : r) {
				if (c->row() == itm->row()) {
					c->setBackground(my_colorFocusBack);
					c->setForeground(my_colorFocusFront);
				}
				else if (c->row() == my_selectedRow) {
					c->setBackground(my_colorSelectedBack);
					c->setForeground(my_colorSelectedFront);
				}
				else {
					c->setBackground(my_colorBack);
					c->setForeground(my_colorFront);
				}
			}
		}
	}
}

void table::leaveEvent(QEvent * _event) {
	for (auto r : my_dataRowItems) {
		for (auto c : r) {
			if (c->row() == my_selectedRow) {
				c->setBackground(my_colorSelectedBack);
				c->setForeground(my_colorSelectedFront);
			}
			else {
				c->setBackground(my_colorBack);
				c->setForeground(my_colorFront);
			}
		}
	}
}

void table::slotSelectionChanged() {
	disconnect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
	QList<QTableWidgetItem *> selection = selectedItems();
	for (auto itm : selection) {
		for (auto c : my_dataRowItems.at(itm->row())) {
			c->setSelected(false);
			c->setBackground(my_colorSelectedBack);
			c->setForeground(my_colorSelectedFront);
		}
		my_selectedRow = itm->row();
	}
	for (int r = 0; r < my_dataRowItems.size(); r++) {
		if (r != my_selectedRow) {
			for (auto c : my_dataRowItems.at(r)) {
				c->setBackground(my_colorBack);
				c->setForeground(my_colorFront);
			}
		}
	}
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
}

// #############################################################################################################

// #############################################################################################################

// #############################################################################################################

copyProjectDialog::copyProjectDialog()
	: my_buttonCancel{ nullptr }, my_buttonConfirm{ nullptr }, my_cancelClose{ false }, my_confirmed{ false }, my_input{ nullptr },
	my_layout{ nullptr }, my_layoutButtons{ nullptr }, my_layoutInput{ nullptr }, my_widgetButtons{ nullptr }, my_widgetInput{ nullptr }
{
	// Create controls
	my_buttonCancel = new QPushButton{ "Cancel" };
	my_buttonConfirm = new QPushButton{ "Confirm" };
	my_input = new lineEdit;
	my_label = new QLabel("Name");
	my_label->setBuddy(my_input);

	// Create main layout
	my_layout = new QVBoxLayout{ this };

	// Create input layout
	my_widgetInput = new QWidget;
	my_layoutInput = new QHBoxLayout{ my_widgetInput };
	my_layoutInput->addWidget(my_label);
	my_layoutInput->addWidget(my_input);
	my_layout->addWidget(my_widgetInput);

	// Create button layout
	my_widgetButtons = new QWidget;
	my_layoutButtons = new QHBoxLayout{ my_widgetButtons };
	my_layoutButtons->addWidget(my_buttonConfirm);
	my_layoutButtons->addWidget(my_buttonCancel);
	my_layout->addWidget(my_widgetButtons);

	setWindowTitle("Copy project");
	setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	// Hide info button
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	
	connect(my_input, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));
	connect(my_buttonCancel, SIGNAL(clicked()), this, SLOT(slotButtonCancelPressed()));
	connect(my_buttonConfirm, SIGNAL(clicked()), this, SLOT(slotButtonConfirmPressed()));
}

copyProjectDialog::~copyProjectDialog() {
	delete my_buttonCancel;
	delete my_buttonConfirm;
	delete my_input;

	delete my_layoutButtons;
	delete my_widgetButtons;
	
	delete my_layoutInput;
	delete my_widgetInput;
	
	delete my_layout;
}

QString copyProjectDialog::projectName(void) const { return my_input->text(); }

void copyProjectDialog::reset(const QString & _projectToCopy) {
	my_input->setText(_projectToCopy + " - copy");
	my_confirmed = false;
	my_projectToCopy = _projectToCopy;
}

void copyProjectDialog::slotButtonConfirmPressed() { my_confirmed = true; Close(); }

void copyProjectDialog::slotButtonCancelPressed() { my_confirmed = false;  close(); }

void copyProjectDialog::slotReturnPressed() { my_confirmed = true; Close(); }

void copyProjectDialog::Close(void) {
	if (my_input->text().length() == 0) { return; }
	if (my_confirmed && my_projectToCopy == my_input->text()) {
		ak::uiAPI::promptDialog::show("The name of the project copy must be different from the original.", "Copy project", ak::promptOkIconLeft, "DialogError", "Default", this);
		my_confirmed = false;
		return;
	}
	my_cancelClose = false;
	Q_EMIT isClosing();
	if (!my_cancelClose) { close(); }
	else { my_confirmed = false; }
}

// #############################################################################################################

renameProjectDialog::renameProjectDialog()
	: my_buttonCancel{ nullptr }, my_buttonConfirm{ nullptr }, my_cancelClose{ false }, my_confirmed{ false }, my_input{ nullptr },
	my_layout{ nullptr }, my_layoutButtons{ nullptr }, my_layoutInput{ nullptr }, my_widgetButtons{ nullptr }, my_widgetInput{ nullptr }
{
	// Create controls
	my_buttonCancel = new QPushButton{ "Cancel" };
	my_buttonConfirm = new QPushButton{ "Confirm" };
	my_input = new lineEdit;
	my_label = new QLabel("Name");
	my_label->setBuddy(my_input);

	// Create main layout
	my_layout = new QVBoxLayout{ this };

	// Create input layout
	my_widgetInput = new QWidget;
	my_layoutInput = new QHBoxLayout{ my_widgetInput };
	my_layoutInput->addWidget(my_label);
	my_layoutInput->addWidget(my_input);
	my_layout->addWidget(my_widgetInput);

	// Create button layout
	my_widgetButtons = new QWidget;
	my_layoutButtons = new QHBoxLayout{ my_widgetButtons };
	my_layoutButtons->addWidget(my_buttonConfirm);
	my_layoutButtons->addWidget(my_buttonCancel);
	my_layout->addWidget(my_widgetButtons);

	setWindowTitle("Rename project");
	setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	// Hide info button
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	connect(my_input, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));
	connect(my_buttonCancel, SIGNAL(clicked()), this, SLOT(slotButtonCancelPressed()));
	connect(my_buttonConfirm, SIGNAL(clicked()), this, SLOT(slotButtonConfirmPressed()));
}

renameProjectDialog::~renameProjectDialog() {
	delete my_buttonCancel;
	delete my_buttonConfirm;
	delete my_input;

	delete my_layoutButtons;
	delete my_widgetButtons;

	delete my_layoutInput;
	delete my_widgetInput;

	delete my_layout;
}

QString renameProjectDialog::projectName(void) const { return my_input->text(); }

void renameProjectDialog::reset(const QString & _projectToCopy) {
	my_input->setText(_projectToCopy);
	my_confirmed = false;
	my_projectToCopy = _projectToCopy;
}

void renameProjectDialog::slotButtonConfirmPressed() { my_confirmed = true; Close(); }

void renameProjectDialog::slotButtonCancelPressed() { my_confirmed = false;  close(); }

void renameProjectDialog::slotReturnPressed() { my_confirmed = true; Close(); }

void renameProjectDialog::Close(void) {
	if (my_input->text().length() == 0) { return; }
	if (my_confirmed && my_projectToCopy == my_input->text()) {
		ak::uiAPI::promptDialog::show("The new name of the project must be different from the original.", "Rename project", ak::promptOkIconLeft, "DialogError", "Default", this);
		my_confirmed = false;
		return;
	}
	my_cancelClose = false;
	Q_EMIT isClosing();
	if (!my_cancelClose) { close(); }
	else { my_confirmed = false; }
}

// #############################################################################################################

// #############################################################################################################

createNewAccountDialog::createNewAccountDialog()
	: my_buttonCancel{ nullptr }, my_buttonConfirm{ nullptr }, my_cancelClose{ false }, my_confirmed{ false }, my_input{ nullptr },
	my_layout{ nullptr }, my_layoutButtons{ nullptr }, my_layoutInput{ nullptr }, my_widgetButtons{ nullptr }, my_widgetInput{ nullptr }
{
	// Create controls
	my_buttonCancel = new QPushButton{ "Cancel" };
	my_buttonConfirm = new QPushButton{ "Create" };
	my_input = new lineEdit;
	my_input->setEchoMode(QLineEdit::EchoMode::Password);
	my_label = new QLabel("Password");
	my_label->setBuddy(my_input);

	// Create main layout
	my_layout = new QVBoxLayout{ this };

	my_text = new QLabel("You can create a new account for this user name by confirming your password below.");
	my_layout->addWidget(my_text);

	// Create input layout
	my_widgetInput = new QWidget;
	my_layoutInput = new QHBoxLayout{ my_widgetInput };
	my_layoutInput->addWidget(my_label);
	my_layoutInput->addWidget(my_input);
	my_layout->addWidget(my_widgetInput);

	// Create button layout
	my_widgetButtons = new QWidget;
	my_layoutButtons = new QHBoxLayout{ my_widgetButtons };
	my_layoutButtons->addWidget(my_buttonConfirm);
	my_layoutButtons->addWidget(my_buttonCancel);
	my_layout->addWidget(my_widgetButtons);

	setWindowTitle("Create New Account");
	setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	// Hide info button
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	connect(my_input, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));
	connect(my_buttonCancel, SIGNAL(clicked()), this, SLOT(slotButtonCancelPressed()));
	connect(my_buttonConfirm, SIGNAL(clicked()), this, SLOT(slotButtonConfirmPressed()));
}

createNewAccountDialog::~createNewAccountDialog() {
	delete my_buttonCancel;
	delete my_buttonConfirm;
	delete my_input;

	delete my_layoutButtons;
	delete my_widgetButtons;

	delete my_layoutInput;
	delete my_widgetInput;

	delete my_layout;
}

void createNewAccountDialog::slotButtonConfirmPressed() { my_confirmed = true; Close(); }

void createNewAccountDialog::slotButtonCancelPressed() { my_confirmed = false;  close(); }

void createNewAccountDialog::slotReturnPressed() { my_confirmed = true; Close(); }

void createNewAccountDialog::Close(void) {
	if (my_input->text().length() == 0) { return; }

	//QCryptographicHash hash(QCryptographicHash::Algorithm::Sha256);
	//QString txt(my_input->text());
	//std::string str(txt.toStdString());
	//hash.addData(str.c_str(), str.length());
	//QByteArray arr(hash.result());
	//QByteArray result(arr.toHex());

	if (my_confirmed && my_password != my_input->text()) 
	{
		ak::uiAPI::promptDialog::show("The passwords do not match, please try again.", "Create New Account", ak::promptOkIconLeft, "DialogError", "Default", this);
		my_confirmed = false;
		return;
	}
	my_cancelClose = false;
	Q_EMIT isClosing();
	if (!my_cancelClose) { close(); }
	else { my_confirmed = false; }
}

// #############################################################################################################

createNewProjectDialog::createNewProjectDialog()
	: my_buttonCancel{ nullptr }, my_buttonConfirm{ nullptr }, my_cancelClose{ false }, my_confirmed{ false }, my_input{ nullptr },
	my_layout{ nullptr }, my_template{ nullptr }, my_layoutButtons{ nullptr }, my_widgetButtons{ nullptr }
{
	// Create controls
	my_buttonCancel = new QPushButton{ "Cancel" };
	my_buttonConfirm = new QPushButton{ "Create" };
	my_input = new lineEdit;
	my_input->setReadOnly(true);
	my_label = new QLabel("New project name");
	my_label->setBuddy(my_input);

	my_types = new QComboBox;
	my_label3 = new QLabel("Project type");
	my_label3->setBuddy(my_types);

	my_template = new QComboBox;
	my_label2 = new QLabel("Default settings template");
	my_label2->setBuddy(my_template);

	// Create main layout
	my_layout = new QVBoxLayout{ this };

	// Create input layout
	my_layout->addWidget(my_label);
	my_layout->addWidget(my_input);

	// Create template selection layout
	my_layout->addWidget(my_label3);
	my_layout->addWidget(my_types);

	// Create template selection layout
	my_layout->addWidget(my_label2);
	my_layout->addWidget(my_template);

	// Create button layout
	my_widgetButtons = new QWidget;
	my_layoutButtons = new QHBoxLayout{ my_widgetButtons };
	my_layoutButtons->addWidget(my_buttonConfirm);
	my_layoutButtons->addWidget(my_buttonCancel);
	my_layout->addWidget(my_widgetButtons);

	setWindowTitle("Create New Project");
	setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	// Hide info button
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	connect(my_input, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));
	connect(my_buttonCancel, SIGNAL(clicked()), this, SLOT(slotButtonCancelPressed()));
	connect(my_buttonConfirm, SIGNAL(clicked()), this, SLOT(slotButtonConfirmPressed()));
}

void createNewProjectDialog::setProjectName(const QString &name)
{ 
	my_input->setText(name);
}

void createNewProjectDialog::setTemplateList(std::vector<std::string> templates)
{
	my_template->addItem("<None>");

	for (auto item : templates)
	{
		my_template->addItem(item.c_str());
	}

	my_template->setEditText("<None>");
}

void createNewProjectDialog::setListOfProjectTypes(std::list<std::string>& projectTypes)
{
	my_types->addItem("Select project type...");

	for (auto item : projectTypes)
	{
		my_types->addItem(item.c_str());
	}

	my_types->setEditText("Select project type...");
}

std::string createNewProjectDialog::getTemplateName(void)
{
	std::string templateName = my_template->currentText().toStdString();

	if (templateName == "<None>") templateName.clear();

	return templateName;
}

std::string createNewProjectDialog::getProjectType(void)
{
	std::string projectType = my_types->currentText().toStdString();

	if (projectType == "Select project type...") projectType = OT_ACTION_PARAM_SESSIONTYPE_DEVELOPMENT;

	return projectType;
}

createNewProjectDialog::~createNewProjectDialog() {
	delete my_buttonCancel;
	delete my_buttonConfirm;
	delete my_input;
	delete my_template;
	delete my_types;

	delete my_layoutButtons;
	delete my_widgetButtons;

	delete my_layout;
}

void createNewProjectDialog::slotButtonConfirmPressed() { my_confirmed = true; Close(); }

void createNewProjectDialog::slotButtonCancelPressed() { my_confirmed = false;  close(); }

void createNewProjectDialog::slotReturnPressed() { my_confirmed = true; Close(); }

void createNewProjectDialog::Close(void) {
	if (my_input->text().length() == 0) { return; }

	std::string projectType = my_types->currentText().toStdString();
	if (projectType == "Select project type...") 
	{ 
		ak::uiAPI::promptDialog::show("Please specify a type for the new project.", "Create New Project", ak::promptOkIconLeft, "DialogError", "Default", this);
		return; 
	}

	my_cancelClose = false;
	Q_EMIT isClosing();
	if (!my_cancelClose) { close(); }
	else { my_confirmed = false; }
}
