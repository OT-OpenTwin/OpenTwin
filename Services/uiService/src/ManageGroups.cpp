#include "ManageGroups.h"
#include "ManageOwner.h"
#include "AppBase.h"

#include <akAPI/uiAPI.h>
#include <akWidgets/aWindow.h>
#include <akWidgets/aPushButtonWidget.h>
#include <akWidgets/aLabelWidget.h>
#include <akWidgets/aLineEditWidget.h>
#include <akWidgets/aTableWidget.h>
#include <akWidgets/aCheckBoxWidget.h>

#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

#include <qlayout.h>
#include <qtablewidget.h>
#include <qevent.h>

#include <array>
#include <cctype>

// ####################################################################################################
// Table Widget 

ManageGroupsTable::ManageGroupsTable()
	: QTableWidget(), my_selectedRow(-1)
{
	verticalHeader()->setVisible(false);
	setFocusPolicy(Qt::NoFocus);

	setMouseTracking(true);
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
}

ManageGroupsTable::ManageGroupsTable(int _rows, int _columns)
	: QTableWidget(_rows, _columns), my_selectedRow(-1)
{
	verticalHeader()->setVisible(false);
	setFocusPolicy(Qt::NoFocus);

	setMouseTracking(true);
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
}

ManageGroupsTable::~ManageGroupsTable() {

}

void ManageGroupsTable::addRow(const std::array<QTableWidgetItem *, 2> & _columns) {
	insertRow(rowCount());
	for (int c = 0; c < 2; c++) {
		setItem(rowCount() - 1, c, _columns[c]);
		Qt::ItemFlags f = _columns[c]->flags();
		f.setFlag(Qt::ItemFlag::ItemIsEditable, false);
		_columns[c]->setFlags(f);
	}
	my_dataRowItems.push_back(_columns);
}

void ManageGroupsTable::Clear() {
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

void ManageGroupsTable::mouseMoveEvent(QMouseEvent * _event) {
	QTableWidgetItem * itm = itemAt(_event->pos());
	if (itm == nullptr) {
		for (auto r : my_dataRowItems) {
			for (auto c : r) {
				if (c->row() == my_selectedRow) {
				}
				else {
				}
			}
		}
	}
	else {
		for (auto r : my_dataRowItems) {
			for (auto c : r) {
				if (c->row() == itm->row()) {
				}
				else if (c->row() == my_selectedRow) {
				}
				else {
				}
			}
		}
	}
}

void ManageGroupsTable::leaveEvent(QEvent * _event) {
	for (auto r : my_dataRowItems) {
		for (auto c : r) {
			if (c->row() == my_selectedRow) {
			}
			else {
			}
		}
	}
}

void ManageGroupsTable::slotSelectionChanged() {
	my_selectedRow = -1;
	disconnect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
	QList<QTableWidgetItem *> selection = selectedItems();
	for (auto itm : selection) {
		for (auto c : my_dataRowItems.at(itm->row())) {
			c->setSelected(false);
		}
		my_selectedRow = itm->row();
	}
	for (int r = 0; r < my_dataRowItems.size(); r++) {
		if (r != my_selectedRow) {
			for (auto c : my_dataRowItems.at(r)) {
			}
		}
	}
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
	Q_EMIT selectionChanged();
}

void ManageGroupsTable::getSelectedItems(QTableWidgetItem *&first, QTableWidgetItem *&second)
{
	first = second = nullptr;

	if (my_selectedRow < 0) return;

	first  = item(my_selectedRow, 0);
	second = item(my_selectedRow, 1);
}

// ####################################################################################################
// Add group dialog

addGroupDialog::addGroupDialog(const std::string &authServerURL)
	: my_buttonCancel{ nullptr }, my_buttonConfirm{ nullptr }, my_confirmed{ false }, my_input{ nullptr },
	my_layout{ nullptr }, my_layoutButtons{ nullptr }, my_layoutInput{ nullptr }, my_widgetButtons{ nullptr }, my_widgetInput{ nullptr }
{

	m_authServerURL = authServerURL;

	// Create controls
	my_buttonCancel = new ak::aPushButtonWidget{ "Cancel" };
	my_buttonConfirm = new ak::aPushButtonWidget{ "OK" };
	my_input = new ak::aLineEditWidget;
	my_label = new ak::aLabelWidget("Group name");
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

	setWindowTitle("Create New Group");
	setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	//setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	// Hide info button
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	connect(my_input, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));
	connect(my_buttonCancel, SIGNAL(clicked()), this, SLOT(slotButtonCancelPressed()));
	connect(my_buttonConfirm, SIGNAL(clicked()), this, SLOT(slotButtonConfirmPressed()));

	my_confirmed = false;
}

addGroupDialog::~addGroupDialog() 
{
	delete my_buttonCancel;
	delete my_buttonConfirm;
	delete my_input;

	delete my_layoutButtons;
	delete my_widgetButtons;

	delete my_layoutInput;
	delete my_widgetInput;

	delete my_layout;
}

QString addGroupDialog::groupName(void) const { return my_input->text(); }

void addGroupDialog::slotButtonConfirmPressed() 
{ 
	// Add new group
	if (my_input->text().length() == 0)
	{
		ak::uiAPI::promptDialog::show("Please specify a name for the new group", "Create New Group", ak::promptType::promptOkIconLeft, "DialogError", "Default", this);
		return;
	}

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CREATE_GROUP, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_GROUP_NAME, ot::JsonString(groupName().toStdString(), doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		assert(0);
		return;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);
	
	if (hasError(response))
	{
		ak::uiAPI::promptDialog::show("The new group can not be created (the name may already be in use)", "Create New Group", ak::promptType::promptOkIconLeft, "DialogError", "Default", this);
		return;
	}

	my_confirmed = true; 
	Close(); 
}

bool addGroupDialog::hasError(const std::string &response)
{
	ot::JsonDocument doc;
	doc.fromJson(response);

	// Check whether the document has an error flag
	return ot::json::exists(doc, OT_ACTION_AUTH_ERROR);
}

void addGroupDialog::slotButtonCancelPressed() 
{
	my_confirmed = false;  
	close(); 
}

void addGroupDialog::slotReturnPressed() 
{
	Q_EMIT my_buttonConfirm->clicked();
}

void addGroupDialog::Close(void) 
{
	Q_EMIT isClosing();
	close();
}


// ####################################################################################################
// Rename group dialog

renameGroupDialog::renameGroupDialog(const std::string &groupName, const std::string &authServerURL)
	: my_buttonCancel{ nullptr }, my_buttonConfirm{ nullptr }, my_cancelClose{ false }, my_confirmed{ false }, my_input{ nullptr },
	my_layout{ nullptr }, my_layoutButtons{ nullptr }, my_layoutInput{ nullptr }, my_widgetButtons{ nullptr }, my_widgetInput{ nullptr }
{
	my_groupToRename = groupName;
	m_authServerURL = authServerURL;

	// Create controls
	my_buttonCancel = new ak::aPushButtonWidget{ "Cancel" };
	my_buttonConfirm = new ak::aPushButtonWidget{ "Confirm" };
	
	my_input = new ak::aLineEditWidget;
	my_input->setText(my_groupToRename.c_str());

	my_label = new ak::aLabelWidget("Name");
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

	setWindowTitle("Rename Group");
	setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	// Hide info button
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	connect(my_input, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));
	connect(my_buttonCancel, SIGNAL(clicked()), this, SLOT(slotButtonCancelPressed()));
	connect(my_buttonConfirm, SIGNAL(clicked()), this, SLOT(slotButtonConfirmPressed()));
}

renameGroupDialog::~renameGroupDialog() 
{
	delete my_buttonCancel;
	delete my_buttonConfirm;
	delete my_input;

	delete my_layoutButtons;
	delete my_widgetButtons;

	delete my_layoutInput;
	delete my_widgetInput;

	delete my_layout;
}

QString renameGroupDialog::groupName(void) const 
{ 
	return my_input->text(); 
}

void renameGroupDialog::slotButtonConfirmPressed() 
{ 
	// Add new group
	if (my_input->text().length() == 0)
	{
		ak::uiAPI::promptDialog::show("Please specify a new name for the group", "Rename Group", ak::promptType::promptOkIconLeft, "DialogError", "Default", this);
		return;
	}

	if (my_groupToRename == my_input->text().toStdString()) 
	{
		ak::uiAPI::promptDialog::show("The new name of the new group must be different from the original.", "Rename Group", ak::promptType::promptOkIconLeft, "DialogError", "Default", this);
		my_confirmed = false;
		return;
	}

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CHANGE_GROUP_NAME, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_GROUP_NAME, ot::JsonString(my_groupToRename, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_NEW_GROUP_NAME, ot::JsonString(my_input->text().toStdString(), doc.GetAllocator()), doc.GetAllocator());
	
	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		assert(0);
		return;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	if (!hasSuccessful(response))
	{
		ak::uiAPI::promptDialog::show("The new group can not be renamed (the new name may already be in use)", "Rename Group", ak::promptType::promptOkIconLeft, "DialogError", "Default", this);
		return;
	}

	my_confirmed = true; 
	Close(); 
}

void renameGroupDialog::slotButtonCancelPressed() 
{ 
	my_confirmed = false;
	close(); 
}

void renameGroupDialog::slotReturnPressed() 
{
	Q_EMIT my_buttonConfirm->clicked();
 }

void renameGroupDialog::Close(void) 
{
	my_cancelClose = false;
	Q_EMIT isClosing();
	if (!my_cancelClose) { close(); }
	else { my_confirmed = false; }
}

bool renameGroupDialog::hasSuccessful(const std::string &response)
{
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	try
	{
		bool success = ot::json::getBool(responseDoc, OT_ACTION_AUTH_SUCCESS);
		return success;
	}
	catch (std::exception)
	{
		return false; // The return document does not have a success flag
	}
}

// ####################################################################################################
// Main dialog box

ManageGroups::ManageGroups(const std::string &authServerURL) 
{
	m_authServerURL = authServerURL;

	// Create controls
 	m_btnClose = new ak::aPushButtonWidget("Close");

	m_buttonLabelLayoutW = new QWidget;
	m_buttonLabelLayout = new QHBoxLayout(m_buttonLabelLayoutW);
	m_buttonLabelLayout->setContentsMargins(0, 0, 0, 5);

	m_buttonLabelLayout->addStretch(1);
	m_buttonLabelLayout->addWidget(m_btnClose, Qt::Alignment::enum_type::AlignRight);
	m_buttonLabelLayout->setStretchFactor(m_btnClose, 0);

	// Create layouts
	m_centralLayout = new QVBoxLayout(this);

	m_listFrameLayoutW = new QWidget;
	m_listFrameLayout = new QHBoxLayout(m_listFrameLayoutW);
	m_listFrameLayout->setContentsMargins(0, 0, 0, 5);

	m_listLeftLayoutW = new QWidget;
	m_listLeftLayout = new QVBoxLayout(m_listLeftLayoutW);
	m_listLeftLayout->setContentsMargins(0, 0, 10, 0);

	m_listRightLayoutW = new QWidget;
	m_listRightLayout = new QVBoxLayout(m_listRightLayoutW);
	m_listRightLayout->setContentsMargins(10, 0, 0, 0);

	m_listFrameLayout->addWidget(m_listLeftLayoutW);
	m_listFrameLayout->addWidget(m_listRightLayoutW);

	m_groupLabelLayoutW = new QWidget;
	m_groupLabelLayout = new QHBoxLayout(m_groupLabelLayoutW);
	m_groupLabelLayout->setContentsMargins(0, 0, 0, 0);

	m_labelGroups = new ak::aLabelWidget;
	m_labelGroups->setText("My Groups");
	QFont font = m_labelGroups->font();
	font.setPointSize(font.pointSize() * 2);
	m_labelGroups->setFont(font);

	m_btnAdd = new ak::aPushButtonWidget("");
	m_btnAdd->setIcon( ak::uiAPI::getIcon("NewGroup", "Default"));
	m_btnAdd->setToolTip("Create new group");

	m_btnDelete = new ak::aPushButtonWidget("");
	m_btnDelete->setIcon( ak::uiAPI::getIcon("Delete", "Default"));
	m_btnDelete->setToolTip("Delete selected group");

	m_btnRename = new ak::aPushButtonWidget("");
	m_btnRename->setIcon( ak::uiAPI::getIcon("RenameItem", "Default"));
	m_btnRename->setToolTip("Rename selected group");

	m_btnOwner = new ak::aPushButtonWidget("");
	m_btnOwner->setIcon( ak::uiAPI::getIcon("ChangeOwner", "Default"));
	m_btnOwner->setToolTip("Change owner of selected group");

	m_groupLabelLayout->addWidget(m_labelGroups);
	m_groupLabelLayout->addStretch(1);
	m_groupLabelLayout->addWidget(m_btnAdd);
	m_groupLabelLayout->addWidget(m_btnRename);
	m_groupLabelLayout->addWidget(m_btnOwner);
	m_groupLabelLayout->addWidget(m_btnDelete);

	m_groupsList = new ManageGroupsTable(0, 2);

	QStringList groupsLabels;
	groupsLabels.push_back("Group Name");
	groupsLabels.push_back("Owner");

	m_groupsList->setHorizontalHeaderLabels(groupsLabels);

	m_filterGroups = new ak::aLineEditWidget;
	m_filterGroups->setPlaceholderText("Filter...");

	m_groupsList->horizontalHeader()->setDefaultAlignment(Qt::Alignment::enum_type::AlignLeft | Qt::Alignment::enum_type::AlignVCenter);
	m_groupsList->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

	m_listLeftLayout->addWidget(m_groupLabelLayoutW);
	m_listLeftLayout->addWidget(m_filterGroups);
	m_listLeftLayout->addWidget(m_groupsList);

	m_memberLabelLayoutW = new QWidget;
	m_memberLabelLayout = new QHBoxLayout(m_memberLabelLayoutW);
	m_memberLabelLayout->setContentsMargins(0, 0, 0, 0);

	m_labelMembers = new ak::aLabelWidget;
	m_labelMembers->setText("Group Members");
	m_labelMembers->setFont(font);

	m_showMembersOnly = new ak::aCheckBoxWidget;
	m_showMembersOnly->setText("Show group members only");
	m_showMembersOnly->setChecked(true);
	m_showMembersOnly->setMinimumSize(QSize(0, 0));
	m_showMembersOnly->setContentsMargins(0, 0, 0, 0);

	m_memberLabelLayout->addWidget(m_labelMembers);
	m_memberLabelLayout->addStretch(1);
	m_memberLabelLayout->addWidget(m_showMembersOnly);

	m_filterMembers = new ak::aLineEditWidget;
	m_filterMembers->setPlaceholderText("Filter...");

	m_membersList = new ManageGroupsTable(0, 2);

	QStringList membersLabels;
	membersLabels.push_back("In Group");
	membersLabels.push_back("User Name");

	m_membersList->setHorizontalHeaderLabels(membersLabels);

	m_membersList->horizontalHeader()->setDefaultAlignment(Qt::Alignment::enum_type::AlignLeft | Qt::Alignment::enum_type::AlignVCenter);
	m_membersList->horizontalHeaderItem(0)->setTextAlignment(Qt::Alignment::enum_type::AlignCenter | Qt::Alignment::enum_type::AlignVCenter);
	m_membersList->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

	m_listRightLayout->addWidget(m_memberLabelLayoutW);
	m_listRightLayout->addWidget(m_filterMembers);
	m_listRightLayout->addWidget(m_membersList);

	int minWidth = m_labelGroups->fontMetrics().boundingRect(m_labelGroups->text()).width();
	
	setWindowTitle("Manage User Groups");
	setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());
	
	hideInfoButton();
	setMinimumSize(minWidth * 10, minWidth * 8);

	m_centralLayout->addWidget(m_listFrameLayoutW);
	m_centralLayout->addWidget(m_buttonLabelLayoutW);

	connect(m_btnClose, &ak::aPushButtonWidget::clicked, this, &ManageGroups::slotClose);
	connect(m_btnAdd, &ak::aPushButtonWidget::clicked, this, &ManageGroups::slotAddGroup);
	connect(m_btnRename, &ak::aPushButtonWidget::clicked, this, &ManageGroups::slotRenameGroup);
	connect(m_btnOwner, &ak::aPushButtonWidget::clicked, this, &ManageGroups::slotChangeGroupOwner);
	connect(m_btnDelete, &ak::aPushButtonWidget::clicked, this, &ManageGroups::slotDeleteGroup);
	connect(m_showMembersOnly, &ak::aCheckBoxWidget::stateChanged, this, &ManageGroups::slotShowMembersOnly);
	connect(m_filterGroups, &ak::aLineEditWidget::textChanged, this, &ManageGroups::slotGroupFilter);
	connect(m_filterMembers, &ak::aLineEditWidget::textChanged, this, &ManageGroups::slotMemberFilter);
	connect(m_groupsList, &ManageGroupsTable::selectionChanged, this, &ManageGroups::slotGroupsSelection);
	connect(m_membersList, &ManageGroupsTable::selectionChanged, this, &ManageGroups::slotMembersSelection);

	readUserList();

	fillGroupsList();
	fillMembersList();
}

ManageGroups::~ManageGroups() 
{
	m_centralLayout->deleteLater();
}

// ####################################################################################################

// Slots

void ManageGroups::slotClose(void) 
{
	Close(ak::resultCancel);
}

void ManageGroups::slotAddGroup(void)
{
	addGroupDialog dialog(m_authServerURL);

	dialog.exec();

	if (dialog.wasConfirmed())
	{
		fillGroupsList();
	}
}

void ManageGroups::slotRenameGroup(void)
{
	QTableWidgetItem *groupNameItem = nullptr;
	QTableWidgetItem *groupOwnerItem = nullptr;
	m_groupsList->getSelectedItems(groupNameItem, groupOwnerItem);

	if (groupNameItem == nullptr || groupOwnerItem == nullptr) return;

	std::string groupName = groupNameItem->text().toStdString();

	renameGroupDialog dialog(groupName, m_authServerURL);

	dialog.exec();

	if (dialog.wasConfirmed())
	{
		fillGroupsList();
	}
}

void ManageGroups::slotChangeGroupOwner(void)
{
	QTableWidgetItem *groupNameItem = nullptr;
	QTableWidgetItem *groupOwnerItem = nullptr;
	m_groupsList->getSelectedItems(groupNameItem, groupOwnerItem);

	if (groupNameItem == nullptr || groupOwnerItem == nullptr) return;

	std::string groupName = groupNameItem->text().toStdString();
	std::string groupOwner = groupOwnerItem->text().toStdString();

	ManageGroupOwner ownerManager(m_authServerURL, groupName, groupOwner);

	ownerManager.showDialog();

	fillGroupsList();
}

void ManageGroups::slotDeleteGroup(void)
{
	QTableWidgetItem *groupNameItem = nullptr;
	QTableWidgetItem *groupOwnerItem = nullptr;
	m_groupsList->getSelectedItems(groupNameItem, groupOwnerItem);

	if (groupNameItem == nullptr || groupOwnerItem == nullptr) return;

	std::string groupName = groupNameItem->text().toStdString();

	if (ak::uiAPI::promptDialog::show("Are you sure to delete group: " + QString(groupName.c_str()) + "\nThis operation can not be undone.", "Delete Group", ak::promptType::promptOkCancelIconLeft, "DialogWarning", "Default", this) == ak::dialogResult::resultOk)
	{
		// Delete the group
		assert(!m_authServerURL.empty());

		AppBase * app{ AppBase::instance() };

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_REMOVE_GROUP, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_PARAM_AUTH_GROUP_NAME, ot::JsonString(groupName, doc.GetAllocator()), doc.GetAllocator());

		std::string response;
		if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
		{
			assert(0);
			return;
		}

		assert(hasSuccessful(response));

		fillGroupsList();
	}
}

void ManageGroups::slotShowMembersOnly(void)
{
	fillMembersList();
}

void ManageGroups::slotGroupFilter(void)
{
	fillGroupsList();
}

void ManageGroups::slotMemberFilter(void)
{
	fillMembersList();
}

void ManageGroups::slotGroupsSelection(void)
{
	fillMembersList();
}

void ManageGroups::slotMembersSelection(void)
{

}

void ManageGroups::slotMemberCheckBoxChanged(bool state, int row)
{
	assert(!m_authServerURL.empty());

	QTableWidgetItem *groupNameItem = nullptr;
	QTableWidgetItem *groupOwnerItem = nullptr;
	m_groupsList->getSelectedItems(groupNameItem, groupOwnerItem);

	if (groupNameItem == nullptr || groupOwnerItem == nullptr) return;

	std::string groupName = groupNameItem->text().toStdString();
	std::string userName = m_membersList->item(row, 1)->text().toStdString();

	AppBase * app{ AppBase::instance() };
	ot::JsonDocument doc;

	if (state)
	{
		// We need to add this user to the group
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_ADD_USER_TO_GROUP, doc.GetAllocator()), doc.GetAllocator());
	}
	else
	{
		// We need to remove the user from the group
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_REMOVE_USER_FROM_GROUP, doc.GetAllocator()), doc.GetAllocator());
	}

	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_GROUP_NAME, ot::JsonString(groupName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(userName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		assert(0);
		return;
	}

	assert(hasSuccessful(response));
}

bool ManageGroups::hasSuccessful(const std::string &response)
{
	ot::JsonDocument doc;
	doc.fromJson(response);

	try
	{
		bool success = ot::json::getBool(doc, OT_ACTION_AUTH_SUCCESS);
		return success;
	}
	catch (std::exception)
	{
		return false; // The return document does not have a success flag
	}
}

void ManageGroups::fillGroupsList(void)
{
	// Store selection

	// Clear list
	m_groupsList->Clear();

	// Add new content to list
	assert(!m_authServerURL.empty());

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_ALL_USER_GROUPS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		assert(0);
		return;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::string filterText = tolower(m_filterGroups->text().toStdString());

	const rapidjson::Value& groupArray = responseDoc[ "groups" ];
	assert(groupArray.IsArray());

	std::list<std::string> groupNames;
	std::map<std::string, std::string> groupOwners;

	for (rapidjson::Value::ConstValueIterator itr = groupArray.Begin(); itr != groupArray.End(); ++itr)
	{
		const rapidjson::Value& group = *itr;
		std::string groupData = group.GetString();

		ot::JsonDocument groupDoc;
		groupDoc.fromJson(groupData);

		std::string groupName = groupDoc[OT_PARAM_AUTH_GROUP].GetString();
		std::string groupOwner = groupDoc[OT_PARAM_AUTH_GROUPOWNER].GetString();
		
		groupNames.push_back(groupName);
		groupOwners[groupName] = groupOwner;
	}

	groupNames.sort();

	for (auto groupName : groupNames)
	{
		std::string groupOwner = groupOwners[groupName];

		if (!filterText.empty())
		{
			if (tolower(groupName).find(filterText) == groupName.npos)
			{
				continue;
			}
		}

		std::array<QTableWidgetItem *, 2> dataRowItems;

		{
			QTableWidgetItem * hItm = new QTableWidgetItem(groupName.c_str());
			auto flags = hItm->flags();
			flags.setFlag(Qt::ItemFlag::ItemIsEditable, false);
			hItm->setFlags(flags);
			dataRowItems[0] = hItm;
		}

		{
			QTableWidgetItem * hItm = new QTableWidgetItem(groupOwner.c_str());
			auto flags = hItm->flags();
			flags.setFlag(Qt::ItemFlag::ItemIsEditable, false);
			hItm->setFlags(flags);
			dataRowItems[1] = hItm;
		}

		m_groupsList->addRow(dataRowItems);
	}

	// Restore selection if possible


	fillMembersList();
}

std::string ManageGroups::tolower(std::string s) 
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
	return s;
}

void ManageGroups::fillMembersList(void)
{
	// Store selection

	// Clear list
	m_membersList->Clear();

	QTableWidgetItem *groupNameItem = nullptr;
	QTableWidgetItem *groupOwnerItem = nullptr;
	m_groupsList->getSelectedItems(groupNameItem, groupOwnerItem);

	if (groupNameItem == nullptr || groupOwnerItem == nullptr)
	{
		m_btnRename->setEnabled(false);
		m_btnOwner->setEnabled(false);
		m_btnDelete->setEnabled(false);
		return;
	}

	std::string groupName = groupNameItem->text().toStdString();
	std::string groupOwner = groupOwnerItem->text().toStdString();

	AppBase * app{ AppBase::instance() };

	if (app->getCurrentLoginData().getUserName() != groupOwner)
	{
		m_btnRename->setEnabled(false);
		m_btnOwner->setEnabled(false);
		m_btnDelete->setEnabled(false);
	}
	else
	{
		m_btnRename->setEnabled(true);
		m_btnOwner->setEnabled(true);
		m_btnDelete->setEnabled(true);
	}

	// Add new content to list
	assert(!m_authServerURL.empty());

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_GROUP_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_GROUP_NAME, ot::JsonString(groupName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		assert(0);
		return;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	// Reset in group flag for all members
	for (auto user : m_userInGroup)
	{
		m_userInGroup[user.first] = false;
	}

	// Now update the group flag according to the group membership
	const rapidjson::Value& userArray = responseDoc[ "users" ];
	assert(userArray.IsArray());

	for (rapidjson::Value::ConstValueIterator itr = userArray.Begin(); itr != userArray.End(); ++itr)
	{
		const rapidjson::Value& user = *itr;
		std::string userName = user.GetString();

		m_userInGroup[userName] = true;
	}

	// Fill the list (considering filter and show group members only flag)

	std::string filterText = tolower(m_filterMembers->text().toStdString());

	m_userList.sort();

	for (auto userName : m_userList)
	{
		if (!filterText.empty())
		{
			if (tolower(userName).find(filterText) == userName.npos)
			{
				continue;
			}
		}

		if (m_showMembersOnly->isChecked() && !m_userInGroup[userName])
		{
			continue;
		}

		std::array<QTableWidgetItem *, 2> dataRowItems;

		{
			QTableWidgetItem * hItm = new QTableWidgetItem();
			dataRowItems[0] = hItm;
		}

		{
			QTableWidgetItem * hItm = new QTableWidgetItem(userName.c_str());
			auto flags = hItm->flags();
			flags.setFlag(Qt::ItemFlag::ItemIsEditable, false);
			hItm->setFlags(flags);
			dataRowItems[1] = hItm;
		}

		m_membersList->addRow(dataRowItems);

		QWidget *pWidget = new QWidget();
		QCheckBox *pCheckBox = new QCheckBox();
		QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
		pLayout->addWidget(pCheckBox);
		pLayout->setAlignment(Qt::AlignCenter);
		pLayout->setContentsMargins(0,0,0,0);
		pWidget->setLayout(pLayout);
		m_membersList->setCellWidget(m_membersList->rowCount()-1, 0, pWidget);
		
		if (m_userInGroup[userName])
		{
			pCheckBox->setChecked(true);
		}

		if (userName == groupOwner)
		{
			pCheckBox->setEnabled(false);
		}

		if (app->getCurrentLoginData().getUserName() != groupOwner)
		{
			pCheckBox->setEnabled(false);
		}

		connect(pCheckBox, &QCheckBox::clicked, this, [pCheckBox, this, row = m_membersList->rowCount() - 1](bool flag) {slotMemberCheckBoxChanged(flag, row); });
	}

	// Restore selection if possible


}

void ManageGroups::readUserList(void)
{
	m_userList.clear();
	m_userInGroup.clear();

	assert(!m_authServerURL.empty());

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_ALL_USERS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		assert(0);
		return;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	const rapidjson::Value& groupArray = responseDoc[ "users" ];
	assert(groupArray.IsArray());

	for (rapidjson::Value::ConstValueIterator itr = groupArray.Begin(); itr != groupArray.End(); ++itr)
	{
		const rapidjson::Value& user = *itr;
		std::string userData = user.GetString();

		ot::JsonDocument userDoc;
		userDoc.fromJson(userData);

		std::string userName = userDoc[OT_PARAM_AUTH_USER_NAME].GetString();

		m_userList.push_back(userName);
		m_userInGroup[userName] = false;
	}
}


