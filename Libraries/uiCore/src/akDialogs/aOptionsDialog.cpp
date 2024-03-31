/*
 *	File:		aOptionsDialog.cpp
 *	Package:	akDialogs
 *
 *  Created on: August 26, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// uiCore header
#include <akCore/aAssert.h>
#include <akDialogs/aOptionsDialog.h>
#include <akWidgets/aLabelWidget.h>
#include <akWidgets/aPushButtonWidget.h>
#include <akWidgets/aTreeWidget.h>
#include <akWidgets/aListWidget.h>
#include <akWidgets/aComboBoxWidget.h>
#include <akWidgets/aComboButtonWidget.h>
#include <akWidgets/aTableWidget.h>
#include <akWidgets/aLineEditWidget.h>
#include <akWidgets/aSpinBoxWidget.h>
#include <akWidgets/aDoubleSpinBoxWidget.h>
#include <akWidgets/aCheckBoxWidget.h>
#include <akWidgets/aPushButtonWidget.h>
#include <akWidgets/aColorEditButtonWidget.h>

// Qt header
#include <qlayout.h>
#include <qsplitter.h>
#include <qscrollarea.h>
#include <qfiledialog.h>

ak::aOptionsDialog::aOptionsDialog(optionsDialogStyle _style, QWidget * _parentWidget)
	: aDialog(otOptionsDialog, _parentWidget), m_activeGroup(nullptr), m_style(_style), 
	m_buttonApply(nullptr), m_buttonCancel(nullptr), m_buttonSave(nullptr), m_ignoreEvents(0), 
	m_itemChangedIsQueued(false), m_requestedGroup(nullptr), m_isClosing(false)
{
	// Create layouts
	m_centralLayout = new QVBoxLayout(this);
	
	m_buttonLayoutW = new QWidget;
	m_buttonLayout = new QHBoxLayout(m_buttonLayoutW);

	m_settingsLayoutW = new QWidget;
	m_settingsLayoutW->setObjectName("AK_OptionsDialog_SettingsLayoutW");
	m_settingsLayout = new QVBoxLayout(m_settingsLayoutW);
	m_settingsLayout->setContentsMargins(0, 0, 0, 0);

	m_settingsAreaLayoutW = new QWidget;
	m_settingsAreaLayoutW->setObjectName("AK_OptionsDialog_SettingsAreaLayoutW");
	m_settingsAreaLayout = new QVBoxLayout(m_settingsAreaLayoutW);

	// Create controls
	m_splitter = new QSplitter(Qt::Orientation::Horizontal);

	m_navigation = new aTreeWidget;
	m_navigation->setFilterVisible(true);
	m_navigation->setFilterCaseSensitive(false);
	m_navigation->setSortingEnabled(true);
	m_navigation->setMultiSelectionEnabled(false);

	m_settingsArea = new QScrollArea;
	m_settingsArea->setObjectName("AK_OptionsDialog_SettingsArea");

	m_buttonLayout->addStretch(1);
	switch (m_style)
	{
	case ak::aOptionsDialog::optionsApplySaveCancel:
		m_buttonApply = new aPushButtonWidget("Apply");
		m_buttonCancel = new aPushButtonWidget("Cancel");
		m_buttonSave = new aPushButtonWidget("Save");
		m_buttonLayout->addWidget(m_buttonSave);
		m_buttonLayout->addWidget(m_buttonCancel);
		m_buttonLayout->addWidget(m_buttonApply);

		m_buttonApply->setEnabled(false);

		connect(m_buttonApply, &aPushButtonWidget::clicked, this, &aOptionsDialog::slotApply);
		connect(m_buttonCancel, &aPushButtonWidget::clicked, this, &aOptionsDialog::slotCancel);
		connect(m_buttonSave, &aPushButtonWidget::clicked, this, &aOptionsDialog::slotSave);
		break;
	case ak::aOptionsDialog::optionsClose:
		m_buttonCancel = new aPushButtonWidget("Close");
		m_buttonLayout->addWidget(m_buttonCancel);
		connect(m_buttonCancel, &aPushButtonWidget::clicked, this, &aOptionsDialog::slotCancel);
		break;
	default:
		break;
	}

	// Setup layots
	m_centralLayout->addWidget(m_splitter, 1);
	m_centralLayout->addWidget(m_buttonLayoutW);
	
	m_settingsLayout->addWidget(m_settingsAreaLayoutW);
	m_settingsLayout->addStretch(1);

	m_splitter->addWidget(m_navigation->widget());
	m_splitter->addWidget(m_settingsArea);
	m_splitter->setStretchFactor(0, 0);
	m_splitter->setStretchFactor(1, 1);

	m_dummy = new QWidget;

	m_settingsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	m_settingsArea->setUpdatesEnabled(true);
	m_settingsArea->setWidgetResizable(true);
	m_settingsArea->setWidget(m_settingsLayoutW);
	m_settingsArea->setObjectName("AK_OptionsDialog_SettingsArea");
	m_settingsArea->setAutoFillBackground(true);

	// Setup dialog window
	hideInfoButton();
	setMinimumSize(600, 500);

	// Connect callbacks
	connect(m_navigation, &aTreeWidget::selectionChanged, this, &aOptionsDialog::slotNavigationChanged);
}

ak::aOptionsDialog::~aOptionsDialog() {
	for (auto g : m_groups) { delete g; }

	delete m_navigation;
	delete m_settingsAreaLayout;
	delete m_settingsAreaLayoutW;
	delete m_settingsArea;
	delete m_splitter;
	delete m_buttonApply;
	delete m_buttonCancel;
	delete m_buttonSave;
	delete m_buttonLayout;
	delete m_buttonLayoutW;
	delete m_centralLayout;
}

// #############################################################################################################

// Setter

ak::aOptionsGroup * ak::aOptionsDialog::addGroup(const QString& _name, const QString& _title) {
	for (auto g : m_groups) { if (g->title().toLower() == _title.toLower()) { return g; } }
	m_ignoreEvents++;
	aOptionsGroup * group = new aOptionsGroup(_name, _title);
	m_groups.push_back(group);
	group->attachToDialog(this);
	m_ignoreEvents--;
	return group;
}

void ak::aOptionsDialog::addGroup(aOptionsGroup * _group) {
	m_ignoreEvents++;
	m_groups.push_back(_group);
	_group->attachToDialog(this);
	m_ignoreEvents--;
}

void ak::aOptionsDialog::removeGroup(const QString& _name) {
	m_ignoreEvents++;
	for (size_t i{ 0 }; i < m_groups.size(); i++) {
		if (m_groups[i]->name() == _name) {
			aOptionsGroup * checkGroup{ m_groups[i] };
			while (checkGroup) {
				if (checkGroup == m_activeGroup) {
					m_activeGroup->hide(this);
					m_activeGroup = nullptr;
					break;
				}
				checkGroup = checkGroup->parentGroup();
			}
			slotRemoveGroup(m_groups[i]);
			m_ignoreEvents--;
			return;
		}
	}
	m_ignoreEvents--;
}

void ak::aOptionsDialog::removeGroup(aOptionsGroup * _group) {
	m_ignoreEvents++;
	slotRemoveGroup(_group);
	m_ignoreEvents--;
}

void ak::aOptionsDialog::addWidget(QWidget * _w) {
	m_settingsAreaLayout->addWidget(_w);
}

void ak::aOptionsDialog::removeWidget(QWidget * _w) {
	m_settingsAreaLayout->removeWidget(_w);
	_w->setParent(nullptr);
}

void ak::aOptionsDialog::setNavigationSearchPlaceholderText(const QString& _text) {
	m_navigation->filterWidget()->setPlaceholderText(_text);
}

void ak::aOptionsDialog::selectGroupByLogicalName(const QString& _logicalName, const QString& _delimiter) {
	m_ignoreEvents++;
	aOptionsGroup * group =	findGroupByLogicalName(_logicalName, _delimiter);
	if (group) {
		std::vector<ID> selection = m_navigation->selectedItems();
		for (ID id : selection) { m_navigation->setSingleItemSelected(id, false); }
		m_navigation->setSingleItemSelected(group->navigationID(), true);
	}
	m_ignoreEvents--;
	slotNavigationChanged();
}

// #############################################################################################################

// Base class functions

ak::dialogResult ak::aOptionsDialog::showDialog(void) {
	if (m_groups.size() > 0) return showDialog(m_groups[0], nullptr);
	else return showDialog(nullptr, nullptr);
}

ak::dialogResult ak::aOptionsDialog::showDialog(QWidget * _parentWidget) {
	if (m_groups.size() > 0) return showDialog(m_groups[0], _parentWidget);
	else return showDialog(nullptr, _parentWidget);
}

ak::dialogResult ak::aOptionsDialog::showDialog(aOptionsGroup * _groupToShow, QWidget * _parentWidget) {
	m_isClosing = false;
	if (_groupToShow) {
		m_requestedGroup = _groupToShow;
		QMetaObject::invokeMethod(this, &aOptionsDialog::slotShowGroup, Qt::QueuedConnection);
	}
	if (_parentWidget) return aDialog::showDialog(_parentWidget);
	else return aDialog::showDialog();	
}

void ak::aOptionsDialog::closeEvent(QCloseEvent * _event) {
	m_isClosing = true;
	aDialog::closeEvent(_event);
}

// #############################################################################################################

// Getter

void findChildOptionItems(std::list<ak::aAbstractOptionsItem *>& _list, ak::aOptionsGroup * _group) {
	for (auto itm : _group->items()) { _list.push_back(itm); }
	for (auto g : _group->subgroups()) { findChildOptionItems(_list, g); }
}

std::list<ak::aAbstractOptionsItem *> ak::aOptionsDialog::items(void) const {
	std::list<ak::aAbstractOptionsItem *> lst;
	for (auto g : m_groups) { findChildOptionItems(lst, g); }
	return lst;
}

ak::aOptionsGroup * ak::aOptionsDialog::findGroupByNavigationId(ID _id) {
	for (auto g : m_groups) {
		aOptionsGroup * group = g->findGroupByNavigationId(_id);
		if (group) { return group; }
	}
	return nullptr;
}

ak::aOptionsGroup * ak::aOptionsDialog::findGroupByLogicalName(const QString& _logicalName, const QString& _delimiter) {
	QStringList lst = _logicalName.split(_delimiter);
	if (lst.size() == 0) { aAssert(0, "No name provided"); return nullptr; }
	for (auto g : m_groups) {
		if (g->name() == lst[0]) {
			if (lst.size() == 1) { return g; }
			else { return g->findSubgroupByLogicalName(lst, 1); }
		}
	}
	aAssert(0, "Group not fround");
	return nullptr;
}

// #############################################################################################################

// Slots

void ak::aOptionsDialog::slotShowGroup() {
	if (m_requestedGroup == nullptr) { return; }
	m_ignoreEvents++;
	auto selection = m_navigation->selectedItems();
	for (auto itm : selection) {
		m_navigation->setSingleItemSelected(itm, false);
	}
	m_navigation->setSingleItemSelected(m_requestedGroup->navigationID(), true);
	m_requestedGroup = nullptr;
	m_ignoreEvents--;
	slotNavigationChanged();
}

void ak::aOptionsDialog::slotRemoveGroup(aOptionsGroup * _group) {
	if (_group == nullptr) { return; }
	auto it = std::find(m_groups.begin(), m_groups.end(), _group);
	auto selection = m_navigation->selectedItems();
	for (ID id : selection) {
		if (id == _group->navigationID()) { m_navigation->setSingleItemSelected(id, false); }
	}
	for (auto g : m_groups) {
		if (g == _group) {
			if (g->hasSubgroup(m_activeGroup)) {
				m_activeGroup->hide(this);
				m_activeGroup = nullptr;
			}
			delete g;
			break;
		}
	}
	if (it != m_groups.end()) {  m_groups.erase(it); }
}

void ak::aOptionsDialog::slotNavigationChanged() {
	if (m_ignoreEvents || m_isClosing) { return; }
	if (m_activeGroup) { m_activeGroup->hide(this); }
	m_activeGroup = nullptr;
	auto selection = m_navigation->selectedItems();
	if (selection.size() == 0) {
		return;
	} else if (selection.size() > 1) {
		assert(0);
		return;
	}

	m_activeGroup = findGroupByNavigationId(selection[0]);

	if (m_activeGroup) {
		m_lastGroupName = m_activeGroup->logicalName();
		m_activeGroup->show(this);
	}
	else {
		assert(0);
	}
}

void ak::aOptionsDialog::slotSave(void) {
	if (m_buttonApply->isEnabled()) { slotApply(); }
	Close(resultOk);
}

void ak::aOptionsDialog::slotCancel(void) {
	Close(resultCancel);
}

void ak::aOptionsDialog::slotApply(void) {
	m_buttonApply->setEnabled(false);
	Q_EMIT saveSettingsRequested();
}

void ak::aOptionsDialog::slotEmitChanged(aAbstractOptionsItem * _item) {
	m_itemChangedIsQueued = false;
	Q_EMIT itemChanged(_item);
}

// #############################################################################################################

// Private functions

void ak::aOptionsDialog::clearSettingsArea(void) {

}

void ak::aOptionsDialog::groupItemChanged(aAbstractOptionsItem * _item) {
	if (m_itemChangedIsQueued) { return; }
	m_itemChangedIsQueued = true;
	if (m_buttonApply) { m_buttonApply->setEnabled(true); }
	QMetaObject::invokeMethod(this, "slotEmitChanged", Qt::QueuedConnection, Q_ARG(aAbstractOptionsItem *, _item));
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aOptionsGroup::aOptionsGroup(const QString& _name, const QString& _title, aOptionsGroup * _parentGroup)
	: aObject(otNone), m_dialog(nullptr), m_title(_title), m_parentGroup(nullptr), m_isVisible(false),
	m_navigationID(invalidID), m_name(_name)
{
	m_titleLayoutW = new QWidget;
	m_titleLayoutW->setObjectName("AK_OptionsGroup_TitleLayoutW");
	m_titleLayout = new QHBoxLayout(m_titleLayoutW);
	m_titleLayout->setObjectName("AK_OptionsGroup_TitleLayout");
	m_titleLabel = new aLabelWidget(m_title);
	m_titleLabel->setObjectName("AK_OptionsGroup_Title");
	m_titleLayout->setContentsMargins(0, 0, 0, 0);

	QFont f = m_titleLabel->font();
	f.setPointSize(14);
	m_titleLabel->setFont(f);

	m_titleLayout->addWidget(m_titleLabel);
}

ak::aOptionsGroup::~aOptionsGroup() {
	hideNavigationItem();
	for (auto itm : m_items) { delete itm; }
	for (auto g : m_groups) { delete g; }
}

// #############################################################################################################

// View manipulation

void ak::aOptionsGroup::show(aOptionsDialog * _dialog) {
	if (m_isVisible) { return; }
	m_isVisible = true;

	_dialog->addWidget(m_titleLayoutW);

	for (auto g : m_groups) { g->show(_dialog); }
	for (auto i : m_items) { i->Show(_dialog); }
}

void ak::aOptionsGroup::hide(aOptionsDialog * _dialog) {
	if (!m_isVisible) { return; }
	m_isVisible = false;

	_dialog->removeWidget(m_titleLayoutW);

	for (auto g : m_groups) { g->hide(_dialog); }
	for (auto i : m_items) { i->Hide(_dialog); }
}

// #############################################################################################################

// Setter

ak::aOptionsGroup * ak::aOptionsGroup::addGroup(const QString& _name, const QString& _title) {
	for (auto g : m_groups) { if (g->title().toLower() == _title.toLower()) { return g; } }

	aOptionsGroup * group = new aOptionsGroup(_name, _title);
	m_groups.push_back(group);
	group->attachToGroup(this);
	return group;
}

void ak::aOptionsGroup::addGroup(aOptionsGroup * _group) {
	m_groups.push_back(_group);
	_group->attachToGroup(this);
}

void ak::aOptionsGroup::addItem(aAbstractOptionsItem * _item) {
	m_items.push_back(_item);
	_item->attachToGroup(this);
}

void ak::aOptionsGroup::attachToDialog(aOptionsDialog * _dialog) {
	if (m_dialog) { aAssert(0, "Group is already attached to a dialog");}
	if (m_parentGroup) {
		aAssert(0, "Group is already attached to a group");
		m_parentGroup = nullptr;
	}
	m_dialog = _dialog;
	displayNavigationItem();
}

void ak::aOptionsGroup::attachToGroup(aOptionsGroup * _group) {
	if (m_dialog) {
		aAssert(0, "Group is already attached to a dialog");
		m_dialog = nullptr;
	}
	if (m_parentGroup) { aAssert(0, "Group is already attached to a group"); }
	m_parentGroup = _group;

	if (m_parentGroup == nullptr) { return; }
	ID parentId = m_parentGroup->navigationID();
	if (parentId != invalidID) { displayNavigationItem(); }
}

void ak::aOptionsGroup::displayNavigationItem(void) {
	aOptionsDialog * dia = getDialog();
	if (dia) {
		if (m_navigationID == invalidID) {
			if (m_parentGroup) { m_navigationID = dia->navigation()->add(m_parentGroup->navigationID(), m_title); }
			else { m_navigationID = dia->navigation()->add(invalidID, m_title); }
		}
		for (auto itm : m_items) { itm->displayNavigationItem(); }
		for (auto sg : m_groups) { sg->displayNavigationItem(); }
	}
	else { aAssert(0, "No parent group is attached to a dialog"); }
}

void ak::aOptionsGroup::hideNavigationItem(void) {
	if (m_parentGroup) {
		if (m_parentGroup->navigationID() == invalidID) { m_navigationID = invalidID; }
	}
	if (m_navigationID == invalidID) { return; }
	aOptionsDialog * dia = getDialog();
	if (dia) {
		dia->navigation()->deleteItem(m_navigationID);
		m_navigationID = invalidID;
		for (auto itm : m_items) { itm->hideNavigationItem(); }
		for (auto g : m_groups) { g->hideNavigationItem(); }
	}
}

// #############################################################################################################

// Getter

ak::aOptionsGroup * ak::aOptionsGroup::findGroupByNavigationId(ID _id) {
	if (_id == m_navigationID) { return this; }
	for (auto g : m_groups) {
		aOptionsGroup * group = g->findGroupByNavigationId(_id);
		if (group) { return group; }
	}
	return nullptr;
}

bool ak::aOptionsGroup::hasSubgroup(aOptionsGroup * _group) {
	for (auto g : m_groups) {
		if (g == _group) { return true; }
		if (g->hasSubgroup(_group)) { return true; }
	}
	return false;
}

bool ak::aOptionsGroup::hasDialog(void) {
	if (m_dialog) { return true; }
	if (m_parentGroup) { return m_parentGroup->hasDialog(); }
	return false;
}

ak::aOptionsDialog * ak::aOptionsGroup::getDialog(void) {
	if (m_dialog) { return m_dialog; }
	if (m_parentGroup) { return m_parentGroup->getDialog(); }
	aAssert(0, "The group or any parent group is not attached to a dialog");
	return nullptr;
}

QString ak::aOptionsGroup::logicalName(const QString& _delimiter) {
	QString ret;
	if (m_parentGroup) { ret = m_parentGroup->logicalName(_delimiter).append(_delimiter); }
	ret.append(m_name);
	return ret;
}

ak::aOptionsGroup * ak::aOptionsGroup::findSubgroupByLogicalName(const QString& _logicalName, const QString& _delimiter) {
	QStringList lst = _logicalName.split(_delimiter);
	if (lst.size() == 0) { aAssert(0, "No name provided"); return nullptr; }
	for (auto g : m_groups) {
		if (g->name() == lst[0]) {
			if (lst.size() == 1) { return g; }
			else { return g->findSubgroupByLogicalName(lst, 1); }
		}
	}
	aAssert(0, "Subgroup not fround");
	return nullptr;
}

ak::aOptionsGroup * ak::aOptionsGroup::findSubgroupByLogicalName(const QStringList& _logicalName, int _index) {
	for (auto g : m_groups) {
		if (g->name() == _logicalName[_index]) {
			if (_logicalName.size() == ++_index) { return g; }
			else { return g->findSubgroupByLogicalName(_logicalName, _index); }
		}
	}
	aAssert(0, "Subgroup not fround");
	return nullptr;
}

ak::aAbstractOptionsItem * ak::aOptionsGroup::findItemByLogicalName(const QString& _logicalName, const QString& _delimiter) {
	QStringList lst = _logicalName.split(_delimiter);
	if (lst.size() == 0) { aAssert(0, "No name provided"); return nullptr; }
	else if (lst.size() == 1) {
		for (auto itm : m_items) {
			if (itm->name() == lst[0]) { return itm; }
		}
		aAssert(0, "Item not fround");
		return nullptr;
	}
	else {
		for (auto g : m_groups) {
			if (g->name() == lst[0]) { return g->findItemByLogicalName(lst, 1); }
		}
		aAssert(0, "Subgroup not fround");
		return nullptr;
	}
}

ak::aAbstractOptionsItem * ak::aOptionsGroup::findItemByLogicalName(const QStringList& _logicalName, int _index) {
	if (_logicalName.size() == _index + 1) {
		for (auto itm : m_items) {
			if (itm->name() == _logicalName[_index]) { return itm; }
		}
		aAssert(0, "Item not fround");
		return nullptr;
	}
	else {
		for (auto g : m_groups) {
			if (g->name() == _logicalName[_index]) { return g->findItemByLogicalName(_logicalName, ++_index); }
		}
		aAssert(0, "Subgroup not fround");
		return nullptr;
	}
}

void ak::aOptionsGroup::itemChanged(aAbstractOptionsItem * _item) {
	if (m_dialog) { m_dialog->groupItemChanged(_item); return; }
	if (m_parentGroup) { m_parentGroup->itemChanged(_item); return; }
	aAssert(0, "No dialog was attached to any of the parent groups");
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aAbstractOptionsItem::aAbstractOptionsItem(optionItemType _type, const QString& _name, const QString& _title)
	: aWidget(otNone), m_name(_name), m_title(_title), m_isVisible(false), m_widgetIsConstructed(false), m_parentGroup(nullptr),
	m_navigationId(invalidID), m_type(_type)
{}

ak::aAbstractOptionsItem::~aAbstractOptionsItem() {
	hideNavigationItem();
}

// #############################################################################################################

// View manipulation

void ak::aAbstractOptionsItem::constructWidget(void) {
	if (m_widgetIsConstructed) {
		aAssert(0, "Widget was already constructed"); return;
	}
	ConstructWidget();
	m_widgetIsConstructed = true;
}

void ak::aAbstractOptionsItem::Show(aOptionsDialog * _dialog) {
	if (m_isVisible) { return; }
	m_isVisible = true;
	if (!m_widgetIsConstructed) { constructWidget(); }
	_dialog->addWidget(widget());
}

void ak::aAbstractOptionsItem::Hide(aOptionsDialog * _dialog) {
	if (!m_isVisible) { return; }
	m_isVisible = false;
	_dialog->removeWidget(widget());
}

// #############################################################################################################

// Setter

void ak::aAbstractOptionsItem::attachToGroup(aOptionsGroup * _group) {
	if (m_parentGroup) {
		aAssert(0, "The item is already attached to a group");
	}
	m_parentGroup = _group;
	if (m_parentGroup->navigationID() != invalidID) { displayNavigationItem(); }
}

void ak::aAbstractOptionsItem::displayNavigationItem(void) {
	if (m_navigationId != invalidID) { return; }
	if (m_parentGroup) {
		aOptionsDialog * dialog = m_parentGroup->getDialog();
		if (dialog) {
			m_navigationId = dialog->navigation()->add(m_parentGroup->navigationID(), m_title);
			dialog->navigation()->setItemVisible(m_navigationId, false);
		}
		else { aAssert(0, "None of the parent groups is attached to a dialog"); }
	}
	else {
		aAssert(0, "The item is not attached to a group");
	}
}

void ak::aAbstractOptionsItem::hideNavigationItem(void) {
	if (m_parentGroup) {
		if (m_parentGroup->navigationID() == invalidID) { m_navigationId = invalidID; }
	}
	if (m_navigationId == invalidID) { return; }
	if (m_parentGroup == nullptr) { aAssert(0, "The item is not attached to any group"); return; }
	aOptionsDialog * dia = m_parentGroup->getDialog();
	if (dia) { dia->navigation()->deleteItem(m_navigationId); m_navigationId = invalidID; }
}

// #############################################################################################################

// Getter

QString ak::aAbstractOptionsItem::logicalName(const QString& _delimiter) const {
	QString ret;
	if (m_parentGroup) { ret = m_parentGroup->logicalName(_delimiter).append(_delimiter); }
	ret.append(m_name);
	return ret;
}

// #############################################################################################################

// Private functions

void ak::aAbstractOptionsItem::emitChanged(void) {
	if (m_parentGroup) { m_parentGroup->itemChanged(this); }
}


// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aBasicOptionsItem::aBasicOptionsItem(optionItemType _type, const QString& _name, const QString& _title)
	: aAbstractOptionsItem(_type, _name, _title), m_label(nullptr), m_supressEvents(false), m_showLabel(true)
{
	m_centralLayoutW = new QWidget;
	m_centralLayout = new QHBoxLayout(m_centralLayoutW);
	m_centralLayout->setContentsMargins(0, 0, 0, 0);
	m_label = new aLabelWidget(_title);
}

ak::aBasicOptionsItem::~aBasicOptionsItem() {
	delete m_label;
	delete m_centralLayout;
	delete m_centralLayoutW;
}

// #############################################################################################################

// Base class functions

void ak::aBasicOptionsItem::ConstructWidget(void) {	
	if (m_showLabel) { m_centralLayout->addWidget(m_label, 0); }
	addItemWidgetsToLayout(m_centralLayout);
}

// #############################################################################################################

// Setter

void ak::aBasicOptionsItem::setExplanation(const QString& _explanation) {
	if (m_label) { m_label->setToolTip(_explanation); }
}

void ak::aBasicOptionsItem::setLabelVisible(bool _isVisible) {
	m_showLabel = _isVisible;
}

void ak::aBasicOptionsItem::setLabel(const QString& _text) {
	m_label->setText(_text);
}

// #############################################################################################################

// Getter

QString ak::aBasicOptionsItem::labelText(void) const {
	return m_label->text();
}

// #############################################################################################################

// Slots

void ak::aBasicOptionsItem::slotChanged(void) {
	if (m_supressEvents) { return; }
	emitChanged();
}

void ak::aBasicOptionsItem::slotChanged(int _v) {
	if (m_supressEvents) { return; }
	emitChanged();
}

void ak::aBasicOptionsItem::slotChanged(double _v) {
	if (m_supressEvents) { return; }
	emitChanged();
}

void ak::aBasicOptionsItem::slotChanged(const QString& _v) {
	if (m_supressEvents) { return; }
	emitChanged();
}

void ak::aBasicOptionsItem::slotChanged(QListWidgetItem * _v) {
	if (m_supressEvents) { return; }
	emitChanged();
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aOptionsItemInfoText::aOptionsItemInfoText(const QString& _name, const QString& _title)
	: aBasicOptionsItem(InfoText, _name, _title) {}

ak::aOptionsItemInfoText::~aOptionsItemInfoText() {}

// #############################################################################################################

// Setter

void ak::aOptionsItemInfoText::setText(const QString& _text) { m_label->setText(_text); }

// #############################################################################################################

// Getter

QString ak::aOptionsItemInfoText::text(void) const { return m_label->text(); }

ak::aLabelWidget * ak::aOptionsItemInfoText::label(void) { return m_label; }

// #############################################################################################################

// Protected functions

void ak::aOptionsItemInfoText::addItemWidgetsToLayout(QHBoxLayout * _layout) {
	if (!m_showLabel) { _layout->addWidget(m_label); }
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aOptionsItemCheckBox::aOptionsItemCheckBox(const QString& _name, const QString& _title, bool _isChecked)
	: aBasicOptionsItem(CheckBox, _name, _title), m_isChecked(_isChecked)
{
	if (m_isChecked) { m_state = Qt::Checked; }
	else { m_state = Qt::Unchecked; }
	m_checkbox = new aCheckBoxWidget;
	m_checkbox->setCheckState(m_state);
	connect(m_checkbox, &aCheckBoxWidget::stateChanged, this, &aOptionsItemCheckBox::slotCheckStateChanged);
}

ak::aOptionsItemCheckBox::~aOptionsItemCheckBox() {

}

// #############################################################################################################

// Setter

void ak::aOptionsItemCheckBox::setExplanation(const QString& _explanation) {
	aBasicOptionsItem::setExplanation(_explanation);
	if (m_checkbox) { m_checkbox->setToolTip(_explanation); }
}

void ak::aOptionsItemCheckBox::setChecked(bool _isChecked) {
	m_isChecked = _isChecked;
	if (m_isChecked) { m_state = Qt::Checked; }
	else { m_state = Qt::Unchecked; }
	m_checkbox->setCheckState(m_state);
	m_checkbox->setTristate(false);
}

void ak::aOptionsItemCheckBox::setTristate(void) {
	m_checkbox->setTristate(true);
	m_checkbox->setCheckState(Qt::PartiallyChecked);
}

// #############################################################################################################

// Protected functions

void ak::aOptionsItemCheckBox::addItemWidgetsToLayout(QHBoxLayout * _layout) {
	_layout->addWidget(m_checkbox, 1);
}

// #############################################################################################################

// Protected functions

void ak::aOptionsItemCheckBox::slotCheckStateChanged(int _state) {
	if (_state == Qt::Checked) {
		m_checkbox->setTristate(false);
		m_isChecked = true;
		m_state = Qt::Checked;
	}
	else if (_state == Qt::Unchecked) {
		m_checkbox->setTristate(false);
		m_isChecked = false;
		m_state = Qt::Unchecked;
	}
	else if (_state == Qt::PartiallyChecked) {
		m_state = Qt::PartiallyChecked;
	}
	slotChanged();
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aOptionsItemLineEdit::aOptionsItemLineEdit(const QString& _name, const QString& _title, const QString& _text)
	: aBasicOptionsItem(LineEdit, _name, _title)
{
	m_lineEdit = new aLineEditWidget(_text);
	connect(m_lineEdit, &aLineEditWidget::finishedChanges, this, qOverload<>(&aOptionsItemLineEdit::slotChanged));
}

ak::aOptionsItemLineEdit::~aOptionsItemLineEdit() {
	delete m_lineEdit;
}

// #############################################################################################################

// Setter

void ak::aOptionsItemLineEdit::setExplanation(const QString& _explanation) {
	aBasicOptionsItem::setExplanation(_explanation);
	if (m_lineEdit) { m_lineEdit->setToolTip(_explanation); }
}

void ak::aOptionsItemLineEdit::setText(const QString& _text) {
	m_lineEdit->setText(_text);
}

void ak::aOptionsItemLineEdit::setPlaceholderText(const QString& _placeholderText) {
	m_lineEdit->setPlaceholderText(_placeholderText);
}

// #############################################################################################################

// Getter

QString ak::aOptionsItemLineEdit::text(void) const {
	return m_lineEdit->text();
}

QString ak::aOptionsItemLineEdit::placeholderText(void) const {
	return m_lineEdit->placeholderText();
}

void ak::aOptionsItemLineEdit::addItemWidgetsToLayout(QHBoxLayout * _layout) {
	_layout->addWidget(m_lineEdit, 1);
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aOptionsItemSpinBox::aOptionsItemSpinBox(const QString& _name, const QString& _title, int _value, int _min, int _max)
	: aBasicOptionsItem(SpinBox, _name, _title)
{
	m_spinBox = new aSpinBoxWidget;
	m_spinBox->setMinimum(_min);
	m_spinBox->setMaximum(_max);
	m_spinBox->setValue(_value);
	connect(m_spinBox, &aSpinBoxWidget::aValueChanged, this, qOverload<int>(&aOptionsItemSpinBox::slotChanged));
}

ak::aOptionsItemSpinBox::~aOptionsItemSpinBox() {
	delete m_spinBox;
}

// #############################################################################################################

// Setter

void ak::aOptionsItemSpinBox::setExplanation(const QString& _explanation) {
	aBasicOptionsItem::setExplanation(_explanation);
	if (m_spinBox) { m_spinBox->setToolTip(_explanation); }
}

void ak::aOptionsItemSpinBox::setValue(int _value) {
	m_spinBox->setValue(_value);
}

void ak::aOptionsItemSpinBox::setMinValue(int _minValue) {
	m_spinBox->setMinimum(_minValue);
}

void ak::aOptionsItemSpinBox::setMaxValue(int _maxValue) {
	m_spinBox->setMaximum(_maxValue);
}

void ak::aOptionsItemSpinBox::setLimits(int _min, int _max) {
	m_spinBox->setMinimum(_min);
	m_spinBox->setMaximum(_max);
}

// #############################################################################################################

// Getter

int ak::aOptionsItemSpinBox::value(void) const {
	return m_spinBox->value();
}

int ak::aOptionsItemSpinBox::minValue(void) const {
	return m_spinBox->minimum();
}

int ak::aOptionsItemSpinBox::maxValue(void) const {
	return m_spinBox->maximum();
}

// #############################################################################################################

// Protected functions

void ak::aOptionsItemSpinBox::addItemWidgetsToLayout(QHBoxLayout * _layout) {
	_layout->addWidget(m_spinBox, 1);
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aOptionsItemDoubleSpinBox::aOptionsItemDoubleSpinBox(const QString& _name, const QString& _title, double _value, double _min, double _max, int _decimals)
	: aBasicOptionsItem(DoubleSpinBox, _name, _title)
{
	m_spinBox = new aDoubleSpinBoxWidget;
	m_spinBox->setMinimum(_min);
	m_spinBox->setMaximum(_max);
	m_spinBox->setValue(_value);
	m_spinBox->setDecimals(_decimals);
	connect(m_spinBox, qOverload<double>(&aDoubleSpinBoxWidget::valueChanged), this, qOverload<double>(&aOptionsItemDoubleSpinBox::slotChanged));
}

ak::aOptionsItemDoubleSpinBox::~aOptionsItemDoubleSpinBox() {
	delete m_spinBox;
}

// #############################################################################################################

// Setter

void ak::aOptionsItemDoubleSpinBox::setExplanation(const QString& _explanation) {
	aBasicOptionsItem::setExplanation(_explanation);
	if (m_spinBox) { m_spinBox->setToolTip(_explanation); }
}

void ak::aOptionsItemDoubleSpinBox::setValue(double _value) {
	m_spinBox->setValue(_value);
}

void ak::aOptionsItemDoubleSpinBox::setMinValue(double _minValue) {
	m_spinBox->setMinimum(_minValue);
}

void ak::aOptionsItemDoubleSpinBox::setMaxValue(double _maxValue) {
	m_spinBox->setMaximum(_maxValue);
}

void ak::aOptionsItemDoubleSpinBox::setLimits(double _min, double _max) {
	m_spinBox->setMinimum(_min);
	m_spinBox->setMaximum(_max);
}

void ak::aOptionsItemDoubleSpinBox::setDecimals(int _decimals) {
	m_spinBox->setDecimals(_decimals);
}

// #############################################################################################################

// Getter

double ak::aOptionsItemDoubleSpinBox::value(void) const {
	return m_spinBox->value();
}

double ak::aOptionsItemDoubleSpinBox::minValue(void) const {
	return m_spinBox->minimum();
}

double ak::aOptionsItemDoubleSpinBox::maxValue(void) const {
	return m_spinBox->maximum();
}

int ak::aOptionsItemDoubleSpinBox::decimals(void) const {
	return m_spinBox->decimals();
}

// #############################################################################################################

// Protected functions

void ak::aOptionsItemDoubleSpinBox::addItemWidgetsToLayout(QHBoxLayout * _layout) {
	_layout->addWidget(m_spinBox, 1);
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aOptionsItemComboBox::aOptionsItemComboBox(const QString& _name, const QString& _title, const QStringList& _possibleSelection, const QString& _value)
	: aBasicOptionsItem(ComboBox, _name, _title)
{
	m_comboBox = new aComboBoxWidget;
	m_comboBox->addItems(_possibleSelection);
	m_comboBox->setEditable(true);
	m_comboBox->setCurrentText(_value);
	connect(m_comboBox, &aComboBoxWidget::currentTextChanged, this, qOverload<const QString&>(&aOptionsItemComboBox::slotChanged));
}

ak::aOptionsItemComboBox::~aOptionsItemComboBox() {
	delete m_comboBox;
}

// #############################################################################################################

// Setter

void ak::aOptionsItemComboBox::setExplanation(const QString& _explanation) {
	aBasicOptionsItem::setExplanation(_explanation);
	if (m_comboBox) { m_comboBox->setToolTip(_explanation); }
}

void ak::aOptionsItemComboBox::setPossibleSelection(const QStringList& _possibleSelection) {
	m_comboBox->clear();
	m_comboBox->addItems(_possibleSelection);
}

void ak::aOptionsItemComboBox::setText(const QString& _text) {
	m_comboBox->setCurrentText(_text);
}

// #############################################################################################################

// Getter

QStringList ak::aOptionsItemComboBox::possibleSelection(void) const {
	QStringList lst;
	for (int i{ 0 }; i < m_comboBox->count(); i++) { lst.push_back(m_comboBox->itemText(i)); }
	return lst;
}

QString ak::aOptionsItemComboBox::text(void) const {
	return m_comboBox->currentText();
}

// #############################################################################################################

// Protected functions

void ak::aOptionsItemComboBox::addItemWidgetsToLayout(QHBoxLayout * _layout) {
	_layout->addWidget(m_comboBox, 1);
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aOptionsItemComboButton::aOptionsItemComboButton(const QString& _name, const QString& _title, const QStringList& _possibleSelection, const QString& _value)
	: aBasicOptionsItem(ComboBox, _name, _title)
{
	m_comboButton = new aComboButtonWidget;
	m_comboButton->setItems(_possibleSelection);
	m_comboButton->setText(_value);
	connect(m_comboButton, &aComboButtonWidget::changed, this, qOverload<>(&aOptionsItemComboButton::slotChanged));
}

ak::aOptionsItemComboButton::~aOptionsItemComboButton() {
	delete m_comboButton;
}

// #############################################################################################################

// Setter

void ak::aOptionsItemComboButton::setExplanation(const QString& _explanation) {
	aBasicOptionsItem::setExplanation(_explanation);
	if (m_comboButton) { m_comboButton->setToolTip(_explanation); }
}

void ak::aOptionsItemComboButton::setPossibleSelection(const QStringList& _possibleSelection) {
	m_comboButton->setItems(_possibleSelection);
}

void ak::aOptionsItemComboButton::setText(const QString& _text) {
	m_comboButton->setText(_text);
}

// #############################################################################################################

// Getter

QStringList ak::aOptionsItemComboButton::possibleSelection(void) const {
	QStringList lst;
	for (auto itm : m_comboButton->items()) { lst.push_back(itm->text()); }
	return lst;
}

QString ak::aOptionsItemComboButton::text(void) const {
	return m_comboButton->text();
}

// #############################################################################################################

// Protected functions

void ak::aOptionsItemComboButton::addItemWidgetsToLayout(QHBoxLayout * _layout) {
	_layout->addWidget(m_comboButton, 1);
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aOptionsItemColorEdit::aOptionsItemColorEdit(const QString& _name, const QString& _title, const aColor& _color)
	: aBasicOptionsItem(ColorSelect, _name, _title)
{
	m_colorEdit = new aColorEditButtonWidget(_color);
	connect(m_colorEdit, &aColorEditButtonWidget::changed, this, qOverload<>(&aOptionsItemColorEdit::slotChanged));
}

ak::aOptionsItemColorEdit::~aOptionsItemColorEdit() {
	delete m_colorEdit;
}

// #############################################################################################################

// Setter

void ak::aOptionsItemColorEdit::setExplanation(const QString& _explanation) {
	aBasicOptionsItem::setExplanation(_explanation);
	if (m_colorEdit) { m_colorEdit->pushButton()->setToolTip(_explanation); }
}

void ak::aOptionsItemColorEdit::setColor(const aColor& _color) {
	m_colorEdit->setColor(_color);
}

void ak::aOptionsItemColorEdit::setEditAlphaChannel(bool _isActive) {
	m_colorEdit->setEditAlphaChannel(_isActive);
}

// #############################################################################################################

// Getter

ak::aColor ak::aOptionsItemColorEdit::color(void) const {
	return m_colorEdit->color();
}

bool ak::aOptionsItemColorEdit::editAlphaChannel(void) const {
	return m_colorEdit->editAlphaChannel();
}

// #############################################################################################################

// Protected functions

void ak::aOptionsItemColorEdit::addItemWidgetsToLayout(QHBoxLayout * _layout) {
	_layout->addWidget(m_colorEdit->widget(), 0);
	_layout->addStretch(1);
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aOptionsItemList::aOptionsItemList(const QString& _name, const QString& _title, const QStringList& _possibleSelection, const QStringList& _currentSelection)
	: aBasicOptionsItem(List, _name, _title)
{
	m_list = new aListWidget;
	m_list->AddItems(_possibleSelection);
	m_list->setSelectedItems(_currentSelection);
}

ak::aOptionsItemList::~aOptionsItemList() {
	delete m_list;
}

// #############################################################################################################

// Setter

void ak::aOptionsItemList::setExplanation(const QString& _explanation) {
	aBasicOptionsItem::setExplanation(_explanation);
	if (m_list) { m_list->setToolTip(_explanation); }
}

void ak::aOptionsItemList::setPossibleSelection(const QStringList& _possibleSelection) {
	m_list->Clear();
	m_list->AddItems(_possibleSelection);
}

void ak::aOptionsItemList::setCurrentSelection(const QStringList& _selection) {
	m_list->setSelectedItems(_selection);
}

// #############################################################################################################

// Getter

QStringList ak::aOptionsItemList::possibleSelection(void) const {
	QStringList lst;
	for (auto itm : m_list->items()) { lst.push_back(itm.second->text()); }
	return lst;
}

QStringList ak::aOptionsItemList::currentSelection(void) const {
	QStringList lst;
	for (auto itm : m_list->selectedItems()) { lst.push_back(itm->text()); }
	return lst;
}

// #############################################################################################################

// Protected functions

void ak::aOptionsItemList::addItemWidgetsToLayout(QHBoxLayout * _layout) {
	_layout->addWidget(m_list);
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aOptionsItemDirectorySelect::aOptionsItemDirectorySelect(const QString& _name, const QString& _title, const QString& _initialDirectory) 
	: aBasicOptionsItem(DirectorySelect, _name, _title)
{
	m_edit = new aLineEditWidget(_initialDirectory);
	if (_initialDirectory.isEmpty()) { m_edit->setText(QDir::currentPath()); }
	m_button = new aPushButtonWidget("Search");
	connect(m_button, &aPushButtonWidget::clicked, this, &aOptionsItemDirectorySelect::slotButtonClicked);
	connect(m_edit, &aLineEditWidget::finishedChanges, this, qOverload<>(&aOptionsItemDirectorySelect::slotChanged));
}

ak::aOptionsItemDirectorySelect::~aOptionsItemDirectorySelect() {
	delete m_edit;
	delete m_button;
}

// #############################################################################################################

// Setter

void ak::aOptionsItemDirectorySelect::setExplanation(const QString& _explanation) {
	aBasicOptionsItem::setExplanation(_explanation);
	if (m_edit) { m_edit->setToolTip(_explanation); }
	if (m_button) { m_button->setToolTip(_explanation); }
}

void ak::aOptionsItemDirectorySelect::setSelectedDirectory(const QString& _directory) {
	m_edit->setText(_directory);
}

// #############################################################################################################

// Getter

QString ak::aOptionsItemDirectorySelect::selectedDirectory(void) const {
	return m_edit->text();
}

// #############################################################################################################

// Protected functions

void ak::aOptionsItemDirectorySelect::addItemWidgetsToLayout(QHBoxLayout * _layout) {
	_layout->addWidget(m_edit, 1);
	_layout->addWidget(m_button, 0);
}

// #############################################################################################################

// Slots functions

void ak::aOptionsItemDirectorySelect::slotButtonClicked(void) {
	QFileDialog dia;
	QString dir = dia.getExistingDirectory(m_edit, labelText(), m_edit->text(), QFileDialog::Option::ShowDirsOnly);
	if (!dir.isEmpty()) { m_edit->setText(dir); slotChanged(); }
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aOptionsItemFileSelectSave::aOptionsItemFileSelectSave(const QString& _name, const QString& _title, const QString& _initialDirectory, const QString& _filter)
	: aBasicOptionsItem(FileSelectSave, _name, _title), m_filter(_filter)
{
	m_edit = new aLineEditWidget(_initialDirectory);
	m_button = new aPushButtonWidget("Search");
	connect(m_button, &aPushButtonWidget::clicked, this, &aOptionsItemFileSelectSave::slotButtonClicked);
	connect(m_edit, &aLineEditWidget::finishedChanges, this, qOverload<>(&aOptionsItemFileSelectSave::slotChanged));
}

ak::aOptionsItemFileSelectSave::~aOptionsItemFileSelectSave() {
	delete m_edit;
	delete m_button;
}

// #############################################################################################################

// Setter

void ak::aOptionsItemFileSelectSave::setExplanation(const QString& _explanation) {
	aBasicOptionsItem::setExplanation(_explanation);
	if (m_edit) { m_edit->setToolTip(_explanation); }
	if (m_button) { m_button->setToolTip(_explanation); }
}

void ak::aOptionsItemFileSelectSave::setSelectedFile(const QString& _directory) {
	m_edit->setText(_directory);
}

// #############################################################################################################

// Getter

QString ak::aOptionsItemFileSelectSave::selectedFile(void) const {
	return m_edit->text();
}

// #############################################################################################################

// Protected functions

void ak::aOptionsItemFileSelectSave::addItemWidgetsToLayout(QHBoxLayout * _layout) {
	_layout->addWidget(m_edit, 1);
	_layout->addWidget(m_button, 0);
}

// #############################################################################################################

// Slots functions

void ak::aOptionsItemFileSelectSave::slotButtonClicked(void) {
	QFileDialog dia;
	QString dir = dia.getSaveFileName(m_edit, labelText(), m_edit->text(), m_filter);
	if (!dir.isEmpty()) { m_edit->setText(dir); slotChanged(); }
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ak::aOptionsItemFileSelectOpen::aOptionsItemFileSelectOpen(const QString& _name, const QString& _title, const QString& _initialDirectory, const QString& _filter)
	: aBasicOptionsItem(FileSelectOpen, _name, _title), m_filter(_filter)
{
	m_edit = new aLineEditWidget(_initialDirectory);
	m_button = new aPushButtonWidget("Search");
	connect(m_button, &aPushButtonWidget::clicked, this, &aOptionsItemFileSelectOpen::slotButtonClicked);
	connect(m_edit, &aLineEditWidget::finishedChanges, this, qOverload<>(&aOptionsItemFileSelectOpen::slotChanged));
}

ak::aOptionsItemFileSelectOpen::~aOptionsItemFileSelectOpen() {
	delete m_edit;
	delete m_button;
}

// #############################################################################################################

// Setter

void ak::aOptionsItemFileSelectOpen::setExplanation(const QString& _explanation) {
	aBasicOptionsItem::setExplanation(_explanation);
	if (m_edit) { m_edit->setToolTip(_explanation); }
	if (m_button) { m_button->setToolTip(_explanation); }
}

void ak::aOptionsItemFileSelectOpen::setSelectedFile(const QString& _directory) {
	m_edit->setText(_directory);
}

// #############################################################################################################

// Getter

QString ak::aOptionsItemFileSelectOpen::selectedFile(void) const {
	return m_edit->text();
}

// #############################################################################################################

// Protected functions

void ak::aOptionsItemFileSelectOpen::addItemWidgetsToLayout(QHBoxLayout * _layout) {
	_layout->addWidget(m_edit, 1);
	_layout->addWidget(m_button, 0);
}

// #############################################################################################################

// Slots functions

void ak::aOptionsItemFileSelectOpen::slotButtonClicked(void) {
	QFileDialog dia;
	QString dir = dia.getOpenFileName(m_edit, labelText(), m_edit->text(), m_filter);
	if (!dir.isEmpty()) { m_edit->setText(dir); slotChanged(); }
}
