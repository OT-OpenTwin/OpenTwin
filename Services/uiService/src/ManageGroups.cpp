// Frontend header
#include "ManageGroups.h"
#include "ManageOwner.h"
#include "AppBase.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/IconManager.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qheaderview.h>

// std header
#include <array>
#include <cctype>

// ####################################################################################################
// Table Widget 

ManageGroupsTable::ManageGroupsTable()
	: m_selectedRow(-1)
{
	verticalHeader()->setVisible(false);
	setFocusPolicy(Qt::NoFocus);

	setMouseTracking(true);
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
}

ManageGroupsTable::ManageGroupsTable(int _rows, int _columns)
	: ot::Table(_rows, _columns), m_selectedRow(-1)
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
	m_dataRowItems.push_back(_columns);
}

void ManageGroupsTable::Clear() {
	for (auto itm : m_dataRowItems) {
		for (auto cell : itm) {
			QTableWidgetItem * actualCell = cell;
			delete actualCell;
		}
	}
	m_dataRowItems.clear();
	setRowCount(0);
	m_selectedRow = -1;
}

void ManageGroupsTable::mouseMoveEvent(QMouseEvent * _event) {
	QTableWidgetItem * itm = itemAt(_event->pos());
	if (itm == nullptr) {
		for (auto r : m_dataRowItems) {
			for (auto c : r) {
				if (c->row() == m_selectedRow) {
				}
				else {
				}
			}
		}
	}
	else {
		for (auto r : m_dataRowItems) {
			for (auto c : r) {
				if (c->row() == itm->row()) {
				}
				else if (c->row() == m_selectedRow) {
				}
				else {
				}
			}
		}
	}
}

void ManageGroupsTable::leaveEvent(QEvent * _event) {
	for (auto r : m_dataRowItems) {
		for (auto c : r) {
			if (c->row() == m_selectedRow) {
			}
			else {
			}
		}
	}
}

void ManageGroupsTable::slotSelectionChanged() {
	m_selectedRow = -1;
	disconnect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
	QList<QTableWidgetItem *> selection = selectedItems();
	for (auto itm : selection) {
		for (auto c : m_dataRowItems.at(itm->row())) {
			c->setSelected(false);
		}
		m_selectedRow = itm->row();
	}
	for (int r = 0; r < m_dataRowItems.size(); r++) {
		if (r != m_selectedRow) {
			for (auto c : m_dataRowItems.at(r)) {
			}
		}
	}
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
	Q_EMIT selectionChanged();
}

void ManageGroupsTable::getSelectedItems(QTableWidgetItem *&first, QTableWidgetItem *&second)
{
	first = second = nullptr;

	if (m_selectedRow < 0) return;

	first  = item(m_selectedRow, 0);
	second = item(m_selectedRow, 1);
}

// ####################################################################################################

// Add group dialog

AddGroupDialog::AddGroupDialog(const std::string &authServerURL) {
	m_authServerURL = authServerURL;

	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);
	QHBoxLayout* inputLayout = new QHBoxLayout;
	QHBoxLayout* buttonsLayout = new QHBoxLayout;

	// Create controls
	ot::PushButton* okButton = new ot::PushButton("Ok");
	ot::PushButton* cancelButton = new ot::PushButton("Cancel");

	m_input = new ot::LineEdit;
	ot::Label* groupLabel = new ot::Label("Group name:");
	groupLabel->setBuddy(m_input);

	// Setup layouts
	inputLayout->addWidget(groupLabel);
	inputLayout->addWidget(m_input);

	buttonsLayout->addWidget(okButton);
	buttonsLayout->addWidget(cancelButton);

	centralLayout->addLayout(inputLayout);
	centralLayout->addLayout(buttonsLayout);

	// Setup window
	this->setWindowTitle("Create New Group");
	this->setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	// Connect signals
	connect(m_input, &ot::LineEdit::returnPressed, this, &AddGroupDialog::slotConfirm);
	connect(okButton, &ot::PushButton::clicked, this, &AddGroupDialog::slotConfirm);
	connect(cancelButton, &ot::PushButton::clicked, this, &AddGroupDialog::closeCancel);
}

AddGroupDialog::~AddGroupDialog() {

}

QString AddGroupDialog::groupName(void) const {
	return m_input->text();
}

void AddGroupDialog::slotConfirm()
{ 
	// Add new group
	AppBase* app = AppBase::instance();
	OTAssertNullptr(app);

	if (m_input->text().length() == 0) {
		app->showErrorPrompt("Please specify a name for the new group", "Create New Group");
		return;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CREATE_GROUP, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_GROUP_NAME, ot::JsonString(groupName().toStdString(), doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response)) {
		OT_LOG_EA("Failed to send message");
		return;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);
	
	if (hasError(response)) {
		app->showErrorPrompt("The new group can not be created (the name may already be in use)", "Create New Group");
		return;
	}

	this->closeOk();
}

bool AddGroupDialog::hasError(const std::string &response) {
	ot::JsonDocument doc;
	doc.fromJson(response);

	// Check whether the document has an error flag
	return ot::json::exists(doc, OT_ACTION_AUTH_ERROR);
}

// ####################################################################################################

// Rename group dialog

RenameGroupDialog::RenameGroupDialog(const std::string &groupName, const std::string &authServerURL) {
	m_groupToRename = groupName;
	m_authServerURL = authServerURL;

	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);
	QHBoxLayout* inputLayout = new QHBoxLayout;
	QHBoxLayout* buttonsLayout = new QHBoxLayout;

	// Create controls
	ot::PushButton* cancelButton = new ot::PushButton("Cancel");
	ot::PushButton* confirmButton = new ot::PushButton("Confirm");
	
	m_input = new ot::LineEdit(QString::fromStdString(m_groupToRename));

	ot::Label* inputLabel = new ot::Label("Name");
	inputLabel->setBuddy(m_input);

	// Setup layouts
	inputLayout->addWidget(inputLabel);
	inputLayout->addWidget(m_input);

	buttonsLayout->addWidget(confirmButton);
	buttonsLayout->addWidget(cancelButton);

	centralLayout->addLayout(inputLayout);
	centralLayout->addLayout(buttonsLayout);

	// Setup window
	this->setWindowTitle("Rename Group");
	this->setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	// Connect signals
	connect(m_input, &ot::LineEdit::returnPressed, this, &RenameGroupDialog::slotConfirm);
	connect(cancelButton, &ot::PushButton::clicked, this, &RenameGroupDialog::closeCancel);
	connect(confirmButton, &ot::PushButton::clicked, this, &RenameGroupDialog::slotConfirm);
}

RenameGroupDialog::~RenameGroupDialog() {

}

QString RenameGroupDialog::groupName(void) const
{ 
	return m_input->text(); 
}

void RenameGroupDialog::slotConfirm() {
	AppBase* app{ AppBase::instance() };
	OTAssertNullptr(app);

	// Add new group
	if (m_input->text().length() == 0) {
		app->showErrorPrompt("Please specify a new name for the group", "Rename Group");
		return;
	}

	if (m_groupToRename == m_input->text().toStdString()) {
		app->showWarningPrompt("The new name of the new group must be different from the original.", "Rename Group");
		return;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CHANGE_GROUP_NAME, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_GROUP_NAME, ot::JsonString(m_groupToRename, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_NEW_GROUP_NAME, ot::JsonString(m_input->text().toStdString(), doc.GetAllocator()), doc.GetAllocator());
	
	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response)) {
		OT_LOG_EA("Failed to send request");
		return;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	if (!hasSuccessful(response)) {
		app->showErrorPrompt("The new group can not be renamed (the new name may already be in use)", "Rename Group");
		return;
	}

	this->closeOk();
}

bool RenameGroupDialog::hasSuccessful(const std::string &response)
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

	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);
	QHBoxLayout* listFrameLayout = new QHBoxLayout;
	QVBoxLayout* listLeftLayout = new QVBoxLayout;
	QVBoxLayout* listRightLayout = new QVBoxLayout;
	QHBoxLayout* groupLabelLayout = new QHBoxLayout;
	
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->setContentsMargins(0, 0, 0, 5);

	QHBoxLayout* memberLabelLayout = new QHBoxLayout;
	memberLabelLayout->setContentsMargins(0, 0, 0, 0);

	// Create controls
 	ot::PushButton* closeButton = new ot::PushButton("Close");

	ot::PushButton* btnAdd = new ot::PushButton("");
	btnAdd->setIcon(ot::IconManager::getIcon("Default/NewGroup.png"));
	btnAdd->setToolTip("Create new group");

	m_btnDelete = new ot::PushButton("");
	m_btnDelete->setIcon(ot::IconManager::getIcon("Default/Delete.png"));
	m_btnDelete->setToolTip("Delete selected group");

	m_btnRename = new ot::PushButton("");
	m_btnRename->setIcon(ot::IconManager::getIcon("Default/RenameItem.png"));
	m_btnRename->setToolTip("Rename selected group");

	m_btnOwner = new ot::PushButton("");
	m_btnOwner->setIcon(ot::IconManager::getIcon("Default/ChangeOwner.png"));
	m_btnOwner->setToolTip("Change owner of selected group");

	m_groupsList = new ManageGroupsTable(0, 2);

	m_filterGroups = new ot::LineEdit;

	ot::Label* labelGroups = new ot::Label("My Groups");
	ot::Label* labelMembers = new ot::Label("Group Members");

	m_showMembersOnly = new ot::CheckBox("Show group members only");

	m_filterMembers = new ot::LineEdit;

	m_membersList = new ManageGroupsTable(0, 2);

	// Setup controls
	QFont font = labelGroups->font();
	font.setPointSize(font.pointSize() * 2);
	labelGroups->setFont(font);

	QStringList groupsLabels;
	groupsLabels.push_back("Group Name");
	groupsLabels.push_back("Owner");

	labelMembers->setFont(font);

	m_groupsList->setHorizontalHeaderLabels(groupsLabels);

	m_filterGroups->setPlaceholderText("Filter...");

	m_groupsList->horizontalHeader()->setDefaultAlignment(Qt::Alignment::enum_type::AlignLeft | Qt::Alignment::enum_type::AlignVCenter);
	m_groupsList->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

	m_showMembersOnly->setChecked(true);
	m_showMembersOnly->setMinimumSize(QSize(0, 0));
	m_showMembersOnly->setContentsMargins(0, 0, 0, 0);

	m_filterMembers->setPlaceholderText("Filter...");

	QStringList membersLabels;
	membersLabels.push_back("In Group");
	membersLabels.push_back("User Name");

	m_membersList->setHorizontalHeaderLabels(membersLabels);

	m_membersList->horizontalHeader()->setDefaultAlignment(Qt::Alignment::enum_type::AlignLeft | Qt::Alignment::enum_type::AlignVCenter);
	m_membersList->horizontalHeaderItem(0)->setTextAlignment(Qt::Alignment::enum_type::AlignCenter | Qt::Alignment::enum_type::AlignVCenter);
	m_membersList->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

	// Setup layouts
	listFrameLayout->setContentsMargins(0, 0, 0, 5);
	listLeftLayout->setContentsMargins(0, 0, 10, 0);
	listRightLayout->setContentsMargins(10, 0, 0, 0);
	groupLabelLayout->setContentsMargins(0, 0, 0, 0);
	
	groupLabelLayout->addWidget(labelGroups);
	groupLabelLayout->addStretch(1);
	groupLabelLayout->addWidget(btnAdd);
	groupLabelLayout->addWidget(m_btnRename);
	groupLabelLayout->addWidget(m_btnOwner);
	groupLabelLayout->addWidget(m_btnDelete);

	buttonLayout->addStretch(1);
	buttonLayout->addWidget(closeButton, Qt::AlignmentFlag::AlignRight);
	
	memberLabelLayout->addWidget(labelMembers);
	memberLabelLayout->addStretch(1);
	memberLabelLayout->addWidget(m_showMembersOnly);

	listLeftLayout->addLayout(groupLabelLayout);
	listLeftLayout->addWidget(m_filterGroups);
	listLeftLayout->addWidget(m_groupsList);

	listRightLayout->addLayout(memberLabelLayout);
	listRightLayout->addWidget(m_filterMembers);
	listRightLayout->addWidget(m_membersList);

	listFrameLayout->addLayout(listLeftLayout);
	listFrameLayout->addLayout(listRightLayout);

	centralLayout->addLayout(listFrameLayout);
	centralLayout->addLayout(buttonLayout);

	// Setup window
	int minWidth = labelGroups->fontMetrics().boundingRect(labelGroups->text()).width();
	
	setWindowTitle("Manage User Groups");
	setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());
	
	setMinimumSize(minWidth * 10, minWidth * 8);

	connect(closeButton, &ot::PushButton::clicked, this, &ManageGroups::closeCancel);
	connect(btnAdd, &ot::PushButton::clicked, this, &ManageGroups::slotAddGroup);
	connect(m_btnRename, &ot::PushButton::clicked, this, &ManageGroups::slotRenameGroup);
	connect(m_btnOwner, &ot::PushButton::clicked, this, &ManageGroups::slotChangeGroupOwner);
	connect(m_btnDelete, &ot::PushButton::clicked, this, &ManageGroups::slotDeleteGroup);
	connect(m_showMembersOnly, &ot::CheckBox::stateChanged, this, &ManageGroups::slotShowMembersOnly);
	connect(m_filterGroups, &ot::LineEdit::textChanged, this, &ManageGroups::slotGroupFilter);
	connect(m_filterMembers, &ot::LineEdit::textChanged, this, &ManageGroups::slotMemberFilter);
	connect(m_groupsList, &ManageGroupsTable::selectionChanged, this, &ManageGroups::slotGroupsSelection);
	
	readUserList();

	fillGroupsList();
	fillMembersList();
}

ManageGroups::~ManageGroups() {
	
}

// ####################################################################################################

// Slots

void ManageGroups::slotAddGroup(void)
{
	AddGroupDialog dialog(m_authServerURL);

	if (dialog.showDialog() == ot::Dialog::Ok)
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

	RenameGroupDialog dialog(groupName, m_authServerURL);

	if (dialog.showDialog() == ot::Dialog::Ok) {
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

	ot::MessageDialogCfg config;
	config.setText("Are you sure to delete group: " + groupName + "\nThis operation can not be undone.");
	config.setTitle("Delete Group");
	config.setIcon(ot::MessageDialogCfg::Warning);
	config.setButtons(ot::MessageDialogCfg::Yes | ot::MessageDialogCfg::No);

	if (AppBase::instance()->showPrompt(config) == ot::MessageDialogCfg::Yes) {
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

void ManageGroups::slotShowMembersOnly(void) {
	fillMembersList();
}

void ManageGroups::slotGroupFilter(void) {
	fillGroupsList();
}

void ManageGroups::slotMemberFilter(void) {
	fillMembersList();
}

void ManageGroups::slotGroupsSelection(void) {
	fillMembersList();
}

void ManageGroups::slotMemberCheckBoxChanged(bool state, int row) {
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
		OT_LOG_EAS("Failed to send request");
		return;
	}

	if (!hasSuccessful(response)) {
		OT_LOG_EA("Request returned error");
	}
}

bool ManageGroups::hasSuccessful(const std::string &response) {
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
