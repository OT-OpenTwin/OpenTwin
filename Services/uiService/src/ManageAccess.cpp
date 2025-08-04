// Frontend header
#include "ManageAccess.h"
#include "AppBase.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTCore/JSON.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/PushButton.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qheaderview.h>

// std header
#include <array>
#include <cctype>

// ####################################################################################################
// Table Widget 

ManageAccessTable::ManageAccessTable()
	: my_selectedRow(-1)
{
	verticalHeader()->setVisible(false);
	setFocusPolicy(Qt::NoFocus);

	setMouseTracking(true);
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
}

ManageAccessTable::ManageAccessTable(int _rows, int _columns)
	: ot::Table(_rows, _columns), my_selectedRow(-1)
{
	verticalHeader()->setVisible(false);
	setFocusPolicy(Qt::NoFocus);

	setMouseTracking(true);
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
}

ManageAccessTable::~ManageAccessTable() {

}

void ManageAccessTable::addRow(const std::array<QTableWidgetItem *, 2> & _columns) {
	insertRow(rowCount());
	for (int c = 0; c < 2; c++) {
		setItem(rowCount() - 1, c, _columns[c]);
		Qt::ItemFlags f = _columns[c]->flags();
		f.setFlag(Qt::ItemFlag::ItemIsEditable, false);
		_columns[c]->setFlags(f);
	}
	my_dataRowItems.push_back(_columns);
}

void ManageAccessTable::Clear() {
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

void ManageAccessTable::mouseMoveEvent(QMouseEvent * _event) {
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

void ManageAccessTable::leaveEvent(QEvent * _event) {
	for (auto r : my_dataRowItems) {
		for (auto c : r) {
			if (c->row() == my_selectedRow) {
			}
			else {
			}
		}
	}
}

void ManageAccessTable::slotSelectionChanged() {
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

void ManageAccessTable::getSelectedItems(QTableWidgetItem *&first, QTableWidgetItem *&second)
{
	first = second = nullptr;

	if (my_selectedRow < 0) return;

	first  = item(my_selectedRow, 0);
	second = item(my_selectedRow, 1);
}


// ####################################################################################################

// Main dialog box

ManageAccess::ManageAccess(const std::string &authServerURL, const std::string &projectName) 
{
	m_authServerURL = authServerURL;
	m_projectName = projectName;

	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);
	QHBoxLayout* groupLayout = new QHBoxLayout;
	QHBoxLayout* buttonLayout = new QHBoxLayout;

	// Create controls
	ot::PushButton* closeButton = new ot::PushButton("Close");

	ot::Label* labelGroups = new ot::Label("User Groups");

	m_filterGroups = new ot::LineEdit;
	m_filterGroups->setPlaceholderText("Filter...");

	m_showGroupsWithAccessOnly = new ot::CheckBox("Show groups with access only");
	m_showGroupsWithAccessOnly->setChecked(true);
	m_showGroupsWithAccessOnly->setMinimumSize(QSize(0, 0));
	m_showGroupsWithAccessOnly->setContentsMargins(0, 0, 0, 0);

	m_groupsList = new ManageAccessTable(0, 2);

	// Setup controls
	QFont font = labelGroups->font();
	font.setPointSize(font.pointSize() * 2);
	labelGroups->setFont(font);

	QStringList membersLabels;
	membersLabels.push_back("Access");
	membersLabels.push_back("User Group");

	m_groupsList->setHorizontalHeaderLabels(membersLabels);

	m_groupsList->horizontalHeader()->setDefaultAlignment(Qt::Alignment::enum_type::AlignLeft | Qt::Alignment::enum_type::AlignVCenter);
	m_groupsList->horizontalHeaderItem(0)->setTextAlignment(Qt::Alignment::enum_type::AlignCenter | Qt::Alignment::enum_type::AlignVCenter);
	m_groupsList->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

	int minWidth = labelGroups->fontMetrics().boundingRect(labelGroups->text()).width();

	// Setup layouts
	groupLayout->setContentsMargins(0, 0, 0, 0);
	buttonLayout->setContentsMargins(0, 0, 0, 5);

	// Fill layouts
	buttonLayout->addStretch(1);
	buttonLayout->addWidget(closeButton, Qt::Alignment::enum_type::AlignRight);

	groupLayout->addWidget(labelGroups);
	groupLayout->addStretch(1);
	groupLayout->addWidget(m_showGroupsWithAccessOnly);

	centralLayout->addLayout(groupLayout);
	centralLayout->addWidget(m_filterGroups);
	centralLayout->addWidget(m_groupsList);
	centralLayout->addLayout(buttonLayout);

	// Setup window

	this->setWindowTitle("Manage Access To Project: " + QString(projectName.c_str()));
	this->setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	this->setMinimumSize(minWidth * 4, minWidth * 4);

	// Connect signals
	connect(closeButton, &ot::PushButton::clicked, this, &ManageAccess::closeCancel);
	connect(m_showGroupsWithAccessOnly, &ot::CheckBox::stateChanged, this, &ManageAccess::slotShowGroupsWithAccessOnly);
	connect(m_filterGroups, &ot::LineEdit::textChanged, this, &ManageAccess::slotGroupsFilter);
	connect(m_groupsList, &ManageAccessTable::selectionChanged, this, &ManageAccess::slotGroupsSelection);

	this->readGroupsList();
	this->fillGroupsList();
}

ManageAccess::~ManageAccess() {
	
}

// ####################################################################################################

// Slots

void ManageAccess::slotShowGroupsWithAccessOnly(void)
{
	fillGroupsList();
}

void ManageAccess::slotGroupsFilter(void)
{
	fillGroupsList();
}

void ManageAccess::slotGroupsSelection(void)
{

}

void ManageAccess::slotGroupCheckBoxChanged(bool state, int row)
{
	assert(!m_authServerURL.empty());

	std::string groupName = m_groupsList->item(row, 1)->text().toStdString();

	AppBase * app{ AppBase::instance() };
	
	ot::JsonDocument doc;
	if (state)
	{
		// We need to add this user to the group
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_ADD_GROUP_TO_PROJECT, doc.GetAllocator()), doc.GetAllocator());
	}
	else
	{
		// We need to remove the user from the group
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_REMOVE_GROUP_FROM_PROJECT, doc.GetAllocator()), doc.GetAllocator());
	}

	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_GROUP_NAME, ot::JsonString(groupName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(m_projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to authorization service");
		AppBase::instance()->showErrorPrompt("Failed to send request to Authorization Service.", "Authorization Service url: \"" + m_authServerURL + "\"", "Network Error");
		exit(ot::AppExitCode::SendFailed);
		return;
	}

	assert(hasSuccessful(response));
}

bool ManageAccess::hasSuccessful(const std::string &response)
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

std::string ManageAccess::tolower(std::string s) 
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
	return s;
}

void ManageAccess::fillGroupsList(void)
{
	// Store selection

	// Clear list
	m_groupsList->Clear();

	AppBase * app{ AppBase::instance() };

	// Add new content to list
	assert(!m_authServerURL.empty());

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_PROJECT_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(m_projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to authorization service");
		AppBase::instance()->showErrorPrompt("Failed to send request to Authorization Service.", "Authorization Service url: \"" + m_authServerURL + "\"", "Network Error");
		exit(ot::AppExitCode::SendFailed);
		return;
	}
	ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(response);
	if (responseMessage == ot::ReturnMessage::Failed)
	{
		return;
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(responseMessage.getWhat());

	// Reset in group flag for all members
	for (auto user : m_groupList)
	{
		m_groupHasAccess[user] = false;
	}

	// Now update the group flag according to the group membership
	const rapidjson::Value& groupArray = responseDoc[ "groups" ];
	assert(groupArray.IsArray());

	for (rapidjson::Value::ConstValueIterator itr = groupArray.Begin(); itr != groupArray.End(); ++itr)
	{
		const rapidjson::Value& group = *itr;
		std::string groupName = group.GetString();

		m_groupHasAccess[groupName] = true;
	}

	// Fill the list (considering filter and show group members only flag)

	std::string filterText = tolower(m_filterGroups->text().toStdString());

	for (auto groupName : m_groupList)
	{
		if (!filterText.empty())
		{
			if (tolower(groupName).find(filterText) == groupName.npos)
			{
				continue;
			}
		}

		if (m_showGroupsWithAccessOnly->isChecked() && !m_groupHasAccess[groupName])
		{
			continue;
		}

		std::array<QTableWidgetItem *, 2> dataRowItems;

		{
			QTableWidgetItem * hItm = new QTableWidgetItem();
			dataRowItems[0] = hItm;
		}

		{
			QTableWidgetItem * hItm = new QTableWidgetItem(groupName.c_str());
			auto flags = hItm->flags();
			flags.setFlag(Qt::ItemFlag::ItemIsEditable, false);
			hItm->setFlags(flags);
			dataRowItems[1] = hItm;
		}

		m_groupsList->addRow(dataRowItems);

		QWidget *pWidget = new QWidget();
		QCheckBox *pCheckBox = new QCheckBox();
		QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
		pLayout->addWidget(pCheckBox);
		pLayout->setAlignment(Qt::AlignCenter);
		pLayout->setContentsMargins(0,0,0,0);
		pWidget->setLayout(pLayout);
		m_groupsList->setCellWidget(m_groupsList->rowCount()-1, 0, pWidget);
		
		if (m_groupHasAccess[groupName])
		{
			pCheckBox->setChecked(true);
		}

		connect(pCheckBox, &QCheckBox::clicked, this, [pCheckBox, this, row = m_groupsList->rowCount() - 1](bool flag) {slotGroupCheckBoxChanged(flag, row); });
	}

	// Restore selection if possible

}

void ManageAccess::readGroupsList(void)
{
	m_groupList.clear();
	m_groupHasAccess.clear();

	// Add new content to list
	assert(!m_authServerURL.empty());

	AppBase * app{ AppBase::instance() };

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_ALL_USER_GROUPS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send request to authorization service");
		AppBase::instance()->showErrorPrompt("Failed to send request to Authorization Service.", "Authorization Service url: \"" + m_authServerURL + "\"", "Network Error");
		exit(ot::AppExitCode::SendFailed);
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

		m_groupList.push_back(groupName);
		m_groupHasAccess[groupName] = false;
	}

	m_groupList.sort();
}
