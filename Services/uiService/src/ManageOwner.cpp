#include "ManageOwner.h"
#include "AppBase.h"

#include <akAPI/uiAPI.h>
#include <akWidgets/aTableWidget.h>

#include "OTCore/JSON.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/CheckBox.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

#include <qlayout.h>
#include <qtablewidget.h>
#include <qevent.h>

#include <array>
#include <cctype>

// ####################################################################################################
// Table Widget 

ManageOwnerTable::ManageOwnerTable()
	: QTableWidget(), my_selectedRow(-1)
{
	verticalHeader()->setVisible(false);
	setFocusPolicy(Qt::NoFocus);

	setMouseTracking(true);
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
}

ManageOwnerTable::ManageOwnerTable(int _rows, int _columns)
	: QTableWidget(_rows, _columns), my_selectedRow(-1)
{
	verticalHeader()->setVisible(false);
	setFocusPolicy(Qt::NoFocus);

	setMouseTracking(true);
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
}

ManageOwnerTable::~ManageOwnerTable() {

}

void ManageOwnerTable::addRow(const std::array<QTableWidgetItem *, 2> & _columns) {
	insertRow(rowCount());
	for (int c = 0; c < 2; c++) {
		setItem(rowCount() - 1, c, _columns[c]);
		Qt::ItemFlags f = _columns[c]->flags();
		f.setFlag(Qt::ItemFlag::ItemIsEditable, false);
		_columns[c]->setFlags(f);
	}
	my_dataRowItems.push_back(_columns);
}

void ManageOwnerTable::Clear() {
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

void ManageOwnerTable::mouseMoveEvent(QMouseEvent * _event) {
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

void ManageOwnerTable::leaveEvent(QEvent * _event) {
	for (auto r : my_dataRowItems) {
		for (auto c : r) {
			if (c->row() == my_selectedRow) {
			}
			else {
			}
		}
	}
}

void ManageOwnerTable::slotSelectionChanged() {
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

void ManageOwnerTable::getSelectedItems(QTableWidgetItem *&first, QTableWidgetItem *&second)
{
	first = second = nullptr;

	if (my_selectedRow < 0) return;

	first  = item(my_selectedRow, 0);
	second = item(my_selectedRow, 1);
}


// ####################################################################################################
// Main dialog box

ManageOwner::ManageOwner(const std::string &authServerURL, const std::string &assetType, const std::string &assetName, const std::string &ownerName) 
{
	m_authServerURL = authServerURL;
	m_assetType = assetType;
	m_assetName = assetName;
	m_assetOwner = ownerName;
	m_ownerCheckBox = nullptr;

	// Create controls
 	m_btnClose = new ot::PushButton("Close");

	m_buttonLabelLayoutW = new QWidget;
	m_buttonLabelLayout = new QHBoxLayout(m_buttonLabelLayoutW);
	m_buttonLabelLayout->setContentsMargins(0, 0, 0, 5);

	m_buttonLabelLayout->addStretch(1);
	m_buttonLabelLayout->addWidget(m_btnClose, Qt::Alignment::enum_type::AlignRight);
	m_buttonLabelLayout->setStretchFactor(m_btnClose, 0);

	// Create layouts
	m_centralLayout = new QVBoxLayout(this);

	m_groupLabelLayoutW = new QWidget;
	m_groupLabelLayout = new QHBoxLayout(m_groupLabelLayoutW);
	m_groupLabelLayout->setContentsMargins(0, 0, 0, 0);

	m_labelGroups = new ot::Label;
	m_labelGroups->setText(QString(assetType.c_str()) + " Owner");
	QFont font = m_labelGroups->font();
	font.setPointSize(font.pointSize() * 2);
	m_labelGroups->setFont(font);

	m_groupLabelLayout->addWidget(m_labelGroups);

	m_filterGroups = new ot::LineEdit;
	m_filterGroups->setPlaceholderText("Filter...");

	m_ownersList = new ManageOwnerTable(0, 2);

	QStringList membersLabels;
	membersLabels.push_back("Owner");
	membersLabels.push_back("Users");

	m_ownersList->setHorizontalHeaderLabels(membersLabels);

	m_ownersList->horizontalHeader()->setDefaultAlignment(Qt::Alignment::enum_type::AlignLeft | Qt::Alignment::enum_type::AlignVCenter);
	m_ownersList->horizontalHeaderItem(0)->setTextAlignment(Qt::Alignment::enum_type::AlignCenter | Qt::Alignment::enum_type::AlignVCenter);
	m_ownersList->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

	m_centralLayout->addWidget(m_groupLabelLayoutW);
	m_centralLayout->addWidget(m_filterGroups);
	m_centralLayout->addWidget(m_ownersList);

	int minWidth = m_labelGroups->fontMetrics().boundingRect(m_labelGroups->text()).width();
	
	setWindowTitle("Manage Owner of " + QString(assetType.c_str()) + ": " + QString(assetName.c_str()));
	setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	setMinimumSize(minWidth * 4, minWidth * 4);

	m_centralLayout->addWidget(m_buttonLabelLayoutW);

	connect(m_btnClose, &ot::PushButton::clicked, this, &ManageOwner::slotClose);
	connect(m_filterGroups, &ot::LineEdit::textChanged, this, &ManageOwner::slotGroupsFilter);
	connect(m_ownersList, &ManageOwnerTable::selectionChanged, this, &ManageOwner::slotGroupsSelection);

	readUserList();
	fillOwnerList();
}

ManageOwner::~ManageOwner() 
{
	m_centralLayout->deleteLater();
}

// ####################################################################################################

// Slots

void ManageOwner::slotClose(void) 
{
	this->close(ot::Dialog::Cancel);
}

void ManageOwner::slotShowGroupsWithAccessOnly(void)
{
	fillOwnerList();
}

void ManageOwner::slotGroupsFilter(void)
{
	fillOwnerList();
}

void ManageOwner::slotGroupsSelection(void)
{

}

bool ManageOwner::hasSuccessful(const std::string &response)
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

bool ManageOwner::hasError(const std::string &response)
{
	ot::JsonDocument doc;
	doc.fromJson(response);

	// Check whether the document has an error flag
	return ot::json::exists(doc, OT_ACTION_AUTH_ERROR);
}

std::string ManageOwner::tolower(std::string s) 
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
	return s;
}

void ManageOwner::readUserList(void)
{
	m_userList.clear();

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

		std::string userName = ot::json::getString(userDoc, OT_PARAM_AUTH_USER_NAME);

		m_userList.push_back(userName);
	}

	m_userList.sort();
}


void ManageOwner::fillOwnerList(void)
{
	// Store selection

	// Clear list
	m_ownersList->Clear();
	m_ownerCheckBox = nullptr;

	// Fill the list (considering filter and show group members only flag)
	AppBase * app{ AppBase::instance() };
	bool currentUserIsOwner = (app->getCurrentLoginData().getUserName() == m_assetOwner);


	std::string filterText = tolower(m_filterGroups->text().toStdString());

	for (auto userName : m_userList)
	{
		if (!filterText.empty())
		{
			if (tolower(userName).find(filterText) == userName.npos)
			{
				continue;
			}
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

		m_ownersList->addRow(dataRowItems);

		QWidget *pWidget = new QWidget();
		ot::CheckBox* pCheckBox = new ot::CheckBox;
		QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
		pLayout->addWidget(pCheckBox);
		pLayout->setAlignment(Qt::AlignCenter);
		pLayout->setContentsMargins(0,0,0,0);
		pWidget->setLayout(pLayout);
		m_ownersList->setCellWidget(m_ownersList->rowCount()-1, 0, pWidget);
		
		if (userName == m_assetOwner)
		{
			pCheckBox->setChecked(true);
			m_ownerCheckBox = pCheckBox;
		}

		if (!currentUserIsOwner)
		{
			pCheckBox->setEnabled(false);
		}

		connect(pCheckBox, &QCheckBox::clicked, this, [pCheckBox, this, row = m_ownersList->rowCount() - 1](bool flag) {slotGroupCheckBoxChanged(flag, row); });
	}

	// Restore selection if possible

}

void ManageGroupOwner::slotGroupCheckBoxChanged(bool state, int row)
{
	assert(!m_authServerURL.empty());

	std::string newOwner = m_ownersList->item(row, 1)->text().toStdString();

	if (m_ownerCheckBox != nullptr)
	{
		m_ownerCheckBox->setChecked(false);
	}

	std::string message = "After changing the owner, you will no longer be able to modify the group's properties.\n\n"
		"Are you sure to proceed?";

	if (ak::uiAPI::promptDialog::show(message.c_str(), "Change Owner", ak::promptType::promptOkCancelIconLeft, "DialogWarning", "Default", this) != ak::dialogResult::resultOk)
	{
		fillOwnerList();
		return;
	}

	AppBase * app{ AppBase::instance() };
	
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CHANGE_GROUP_OWNER, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_GROUP_NAME, ot::JsonString(m_assetName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_GROUP_OWNER_NEW_USER_USERNAME, ot::JsonString(newOwner, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		assert(0);
		return;
	}

	if (hasSuccessful(response))
	{
		m_assetOwner = newOwner;
	}
	else
	{
		ak::uiAPI::promptDialog::show("The owner could not be changed.", "Change Owner", ak::promptType::promptOkIconLeft, "DialogError", "Default", this);
	}

	fillOwnerList();
}

void ManageProjectOwner::slotGroupCheckBoxChanged(bool state, int row)
{
	assert(!m_authServerURL.empty());

	std::string newOwner = m_ownersList->item(row, 1)->text().toStdString();

	if (m_ownerCheckBox != nullptr)
	{
		m_ownerCheckBox->setChecked(false);
	}

	std::string message = "After changing the owner, you might no longer be able to access the project.\n\n"
		"Are you sure to proceed?";

	if (ak::uiAPI::promptDialog::show(message.c_str(), "Change Owner", ak::promptType::promptOkCancelIconLeft, "DialogWarning", "Default", this) != ak::dialogResult::resultOk)
	{
		fillOwnerList();
		return;
	}

	AppBase * app{ AppBase::instance() };
	
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CHANGE_PROJECT_OWNER, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(m_assetName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_NEW_PROJECT_OWNER, ot::JsonString(newOwner, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		assert(0);
		return;
	}

	if (hasError(response))
	{
		ak::uiAPI::promptDialog::show("The owner could not be changed.", "Change Owner", ak::promptType::promptOkIconLeft, "DialogError", "Default", this);
	}
	else
	{
		m_assetOwner = newOwner;
	}

	fillOwnerList();
}

