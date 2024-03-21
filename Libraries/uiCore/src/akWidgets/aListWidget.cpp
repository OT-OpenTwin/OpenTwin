/*
 *	File:		aListWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: October 12, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akWidgets/aListWidget.h>

// Qt header
#include <qevent.h>
#include <qscrollbar.h>

ak::aListWidget::aListWidget()
	: aWidget(otList), m_currentId(invalidID)
{}

ak::aListWidget::~aListWidget() {
	A_OBJECT_DESTROYING
	memFree();
}

QWidget * ak::aListWidget::widget(void) { return this; }

void ak::aListWidget::keyPressEvent(QKeyEvent *_event)
{ QListWidget::keyPressEvent(_event); emit keyPressed(_event); }

void ak::aListWidget::keyReleaseEvent(QKeyEvent * _event)
{ QListWidget::keyReleaseEvent(_event); emit keyReleased(_event); }

void ak::aListWidget::mouseMoveEvent(QMouseEvent * _event)
{ QListWidget::mouseMoveEvent(_event); emit mouseMove(_event); }

// ###########################################################################################################################################

//! @brief Will add a new item to this aListWidget
				//! @param _text The item text
ak::ID ak::aListWidget::AddItem(
	const QString &					_text
) {
	aListWidgetItem * nItm = new aListWidgetItem(_text, ++m_currentId, this);
	m_items.insert_or_assign(nItm->id(), nItm);
	//addItem(nItm);
	return m_currentId;
}

void ak::aListWidget::AddItems(
	const QStringList&			_itemLabels
) {
	for (auto itm : _itemLabels) {
		AddItem(itm);
	}
}

//! @brief Will add a new item to this aListWidget
//! @param _icon The item icon
//! @param _text The item text
ak::ID ak::aListWidget::AddItem(
	const QIcon &					_icon,
	const QString &					_text
) {
	aListWidgetItem * nItm = new aListWidgetItem(_icon, _text, ++m_currentId, this);
	m_items.insert_or_assign(nItm->id(), nItm);
	//addItem(nItm);
	return m_currentId;
}

ak::aListWidgetItem * ak::aListWidget::Item(
	const QString &					_text
) {
	for (auto itm : m_items) {
		if (itm.second->text() == _text) {
			return itm.second;
		}
	}
	assert(0); // Item does not exist
	return nullptr;
}

ak::aListWidgetItem * ak::aListWidget::Item(
	ak::ID							_id
) {
	auto itm = m_items.find(_id);
	assert(itm != m_items.end()); // Invalid item id provided
	assert(itm->second != nullptr);	// nullptr stored
	return itm->second;
}

QString ak::aListWidget::itemText(
	ak::ID							_id
) {
	aListWidgetItem * itm = Item(_id);
	return itm->text();
}

void ak::aListWidget::setItemText(
	ak::ID							_id,
	const QString &					_text
) {
	aListWidgetItem * itm = Item(_id);
	itm->setText(_text);
}

void ak::aListWidget::setItemIcon(
	ak::ID							_id,
	const QIcon &					_icon
) {
	aListWidgetItem * itm = Item(_id);
	itm->setIcon(_icon);
}

void ak::aListWidget::removeItem(
	ak::ID							_id
) {
	aListWidgetItem * itm = Item(_id);
	removeItemWidget(itm);
	delete itm;
	m_items.erase(_id);
}

void ak::aListWidget::setSelectedItems(
	const QStringList&			_selectedItems
) {
	for (auto itm : m_items) {
		bool found{ false };
		for (auto selected : _selectedItems) {
			if (itm.second->text() == selected) { found = true; break; }
		}
		if (found) { itm.second->setCheckState(Qt::Checked); }
		else { itm.second->setCheckState(Qt::Unchecked); }
	}
}

std::list<ak::aListWidgetItem *> ak::aListWidget::selectedItems(void) {
	std::list<ak::aListWidgetItem *> lst;
	for (auto itm : m_items) {
		if (itm.second->checkState() == Qt::Checked) { lst.push_back(itm.second); }
	}
	return lst;
}

void ak::aListWidget::Clear() { m_currentId = ak::invalidID; memFree(); clear(); }

void ak::aListWidget::setVerticalScrollbarAlwaysVisible(
	bool							_vis
) {
	setVerticalScrollBarPolicy((_vis ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAsNeeded));
}

void ak::aListWidget::memFree(void) {
	for (auto itm = m_items.begin(); itm != m_items.end(); itm++) {
		aListWidgetItem * actualItem = itm->second;
		assert(actualItem != nullptr);	// Nullptr stored
		delete actualItem;
	}
	m_items.clear();
}

// ###########################################################################################################################################

// ###########################################################################################################################################

// ###########################################################################################################################################

ak::aListWidgetItem::aListWidgetItem(
	ak::ID				_id,
	aListWidget *				_view,
	int					_type
) : QListWidgetItem(_view, _type), m_id(_id)
{}

ak::aListWidgetItem::aListWidgetItem(
	const QString &		_text,
	ak::ID				_id,
	aListWidget *				_view,
	int					_type
) : QListWidgetItem(_text, _view, _type), m_id(_id)
{}

ak::aListWidgetItem::aListWidgetItem(
	const QIcon &		_icon,
	const QString &		_text,
	ak::ID				_id,
	aListWidget *				_view,
	int					_type
) : QListWidgetItem(_icon, _text, _view, _type), m_id(_id)
{}

ak::aListWidgetItem::~aListWidgetItem() {}

void ak::aListWidgetItem::setId(
	ak::ID				_id
) { m_id = _id; }

ak::ID ak::aListWidgetItem::id(void) const { return m_id; }
