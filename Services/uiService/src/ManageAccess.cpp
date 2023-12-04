#include "ManageAccess.h"
#include "AppBase.h"

#include <akAPI/uiAPI.h>
#include <akWidgets/aPushButtonWidget.h>
#include <akWidgets/aLabelWidget.h>
#include <akWidgets/aLineEditWidget.h>
#include <akWidgets/aTableWidget.h>
#include <akWidgets/aCheckBoxWidget.h>
#include <akWidgets/aPropertyGridWidget.h>

#include <akGui/aColorStyle.h>

#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "OTCore/ReturnMessage.h"

#include <qlayout.h>
#include <qtablewidget.h>
#include <qevent.h>

#include <array>
#include <cctype>

// ####################################################################################################
// Table Widget 

ManageAccessTable::ManageAccessTable()
	: QTableWidget(), my_selectedRow(-1),
	my_colorBack(255, 255, 255), my_colorFocusBack(0, 0, 255), my_colorFocusFront(0, 0, 0),
	my_colorFront(0, 0, 0), my_colorSelectedBack(0, 255, 0), my_colorSelectedFront(0, 0, 0)
{
	verticalHeader()->setVisible(false);
	setFocusPolicy(Qt::NoFocus);

	setMouseTracking(true);
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
}

ManageAccessTable::ManageAccessTable(int _rows, int _columns)
	: QTableWidget(_rows, _columns), my_selectedRow(-1),
	my_colorBack(255, 255, 255), my_colorFocusBack(0, 0, 255), my_colorFocusFront(0, 0, 0),
	my_colorFront(0, 0, 0), my_colorSelectedBack(0, 255, 0), my_colorSelectedFront(0, 0, 0)
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
		_columns[c]->setBackground(my_colorBack);
		_columns[c]->setForeground(my_colorFront);
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

void ManageAccessTable::leaveEvent(QEvent * _event) {
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

void ManageAccessTable::setColorStyle(ak::aColorStyle *	_colorStyle) {
	assert(_colorStyle != nullptr);

	QString sheet = _colorStyle->toStyleSheet(ak::cafBackgroundColorControls | ak::cafForegroundColorControls,
		"QTableWidget{", "selection-color: transparent; selection-background-color: transparent;}");

	setShowGrid(false);
	setStyleSheet(sheet);
	my_colorBack = _colorStyle->getWindowMainBackgroundColor().toQColor();
	my_colorFront = _colorStyle->getControlsMainForegroundColor().toQColor();
	my_colorFocusBack = _colorStyle->getControlsFocusedBackgroundColor().toQColor();
	my_colorFocusFront = _colorStyle->getControlsFocusedForegroundColor().toQColor();
	my_colorSelectedBack = _colorStyle->getControlsPressedBackgroundColor().toQColor();
	my_colorSelectedFront = _colorStyle->getControlsPressedForegroundColor().toQColor();

	// Table header
	sheet = _colorStyle->toStyleSheet(ak::cafForegroundColorHeader | ak::cafBackgroundColorHeader,
		"QHeaderView{border: none;", "}\n");
	sheet.append(_colorStyle->toStyleSheet(ak::cafForegroundColorHeader |
		ak::cafBackgroundColorHeader |
		ak::cafDefaultBorderHeader | ak::cafBorderColorHeader
		,
		"QHeaderView::section{", "}"));
	horizontalHeader()->setStyleSheet(sheet);
	verticalHeader()->setStyleSheet(sheet);
	slotSelectionChanged();
}

void ManageAccessTable::slotSelectionChanged() {
	my_selectedRow = -1;
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
	emit selectionChanged();
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

	m_groupLabelLayoutW = new QWidget;
	m_groupLabelLayout = new QHBoxLayout(m_groupLabelLayoutW);
	m_groupLabelLayout->setContentsMargins(0, 0, 0, 0);

	m_labelGroups = new ak::aLabelWidget;
	m_labelGroups->setText("User Groups");
	QFont font = m_labelGroups->font();
	font.setPointSize(font.pointSize() * 2);
	m_labelGroups->setFont(font);

	m_showGroupsWithAccessOnly = new ak::aCheckBoxWidget;
	m_showGroupsWithAccessOnly->setText("Show groups with access only");
	m_showGroupsWithAccessOnly->setChecked(true);
	m_showGroupsWithAccessOnly->setMinimumSize(QSize(0, 0));
	m_showGroupsWithAccessOnly->setContentsMargins(0, 0, 0, 0);

	m_groupLabelLayout->addWidget(m_labelGroups);
	m_groupLabelLayout->addStretch(1);
	m_groupLabelLayout->addWidget(m_showGroupsWithAccessOnly);

	m_filterGroups = new ak::aLineEditWidget;
	m_filterGroups->setPlaceholderText("Filter...");

	m_groupsList = new ManageAccessTable(0, 2);

	QStringList membersLabels;
	membersLabels.push_back("Access");
	membersLabels.push_back("User Group");

	m_groupsList->setHorizontalHeaderLabels(membersLabels);

	m_groupsList->horizontalHeader()->setDefaultAlignment(Qt::Alignment::enum_type::AlignLeft | Qt::Alignment::enum_type::AlignVCenter);
	m_groupsList->horizontalHeaderItem(0)->setTextAlignment(Qt::Alignment::enum_type::AlignCenter | Qt::Alignment::enum_type::AlignVCenter);
	m_groupsList->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

	m_centralLayout->addWidget(m_groupLabelLayoutW);
	m_centralLayout->addWidget(m_filterGroups);
	m_centralLayout->addWidget(m_groupsList);

	int minWidth = m_labelGroups->fontMetrics().boundingRect(m_labelGroups->text()).width();
	
	setWindowTitle("Manage Access To Project: " + QString(projectName.c_str()));
	setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

	hideInfoButton();
	setMinimumSize(minWidth * 4, minWidth * 4);

	m_centralLayout->addWidget(m_buttonLabelLayoutW);

	connect(m_btnClose, &ak::aPushButtonWidget::clicked, this, &ManageAccess::slotClose);
	connect(m_showGroupsWithAccessOnly, &ak::aCheckBoxWidget::stateChanged, this, &ManageAccess::slotShowGroupsWithAccessOnly);
	connect(m_filterGroups, &ak::aLineEditWidget::textChanged, this, &ManageAccess::slotGroupsFilter);
	connect(m_groupsList, &ManageAccessTable::selectionChanged, this, &ManageAccess::slotGroupsSelection);

	readGroupsList();
	fillGroupsList();
}

ManageAccess::~ManageAccess() 
{
	m_centralLayout->deleteLater();
}

void ManageAccess::setColorStyle(ak::aColorStyle *_colorStyle) 
{
	aDialog::setColorStyle(_colorStyle);

	if (m_btnClose) { m_btnClose->setColorStyle(m_colorStyle); }
	if (m_labelGroups) { m_labelGroups->setColorStyle(m_colorStyle); }
	if (m_filterGroups) { m_filterGroups->setColorStyle(m_colorStyle); }
	if (m_groupsList) { m_groupsList->setColorStyle(m_colorStyle); }
	if (m_showGroupsWithAccessOnly) { m_showGroupsWithAccessOnly->setColorStyle(m_colorStyle); }
}

// ####################################################################################################

// Slots

void ManageAccess::slotClose(void) 
{
	Close(ak::resultCancel);
}

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

	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCredentialUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCredentialUserPasswordClear(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_GROUP_NAME, ot::JsonString(groupName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(m_projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		assert(0);
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
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCredentialUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCredentialUserPasswordClear(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(m_projectName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", m_authServerURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response))
	{
		assert(0);
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
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(app->getCredentialUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(app->getCredentialUserPasswordClear(), doc.GetAllocator()), doc.GetAllocator());

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

		m_groupList.push_back(groupName);
		m_groupHasAccess[groupName] = false;
	}

	m_groupList.sort();
}
