/*
 *	File:		aPropertyGridWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: August 2, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// uiCore header
#include <akWidgets/aPropertyGridWidget.h>
#include <akWidgets/aTableWidget.h>
#include <akWidgets/aLabelWidget.h>
#include <akWidgets/aCheckBoxWidget.h>
#include <akWidgets/aColorEditButtonWidget.h>
#include <akWidgets/aComboButtonWidget.h>
#include <akWidgets/aTimePickWidget.h>
#include <akWidgets/aDatePickWidget.h>
#include <akWidgets/aPushButtonWidget.h>
#include <akWidgets/aSpinBoxWidget.h>

// Qt header
#include <qlayout.h>
#include <qmenu.h>

#include <limits>

enum tableColumns {
	tcName,
	tcValue,
	tcCount
};

ak::aPropertyGridWidget::aPropertyGridWidget(QWidget * _parent)
	: aWidget(otPropertyGrid), m_currentID(invalidID), m_groupIconsSet(false), m_isEnabled(true)
{
	// Setup main layout
	m_centralLayoutW = new QWidget(_parent);
	m_centralLayout = new QVBoxLayout(m_centralLayoutW);

	m_centralLayoutW->setObjectName("AK_aPropertyGrid_CentralLayoutW");
	m_centralLayout->setObjectName("AK_aPropertyGrid_CentralLayout");

	m_centralLayoutW->setContentsMargins(0, 0, 0, 0);
	m_centralLayout->setContentsMargins(0, 0, 0, 0);

	// Setup table
	m_table = new aTableWidget(0, tcCount);
	QTableWidgetItem * nItm1 = new QTableWidgetItem("Name");
	nItm1->setFlags(nItm1->flags() ^ Qt::ItemIsEditable ^ Qt::ItemIsSelectable);
	m_table->setHorizontalHeaderItem(tcName, nItm1);
	

	QTableWidgetItem * nItm2 = new QTableWidgetItem("Value");
	nItm2->setFlags(nItm2->flags() ^ Qt::ItemIsEditable ^ Qt::ItemIsSelectable);
	m_table->setHorizontalHeaderItem(tcValue, nItm2);

	m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	m_table->setRowHeaderIsVisible(false);
	m_table->setVisible(false);
	m_table->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	m_table->setObjectName("AK_aPropertyGrid_Table");

	m_stretch = new QWidget;

	m_label = new aLabelWidget("No items");
	m_label->setAlignment(Qt::AlignCenter);
	m_label->setObjectName("AK_aPropertyGrid_Label");
	m_label->setMargin(6);

	m_centralLayout->addWidget(m_label);
	m_centralLayout->addWidget(m_stretch, 1);

	// Create default group
	m_defaultGroup = new aPropertyGridGroup(this, "", aColor(255, 255, 255), aColor(0, 0, 0), aColor(255, 0, 0));
	m_groups.insert_or_assign("", m_defaultGroup);

	connect(m_table, &aTableWidget::focusLost, this, &aPropertyGridWidget::slotFocusOut);
}

ak::aPropertyGridWidget::~aPropertyGridWidget() {

}

// ##############################################################################################

// Base class functions

// ##############################################################################################

// Item creation

void ak::aPropertyGridWidget::addGroup(const QString& _groupName, const aColor& _groupColor, const aColor& _foregroundColor, const aColor& _errorColor) {
	auto itm = m_groups.find(_groupName);
	if (itm != m_groups.end()) {
		assert(0); // A group with the provided name does already exist
		return;
	}
	aPropertyGridGroup * g = new aPropertyGridGroup(this, _groupName, _groupColor, _foregroundColor, _errorColor);
	g->refreshStateIcons();

	m_groups.insert_or_assign(_groupName, g);
}

ak::ID ak::aPropertyGridWidget::addItem(const QString& _groupName, const QString& _itemName, bool _value, bool _isMultipleValues) {
	aPropertyGridGroup * g = group(_groupName);
	if (g) {
		aPropertyGridItem * nItm = new aPropertyGridItem(m_table, g->lastItemRow() + 1, _itemName, _value, _isMultipleValues);
		return newItemCreated(g, nItm);
	}
	return invalidID;
}

ak::ID ak::aPropertyGridWidget::addItem(const QString& _groupName, const QString& _itemName, int _value, bool _isMultipleValues) {
	aPropertyGridGroup * g = group(_groupName);
	if (g) {
		aPropertyGridItem * nItm = new aPropertyGridItem(m_table, g->lastItemRow() + 1, _itemName, _value, aPropertyGridItem::TextForm, INT_MIN, INT_MAX, _isMultipleValues);
		return newItemCreated(g, nItm);
	}
	return invalidID;
}

ak::ID ak::aPropertyGridWidget::addItem(const QString& _groupName, const QString& _itemName, int _value, int _min, int _max, bool _isMultipleValues) {
	aPropertyGridGroup * g = group(_groupName);
	if (g) {
		aPropertyGridItem * nItm = new aPropertyGridItem(m_table, g->lastItemRow() + 1, _itemName, _value, aPropertyGridItem::NumericUpDown, _min, _max, _isMultipleValues);
		return newItemCreated(g, nItm);
	}
	return invalidID;
}

ak::ID ak::aPropertyGridWidget::addItem(const QString& _groupName, const QString& _itemName, double _value, bool _isMultipleValues) {
	aPropertyGridGroup * g = group(_groupName);
	if (g) {
		aPropertyGridItem * nItm = new aPropertyGridItem(m_table, g->lastItemRow() + 1, _itemName, _value, _isMultipleValues);
		return newItemCreated(g, nItm);
	}
	return invalidID;
}

ak::ID ak::aPropertyGridWidget::addItem(const QString& _groupName, const QString& _itemName, const QString& _value, bool _isMultipleValues) {
	aPropertyGridGroup * g = group(_groupName);
	if (g) {
		aPropertyGridItem * nItm = new aPropertyGridItem(m_table, g->lastItemRow() + 1, _itemName, _value, _isMultipleValues);
		return newItemCreated(g, nItm);
	}
	return invalidID;
}

ak::ID ak::aPropertyGridWidget::addItem(const QString& _groupName, const QString& _itemName, const aTime& _value, timeFormat _timeFormat, bool _isMultipleValues) {
	aPropertyGridGroup * g = group(_groupName);
	if (g) {
		aPropertyGridItem * nItm = new aPropertyGridItem(m_table, g->lastItemRow() + 1, _itemName, _value, _timeFormat, _isMultipleValues);
		return newItemCreated(g, nItm);
	}
	return invalidID;
}

ak::ID ak::aPropertyGridWidget::addItem(const QString& _groupName, const QString& _itemName, const aDate& _value, dateFormat _dateFormat, bool _isMultipleValues) {
	aPropertyGridGroup * g = group(_groupName);
	if (g) {
		aPropertyGridItem * nItm = new aPropertyGridItem(m_table, g->lastItemRow() + 1, _itemName, _value, _dateFormat, _isMultipleValues);
		return newItemCreated(g, nItm);
	}
	return invalidID;
}

ak::ID ak::aPropertyGridWidget::addItem(const QString& _groupName, const QString& _itemName, const aColor& _value, bool _isMultipleValues) {
	aPropertyGridGroup * g = group(_groupName);
	if (g) {
		aPropertyGridItem * nItm = new aPropertyGridItem(m_table, g->lastItemRow() + 1, _itemName, _value, _isMultipleValues);
		return newItemCreated(g, nItm);
	}
	return invalidID;
}

ak::ID ak::aPropertyGridWidget::addItem(const QString& _groupName, const QString& _itemName, const QStringList& _possibleSelection, const QString& _value, bool _isMultipleValues) {
	aPropertyGridGroup * g = group(_groupName);
	if (g) {
		aPropertyGridItem * nItm = new aPropertyGridItem(m_table, g->lastItemRow() + 1, _itemName, _possibleSelection, _value, _isMultipleValues);
		return newItemCreated(g, nItm);
	}
	return invalidID;
}

// ##############################################################################################

// Manipulator

void ak::aPropertyGridWidget::setEnabled(bool _isEnabled) {
	m_isEnabled = _isEnabled;
	for (auto itm : m_items) { itm.second->setEnabled(m_isEnabled); }
}

void ak::aPropertyGridWidget::setGroupIcons(const QIcon& _expand, const QIcon& _collapse) {
	m_iconGroupCollapse = _collapse;
	m_iconGroupExpand = _expand;
	m_groupIconsSet = true;
	for (auto group : m_groups) {
		group.second->refreshStateIcons();
	}
}

void ak::aPropertyGridWidget::setDeleteIcon(const QIcon& _delete) {
	m_iconDelete = _delete;
}

void ak::aPropertyGridWidget::clear(bool _keepGroups) {
	if (_keepGroups) {
		for (auto g : m_groups) {
			g.second->clear();
		}
	}
	else {
		for (auto g : m_groups) {
			if (m_defaultGroup->title() != g.second->title()) {
				delete g.second;
			}
			else {
				m_defaultGroup->clear();
			}
		}
		for (auto itm : m_items) {
			delete itm.second;
		}
		m_groups.clear();
		m_items.clear();
		m_groups.insert_or_assign(m_defaultGroup->title(), m_defaultGroup);
		m_table->setRowCount(1);
	}

	updateTableVisibility();

	emit cleared();
}

// ##############################################################################################

// Getter

ak::aPropertyGridGroup * ak::aPropertyGridWidget::group(const QString& _groupName) {
	auto g = m_groups.find(_groupName);
	if (g == m_groups.end()) {
		assert(0); // Invalid Group name
		return nullptr;
	} else { return g->second; }
}

ak::aPropertyGridItem * ak::aPropertyGridWidget::item(ID _id) {
	auto itm = m_items.find(_id);
	if (itm == m_items.end()) {
		assert(0);	// Invalid item ID
		return nullptr;
	}
	else { return itm->second; }
}

ak::aPropertyGridItem * ak::aPropertyGridWidget::findItem(const QString& _itemName)
{
	for (auto item : m_items)
	{
		if (item.second->name() == _itemName)
		{
			return item.second;
		}
	}

	return nullptr;
}

// ##############################################################################################

// Slots

void ak::aPropertyGridWidget::slotItemChanged(void) {
	aPropertyGridItem * itm = dynamic_cast<aPropertyGridItem *>(sender());
	if (itm) {
		emit itemChanged(itm->id());
	}
	else {
		assert(0);
	}
}

void ak::aPropertyGridWidget::slotFocusOut(void) {
	for (auto g : m_groups) {
		g.second->clearFocus();
	}
}

void ak::aPropertyGridWidget::slotItemDeleted(void) {
	aPropertyGridItem* itm = dynamic_cast<aPropertyGridItem*>(sender());
	if (itm) {
		emit itemDeleted(itm->id());
	}
	else {
		assert(0);
	}
}

// ##############################################################################################

// Private functions

ak::ID ak::aPropertyGridWidget::newItemCreated(aPropertyGridGroup * _group, aPropertyGridItem * _item) {
	_item->setId(++m_currentID);
	m_items.insert_or_assign(_item->id(), _item);
	_group->addItem(_item);
	_item->setEnabled(m_isEnabled);

	updateTableVisibility();

	connect(_item, &aPropertyGridItem::changed, this, &aPropertyGridWidget::slotItemChanged);
	connect(_item, &aPropertyGridItem::deleted, this, &aPropertyGridWidget::slotItemDeleted);

	return _item->id();
}

void ak::aPropertyGridWidget::eraseItemFromMap(aPropertyGridItem * _item) {
	m_items.erase(_item->id());
}

void ak::aPropertyGridWidget::updateTableVisibility(void) {
	if (m_items.size() == 0 && m_table->isVisible()) {
		m_centralLayout->removeWidget(m_table);
		m_centralLayout->addWidget(m_label);
		m_centralLayout->addWidget(m_stretch, 1);
		m_table->setVisible(false);

		m_centralLayoutW->setStyleSheet("#AK_aPropertyGrid_CentralLayoutW {border-width: 1px; border-style: solid; border-color: black; }");
	}
	else if (m_items.size() > 0 && !m_table->isVisible()) {
		m_centralLayoutW->setStyleSheet("#AK_aPropertyGrid_CentralLayoutW {border-width: 0px; }");

		m_centralLayout->removeWidget(m_label);
		m_centralLayout->removeWidget(m_stretch);
		m_centralLayout->addWidget(m_table);
		m_table->setVisible(true);

	}
}

// #############################################################################################################################################

// #############################################################################################################################################

// #############################################################################################################################################

ak::aPropertyGridGroup::aPropertyGridGroup(aPropertyGridWidget * _owner, const QString& _title, const aColor& _color, const aColor& _foregroundColor, const aColor& _errorColor)
	: m_owner(_owner), m_color(_color), m_isAlternate(true), m_headerColorB(160, 160, 160), m_headerColorF(0, 0, 0), m_title(_title),
	m_isCollapsed(false), m_colorForeground(_foregroundColor), m_colorForegroundError(_errorColor)
{
	assert(m_owner);
	m_headerItem = new QTableWidgetItem(_title);
	m_headerItem->setBackground(m_headerColorB.toQColor());
	m_headerItem->setForeground(m_headerColorF.toQColor());
	m_headerItem->setFlags(m_headerItem->flags() ^ Qt::ItemIsEditable ^ Qt::ItemIsSelectable);
	
	int r = m_owner->m_table->rowCount();
	m_owner->m_table->insertRow(r);
	m_owner->m_table->setItem(r, 0, m_headerItem);
	m_owner->m_table->setSpan(r, 0, 1, 2);
	
	// Hide
	m_owner->m_table->setRowHidden(r, true);

	// Calculate colors
	setGroupColors(_color, _foregroundColor, _errorColor);

	connect(m_owner->m_table, &aTableWidget::itemDoubleClicked, this, &aPropertyGridGroup::slotItemDoubleClicked);
}

ak::aPropertyGridGroup::~aPropertyGridGroup() {
	delete m_headerItem;
}

// ##############################################################################################

// Getter

int ak::aPropertyGridGroup::lastItemRow(void) const { return m_headerItem->row() + m_items.size(); }

int ak::aPropertyGridGroup::headerRow(void) const { return m_headerItem->row(); }

// ##############################################################################################

// Manipulators

void ak::aPropertyGridGroup::repaint(void) {
	m_headerItem->setBackground(m_headerColorB.toQColor());
	m_headerItem->setForeground(m_headerColorF.toQColor());

	for (auto itm : m_items) { itm->repaint(); }
}

void ak::aPropertyGridGroup::addItem(aPropertyGridItem * _item) {
	// Display the header if its the first item that is added
	if (m_items.size() == 0) {
		m_owner->m_table->setRowHidden(m_headerItem->row(), false);
	}

	// Store and setup the item
	m_items.push_back(_item);
	_item->setGroup(this);
	m_isAlternate = !m_isAlternate;
	if (m_isAlternate) { _item->setColor(m_colorForeground, m_colorAlternate, m_colorForegroundError); }
	else { _item->setColor(m_colorForeground, m_color, m_colorForegroundError); }
	if (m_isCollapsed) {
		m_owner->m_table->setRowHidden(_item->row(), true);
	}
}

void ak::aPropertyGridGroup::setGroupColors(const aColor& _color, const aColor& _foregroundColor, const aColor& _errorColor, bool _repaintItems) {
	m_color = _color;
	m_colorForeground = _foregroundColor;
	m_colorForegroundError = _errorColor;
	m_colorAlternate = _color;
	m_colorAlternate.changeBy(50);

	if (_repaintItems) {
		bool alt = true;
		for (auto itm : m_items) {
			alt = !alt;
			if (alt) {
				itm->setColor(m_colorForeground, m_colorAlternate, m_colorForegroundError); 
			}
			else {
				itm->setColor(m_colorForeground, m_color, m_colorForegroundError); 
			}
		}
	}
}

void ak::aPropertyGridGroup::refreshStateIcons(void) {
	if (m_owner->m_groupIconsSet) {
		if (m_isCollapsed) { m_headerItem->setIcon(m_owner->m_iconGroupExpand); }
		else { m_headerItem->setIcon(m_owner->m_iconGroupCollapse); }
 	}
}

void ak::aPropertyGridGroup::clear(void) {
	if (m_items.size() > 0) {
		m_owner->m_table->setRowHidden(m_headerItem->row(), true);
	}
	if (m_isCollapsed) {
		slotItemDoubleClicked(m_headerItem);
	}
	for (auto itm : m_items) {
		int r = itm->row();
		m_owner->eraseItemFromMap(itm);
		delete itm;
		m_owner->m_table->removeRow(r);
	}
	m_items.clear();
}

void ak::aPropertyGridGroup::clearFocus(void) {
	for (auto itm : m_items) { itm->clearFocus(); }
}

// ##############################################################################################

// 

void ak::aPropertyGridGroup::slotItemDoubleClicked(QTableWidgetItem * _item) {
	if (_item == m_headerItem) {
		m_isCollapsed = !m_isCollapsed;
		for (auto itm : m_items) {
			m_owner->m_table->setRowHidden(itm->row(), m_isCollapsed);
		}
		refreshStateIcons();
	}
}

// #############################################################################################################################################

// #############################################################################################################################################

// #############################################################################################################################################

ak::aPropertyGridItem::aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, bool _value, bool _isMultipleValues)
	: m_name(_name), m_table(_table), m_row(_row), m_valueType(vtBool), m_vBool(_value), m_isMultipleValues(_isMultipleValues), m_numberInputMode(TextForm), m_vIntMin(INT_MIN), m_vIntMax(INT_MAX)
{
	ini();

}

ak::aPropertyGridItem::aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, int _value, NumberInputMode _inputMode, int _min, int _max, bool _isMultipleValues)
	: m_name(_name), m_table(_table), m_row(_row), m_valueType(vtInt), m_vInt(_value), m_isMultipleValues(_isMultipleValues), m_numberInputMode(_inputMode), m_vIntMin(_min), m_vIntMax(_max)
{
	ini();

}

ak::aPropertyGridItem::aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, double _value, bool _isMultipleValues)
	: m_name(_name), m_table(_table), m_row(_row), m_valueType(vtDouble), m_vDouble(_value), m_isMultipleValues(_isMultipleValues), m_numberInputMode(TextForm), m_vIntMin(INT_MIN), m_vIntMax(INT_MAX)
{
	ini();

}

ak::aPropertyGridItem::aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, const QString& _value, bool _isMultipleValues)
	: m_name(_name), m_table(_table), m_row(_row), m_valueType(vtString), m_vString(_value), m_isMultipleValues(_isMultipleValues), m_numberInputMode(TextForm), m_vIntMin(INT_MIN), m_vIntMax(INT_MAX)
{
	ini();
}

ak::aPropertyGridItem::aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, const aTime& _value, timeFormat _timeFormat, bool _isMultipleValues)
	: m_name(_name), m_table(_table), m_row(_row), m_valueType(vtTime), m_vTime(_value), m_isMultipleValues(_isMultipleValues), m_vTimeFormat(_timeFormat), m_numberInputMode(TextForm), m_vIntMin(INT_MIN), m_vIntMax(INT_MAX)
{
	ini();

}

ak::aPropertyGridItem::aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, const aDate& _value, dateFormat _dateFormat, bool _isMultipleValues)
	: m_name(_name), m_table(_table), m_row(_row), m_valueType(vtColor), m_vDate(_value), m_isMultipleValues(_isMultipleValues), m_vDateFormat(_dateFormat), m_numberInputMode(TextForm), m_vIntMin(INT_MIN), m_vIntMax(INT_MAX)
{
	ini();

}

ak::aPropertyGridItem::aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, const aColor& _value, bool _isMultipleValues)
	: m_name(_name), m_table(_table), m_row(_row), m_valueType(vtColor), m_vColor(_value), m_isMultipleValues(_isMultipleValues), m_numberInputMode(TextForm), m_vIntMin(INT_MIN), m_vIntMax(INT_MAX)
{
	ini();

}

ak::aPropertyGridItem::aPropertyGridItem(aTableWidget * _table, int _row, const QString& _name, const QStringList& _possibleSelection, const QString& _value, bool _isMultipleValues)
	: m_name(_name), m_table(_table), m_row(_row), m_valueType(vtSelection), m_vSelection(_value), m_vSelectionPossible(_possibleSelection), m_isMultipleValues(_isMultipleValues), m_numberInputMode(TextForm), m_vIntMin(INT_MIN), m_vIntMax(INT_MAX)
{
	ini();
}

ak::aPropertyGridItem::~aPropertyGridItem() {
	switch (m_valueType)
	{
	case ak::vtBool: delete m_cBool; break;
	case ak::vtColor: delete m_cColor; break;
	case ak::vtDate: delete m_cDate; break;
	case ak::vtSelection: delete m_cSelection; break;
		break;
	case ak::vtTime: delete m_cTime; break;
		break;
	case ak::vtDouble:
	case ak::vtInt:
	case ak::vtString:
		delete m_textItem;
		break;
	default:
		assert(0);
		break;
	}
	delete m_nameItem;
}

// ##############################################################################################

// Getter

int ak::aPropertyGridItem::row(void) const { return m_nameItem->row(); }

// ##############################################################################################

// Manipulators

void ak::aPropertyGridItem::repaint(void) {
	setErrorState(m_isError, true);
	m_nameItem->setBackground(m_colorBackground.toQColor());
	m_nameItem->setForeground(m_colorNormal.toQColor());
}

void ak::aPropertyGridItem::setEnabled(bool _isEnabled) {
	m_isEnabled = _isEnabled;
	updateEditability();
}

void ak::aPropertyGridItem::setReadOnly(bool _isReadOnly) {
	m_isReadOnly = _isReadOnly;
	updateEditability();
}

void ak::aPropertyGridItem::setDeletable(bool _isDeletable) {
	
	if (m_isDeletable != _isDeletable)
	{
		if (_isDeletable)
		{
			QPushButton* button = new QPushButton();
			button->setIcon(m_group->getOwner()->getDeleteIcon());
			button->setFlat(true);
			button->setStyleSheet("QPushButton { background-color: transparent }");

			QHBoxLayout* l = new QHBoxLayout();
			l->addStretch(0);
			l->addWidget(button);
			l->setContentsMargins(0, 0, 0, 0);

			QWidget* w = new QWidget();
			w->setLayout(l);

			m_table->setCellWidget(m_row, tcName, w);

			connect(button, &QPushButton::pressed, this, &aPropertyGridItem::slotItemDeleted);
		}
		else
		{
			m_table->removeCellWidget(m_row, tcName);
		}
	}

	m_isDeletable = _isDeletable;
}

void ak::aPropertyGridItem::setColor(const aColor& _normalForeground, const aColor& _normalBackground, const aColor& _errorForeground) {
	m_colorBackground = _normalBackground;
	m_colorNormal = _normalForeground;
	m_colorError = _errorForeground;
	m_ignoreEvent = true;
	m_nameItem->setBackground(m_colorBackground.toQColor());
	m_nameItem->setForeground(m_colorNormal.toQColor());
	m_ignoreEvent = false;
	setErrorState(m_isError, true);
}

void ak::aPropertyGridItem::clearFocus(void) {
	if (m_ignoreEvent) { return; }
	if (m_textItem) {
		m_textItem->setBackground(m_colorBackground.toQColor());
	}
	return;
	switch (m_valueType)
	{
	case ak::vtBool: 
	{
		Qt::FocusPolicy policy = m_cBool->focusPolicy();
		m_cBool->setFocusPolicy(Qt::FocusPolicy::NoFocus);
		m_cBool->setFocusPolicy(policy);
		m_table->setCurrentItem(nullptr);
	}
		break;
	case ak::vtInt:
		if (m_numberInputMode == NumericUpDown) {
			Qt::FocusPolicy policy = m_cInt->focusPolicy();
			m_cInt->setFocusPolicy(Qt::FocusPolicy::NoFocus);
			m_cInt->setFocusPolicy(policy);
			break;
		}
	case ak::vtColor:
	case ak::vtDate:
	case ak::vtSelection:
	case ak::vtTime:
	case ak::vtDouble:
	case ak::vtString: m_table->setCurrentItem(nullptr); break;
	default:
		assert(0);
		return;
	}
}

// ##############################################################################################

// Value setter/getter

void ak::aPropertyGridItem::setBool(bool _value) {
	if (m_valueType == vtBool) {
		m_vBool = _value;
		m_ignoreEvent = true;
		if (_value) { m_cBool->setCheckState(Qt::Checked); }
		else { m_cBool->setCheckState(Qt::Unchecked); }
		m_ignoreEvent = false;
	}
	else { assert(0); }
}

void ak::aPropertyGridItem::setInt(int _value) {
	if (m_valueType == vtInt) {
		m_vInt = _value;
		m_ignoreEvent = true;
		m_textItem->setText(QString::number(_value));
		m_ignoreEvent = false;
	}
	else { assert(0); }
}

void ak::aPropertyGridItem::setDouble(double _value) {
	if (m_valueType == vtDouble) {
		m_vDouble = _value;
		m_ignoreEvent = true;
		m_textItem->setText(QString::number(_value));
		m_ignoreEvent = false;
	}
	else { assert(0); }
}

void ak::aPropertyGridItem::setString(const QString& _value) {
	if (m_valueType == vtString) {
		m_vString = _value;
		m_ignoreEvent = true;
		m_textItem->setText(_value);
		m_ignoreEvent = false;
	}
	else  if (m_valueType == vtSelection) {
		m_vSelection = _value;
		m_ignoreEvent = true;
		m_cSelection->setText(_value);
		m_ignoreEvent = false;
	}
	else { assert(0); }
}

void ak::aPropertyGridItem::setColor(const aColor& _value) {
	if (m_valueType == vtColor) {
		m_vColor = _value;
		m_ignoreEvent = true;
		m_cColor->setColor(_value);
		m_ignoreEvent = false;
	}
	else { assert(0); }
}

void ak::aPropertyGridItem::setTime(const aTime& _value) {
	if (m_valueType == vtTime) {
		m_vTime = _value;
		m_ignoreEvent = true;
		m_cTime->setCurrentTime(_value);
		m_ignoreEvent = false;
	}
	else { assert(0); }
}

void ak::aPropertyGridItem::setDate(const aDate& _value) {
	if (m_valueType == vtDate) {
		m_vDate = _value;
		m_ignoreEvent = true;
		m_cDate->setCurrentDate(_value);
		m_ignoreEvent = false;
	}
	else { assert(0); }
}

// ##############################################################################################

// Error

void ak::aPropertyGridItem::setErrorState(bool _isError, bool _forceRepaint) {
	if (_isError == m_isError && !_forceRepaint) { return; }
	m_isError = _isError;
	m_ignoreEvent = true;
	
	QString sheet;
	sheet = "background-color: #" + m_colorBackground.toHexString();
	if (m_isError) {
		sheet.append("; foreground-color: #" + m_colorError.toHexString());
	}
	else {
		sheet.append("; foreground-color: #" + m_colorNormal.toHexString());
	}
	sheet.append(";");

	if (m_textItem) {  
		m_textItem->setBackground(m_colorBackground.toQColor());
		m_textItem->setForeground(m_isError ? m_colorError.toQColor() : m_colorNormal.toQColor());
	}
	if (m_cBool) {
		m_cBool->setStyleSheet(sheet);
	}
	if (m_cColor) {
		m_cColor->widget()->setStyleSheet("#AK_ColorEditButton_Central{" + sheet + "}");
		m_cColor->pushButton()->setStyleSheet(sheet);
	}
	if (m_cSelection) {
		m_cSelection->setStyleSheet(sheet);
		m_cSelection->getMenu()->setStyleSheet("QMenu {" + sheet + "}; QMenu::item {" + sheet + "};");
	}
	if (m_cTime) {
		m_cTime->setStyleSheet(sheet);
	}
	if (m_cDate) {
		m_cDate->setStyleSheet(sheet);
	}
	if (m_cInt) {
		m_cInt->setStyleSheet(sheet);
	}
	if (m_cDelete) {
		m_cDelete->setStyleSheet(sheet);
	}

	m_ignoreEvent = false;
}

void ak::aPropertyGridItem::setError(bool _value) {
	setBool(_value);
	setErrorState(true);
}

void ak::aPropertyGridItem::setError(int _value) {
	setInt(_value);
	setErrorState(true);
}

void ak::aPropertyGridItem::setError(double _value) {
	setDouble(_value);
	setErrorState(true);
}

void ak::aPropertyGridItem::setError(const QString& _value) {
	setString(_value);
	setErrorState(true);
}

void ak::aPropertyGridItem::setError(const aColor& _value) {
	setColor(_value);
	setErrorState(true);
}

void ak::aPropertyGridItem::setError(const aTime& _value) {
	setTime(_value);
	setErrorState(true);
}

void ak::aPropertyGridItem::setError(const aDate& _value) {
	setDate(_value);
	setErrorState(true);
}

// ##############################################################################################

// Slots

void ak::aPropertyGridItem::slotValueChanged(void) {
	if (m_ignoreEvent) { return; }
	switch (m_valueType)
	{
	case ak::vtColor:
		if (m_vColor == m_cColor->color()) { return; }
		m_vColor = m_cColor->color();
		break;
	case ak::vtDate:
		if (m_vDate == m_cDate->currentDate()) { return; }
		m_vDate = m_cDate->currentDate();
		break;
	case ak::vtSelection:
		if (m_vSelection == m_cSelection->text()) { return; }
		m_vSelection = m_cSelection->text();
		break;
	case ak::vtTime:
		if (m_vTime == m_cTime->currentTime()) { return; }
		m_vTime = m_cTime->currentTime();
		break;
	case ak::vtInt:
		if (m_vInt == m_cInt->value()) return;
		m_vInt = m_cInt->value();
		break;
	case ak::vtBool:
	case ak::vtDouble:
	case ak::vtString:
	default:
		assert(0);
		return;
	}
	m_isMultipleValues = false;
	emit changed();
}

void ak::aPropertyGridItem::slotValueBoolChanged(int _v) {
	if (m_ignoreEvent) { return; }
	switch (m_valueType)
	{
	case ak::vtBool:
		if (m_vBool == m_cBool->isChecked()) { return; }
		m_vBool = m_cBool->isChecked();
		break;
	case ak::vtInt:
		if (m_vInt == m_cInt->value()) return;
		m_vInt = m_cInt->value();
		break;
	case ak::vtColor:
	case ak::vtDate:
	case ak::vtSelection:
	case ak::vtTime:
	case ak::vtDouble:
	case ak::vtString:
	default:
		assert(0);
		return;
	}
	m_isMultipleValues = false;
	emit changed();
}

void ak::aPropertyGridItem::slotItemChanged(QTableWidgetItem * _item) {
	if (m_ignoreEvent || _item != m_textItem) { return; }
	switch (m_valueType)
	{
	case ak::vtDouble:
	{
		bool fail;
		double v = toNumber<double>(m_textItem->text().toStdString(), fail);
		if (fail) { setErrorState(true); return; }
		if (m_vDouble == v) { return; }
		m_vDouble = v;
	}
		break;
	case ak::vtInt:
	{
		bool fail;
		int v = toNumber<int>(m_textItem->text().toStdString(), fail);
		if (fail) { setErrorState(true); return; }
		if (m_vInt == v) { return; }
		m_vInt = v;
	}
		break;
	case ak::vtString: 
		if (m_vString == m_textItem->text()) { return; }
		m_vString = m_textItem->text();
		break;
	case ak::vtBool:
	case ak::vtColor:
	case ak::vtDate:
	case ak::vtSelection:
	case ak::vtTime:
	default:
		assert(0);
		return;
	}
	setErrorState(false);
	emit changed();
}

void ak::aPropertyGridItem::slotItemDeleted(void) {
	emit deleted();
}


// ##############################################################################################

// Private functions

void ak::aPropertyGridItem::ini(void) {
	// Setup default values
	m_id = invalidID;
	m_isReadOnly = false;
	m_isDeletable = false;
	m_isEnabled = true;
	m_isError = false;
	m_ignoreEvent = false;
	m_isEditable = true;
	m_group = nullptr;

	// Setup default colors
	m_colorBackground.setRGBA(255, 255, 255);
	m_colorError.setRGBA(255, 0, 0);
	m_colorNormal.setRGBA(0, 0, 0);

	// Create new row
	m_table->insertRow(m_row);

	// Initialize control pointer
	m_textItem = nullptr;
	m_cBool = nullptr;
	m_cColor = nullptr;
	m_cSelection = nullptr;
	m_cTime = nullptr;
	m_cDate = nullptr;
	m_cInt = nullptr;
	m_cDelete = nullptr;

	// Create items
	m_nameItem = new QTableWidgetItem(m_name);
	m_nameItem->setFlags(m_nameItem->flags() ^ Qt::ItemIsSelectable ^ Qt::ItemIsEditable);
	m_nameItem->setForeground(m_colorNormal.toQColor());
	m_nameItem->setBackground(m_colorBackground.toQColor());
	m_table->setItem(m_row, tcName, m_nameItem);
	switch (m_valueType)
	{
	case ak::vtBool:
		// Create and setup widget
		m_cBool = new aCheckBoxWidget;
		if (m_isMultipleValues) { m_cBool->setCheckState(Qt::PartiallyChecked); }
		else { m_cBool->setChecked(m_vBool); }
		m_cBool->setPaintBackground(true);
		m_table->setCellWidget(m_row, tcValue, m_cBool);
		// Connect callback
		connect(m_cBool, &aCheckBoxWidget::stateChanged, this, &aPropertyGridItem::slotValueBoolChanged);
		break;
	case ak::vtColor:
		// Create and setup widget
		m_cColor = new aColorEditButtonWidget(m_vColor);
		if (m_isMultipleValues) { m_cColor->overrideText("..."); }
		m_table->setCellWidget(m_row, tcValue, m_cColor->widget());
		// Connect callback
		connect(m_cColor, &aColorEditButtonWidget::changed, this, &aPropertyGridItem::slotValueChanged);
		break;
	case ak::vtDate:
		// Create and setup widget
		m_cDate = new aDatePickWidget(m_vDate, m_vDateFormat);
		if (m_isMultipleValues) { m_cDate->setText("..."); }
		m_table->setCellWidget(m_row, tcValue, m_cDate);
		// Connect callback
		connect(m_cDate, &aDatePickWidget::changed, this, &aPropertyGridItem::slotValueChanged);
		break;
	case ak::vtDouble:
		// Create and setup item
		if (m_isMultipleValues) { m_textItem = new QTableWidgetItem("..."); }
		else { m_textItem = new QTableWidgetItem(QString::number(m_vDouble)); }
		m_textItem->setForeground(m_colorNormal.toQColor());
		m_textItem->setBackground(m_colorBackground.toQColor());
		m_table->setItem(m_row, tcValue, m_textItem);
		// Connect callback
		connect(m_table, &aTableWidget::itemChanged, this, &aPropertyGridItem::slotItemChanged);
		break;
	case ak::vtInt:
		// Create and setup item
		if (m_numberInputMode == TextForm) {
			if (m_isMultipleValues) { m_textItem = new QTableWidgetItem("..."); }
			else { m_textItem = new QTableWidgetItem(QString::number(m_vInt)); }
			m_textItem->setForeground(m_colorNormal.toQColor());
			m_textItem->setBackground(m_colorBackground.toQColor());
			m_table->setItem(m_row, tcValue, m_textItem);
			// Connect callback
			connect(m_table, &aTableWidget::itemChanged, this, &aPropertyGridItem::slotItemChanged);
		}
		else if (m_numberInputMode == NumericUpDown) {
			m_cInt = new aSpinBoxWidget;
			m_cInt->setMinimum(m_vIntMin);
			m_cInt->setMaximum(m_vIntMax);
			m_cInt->setValue(m_vInt);
			if (m_isMultipleValues) {
				m_cInt->setSpecialValueText("...");
			}
			m_table->setCellWidget(m_row, tcValue, m_cInt);
			// Connect callback
			connect(m_cInt, &aSpinBoxWidget::editingFinished, this, &aPropertyGridItem::slotValueChanged);
			connect(m_cInt, &aSpinBoxWidget::releaseMouse, this, &aPropertyGridItem::slotValueChanged);
		}
		break;
	case ak::vtSelection:
		// Create and setup widget
		m_cSelection = new aComboButtonWidget(m_vSelection);
		m_cSelection->setItems(m_vSelectionPossible);
		if (m_isMultipleValues) { m_cSelection->setText("..."); }
		m_table->setCellWidget(m_row, tcValue, m_cSelection);
		// Connect callback
		connect(m_cSelection, &aComboButtonWidget::changed, this, &aPropertyGridItem::slotValueChanged);
		break;
	case ak::vtString:
		// Create and setup item
		if (m_isMultipleValues) { m_textItem = new QTableWidgetItem("..."); }
		else { m_textItem = new QTableWidgetItem(m_vString); }
		m_textItem->setForeground(m_colorNormal.toQColor());
		m_textItem->setBackground(m_colorBackground.toQColor());
		m_table->setItem(m_row, tcValue, m_textItem);
		// Connect callback
		connect(m_table, &aTableWidget::itemChanged, this, &aPropertyGridItem::slotItemChanged);
		break;
	case ak::vtTime:
		// Create and setup widget
		m_cTime = new aTimePickWidget(m_vTime, m_vTimeFormat);
		if (m_isMultipleValues) { m_cTime->setText("..."); }
		m_table->setCellWidget(m_row, tcValue, m_cTime);
		// Connect callback
		connect(m_cTime, &aTimePickWidget::changed, this, &aPropertyGridItem::slotValueChanged);
		break;
	default:
		assert(0); // Not implemented type
		break;
	}
}

void ak::aPropertyGridItem::updateEditability(void) {
	if (m_isEnabled && !m_isReadOnly) {
		if (m_isEditable) { return; }
		m_isEditable = true;
	}
	else {
		if (!m_isEditable) { return; }
		m_isEditable = false;
	}

	m_ignoreEvent = true;

	switch (m_valueType)
	{
	case ak::vtBool:
		m_cBool->setEnabled(m_isEditable);
		break;
	case ak::vtColor:
		m_cColor->SetEnabled(m_isEditable);
		break;
	case ak::vtDate:
		m_cDate->setEnabled(m_isEditable);
		break;
	case ak::vtSelection:
		m_cSelection->setEnabled(m_isEditable);
		break;
	case ak::vtTime:
		m_cTime->setEnabled(m_isEditable);
		break;
	case ak::vtInt:
		if (m_numberInputMode == NumericUpDown) {
			m_cInt->setEnabled(m_isEditable);
			m_ignoreEvent = false;
			return;
		}
	case ak::vtDouble:
	case ak::vtString:
		if (m_isEditable) { m_textItem->setFlags(m_textItem->flags() | Qt::ItemIsEditable); }
		else { m_textItem->setFlags(m_textItem->flags() ^ Qt::ItemIsEditable); }
		break;
	default:
		assert(0);
		break;
	}

	m_ignoreEvent = false;
}


