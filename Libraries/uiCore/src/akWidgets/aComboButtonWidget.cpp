/*
 *	File:		aComboButtonWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: March 19, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akCore/aException.h>
#include <akWidgets/aComboButtonWidget.h>

// Qt header
#include <qmenu.h>					// dropDownMenu

ak::aComboButtonWidget::aComboButtonWidget(
	const QString &				_initialText,
	QWidget *					_parent
) : QPushButton(_initialText, _parent),
	ak::aWidget(otComboButton),
	m_menu(nullptr)
{
	try {
		// Create new QMenu to be a dropdown menu of the pushbutton
		m_menu = new QMenu;
		if (m_menu == nullptr) { throw aException("Failed to create", "Create menu"); }

		// Apply color style and menu
		setMenu(m_menu);
		m_itemsUIDmanager.setLatestUid(1);
		setObjectName("AK_ComboButtonWidget");
	}
	catch (const aException & e) { throw aException(e, "ak::aComboButtonWidget::aComboButtonWidget()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aComboButtonWidget::aComboButtonWidget()"); }
	catch (...) { throw aException("Unknown error", "ak::aComboButtonWidget::aComboButtonWidget()"); }
}

//! @brief Deconstructor
ak::aComboButtonWidget::~aComboButtonWidget() { A_OBJECT_DESTROYING clearItems(); }

// #######################################################################################################
// Event handling

void ak::aComboButtonWidget::keyPressEvent(QKeyEvent *_event)
{
	QPushButton::keyPressEvent(_event);
	emit keyPressed(_event);
}

void ak::aComboButtonWidget::keyReleaseEvent(QKeyEvent * _event) {
	QPushButton::keyReleaseEvent(_event);
	emit keyReleased(_event);
}

// #######################################################################################################

void ak::aComboButtonWidget::clearItems(void) {
	for (int i = 0; i < m_items.size(); i++){
		// Remove the action from the menu of this combo button
		m_menu->removeAction(m_items.at(i));
		// Disconnect the signal
		disconnect(m_items.at(i), SIGNAL(triggered()), this, SLOT(slotItemTriggered()));
		// Remove entry from the vector
		delete m_items.at(i);
	}
	m_items.clear();
	m_itemUids.clear();
	m_itemsUIDmanager.setLatestUid(1);
}

void ak::aComboButtonWidget::setItems(
	const std::vector<QString> &		_menu
) {
	try {
		clearItems();
		for (int i = 0; i < _menu.size(); i++) {
			// Create new item
			ak::aComboButtonWidgetItem * n_itm = nullptr;
			n_itm = new ak::aComboButtonWidgetItem(_menu.at(i));
			// Set the items UID
			n_itm->setUid(m_itemsUIDmanager.getId());
			// Add the new item to the menu
			m_menu->addAction(n_itm);
			// Store data
			m_items.push_back(n_itm);
			m_itemUids.insert_or_assign(n_itm->uid(), m_items.size() - 1);
			// Connect the triggered signal of the new item
			connect(n_itm, &aComboButtonWidgetItem::triggered, this, &aComboButtonWidget::slotItemTriggered);
		}
	}
	catch (const aException & e) { throw aException(e, "ak::aComboButtonWidget::setItems()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aComboButtonWidget::setItems()"); }
	catch (...) { throw aException("Unknown error", "ak::aComboButtonWidget::setItems()"); }
}

void ak::aComboButtonWidget::setItems(
	const QStringList &								_menu
) {
	try {
		clearItems();
		for (int i = 0; i < _menu.size(); i++) {
			// Create new item
			ak::aComboButtonWidgetItem * n_itm = nullptr;
			n_itm = new ak::aComboButtonWidgetItem(_menu.at(i));
			// Set the items UID
			n_itm->setUid(m_itemsUIDmanager.getId());
			// Add the new item to the menu
			m_menu->addAction(n_itm);
			// Store data
			m_items.push_back(n_itm);
			m_itemUids.insert_or_assign(n_itm->uid(), m_items.size() - 1);
			// Connect the triggered signal of the new item
			connect(n_itm, &aComboButtonWidgetItem::triggered, this, &aComboButtonWidget::slotItemTriggered);
		}
	}
	catch (const aException & e) { throw aException(e, "ak::aComboButtonWidget::setItems()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aComboButtonWidget::setItems()"); }
	catch (...) { throw aException("Unknown error", "ak::aComboButtonWidget::setItems()"); }
}

int ak::aComboButtonWidget::addItem(
	const QString &						_text
) {
	try {
		for (size_t i{ 0 }; i < m_items.size(); i++) {
			if (m_items[i]->text() == _text) { return i; }
		}

		// Create new item
		ak::aComboButtonWidgetItem * n_itm = nullptr;
		n_itm = new ak::aComboButtonWidgetItem(_text);
		if (n_itm == nullptr) { throw aException("Failed to create", "Create aComboButtonWidgetItem"); }
		// Set the items UID
		n_itm->setUid(m_itemsUIDmanager.getId());
		// Add the new item to the menu
		m_menu->addAction(n_itm);
		// Store data
		m_items.push_back(n_itm);
		m_itemUids.insert_or_assign(n_itm->uid(), m_items.size() - 1);
		// Connect the triggered signal of the new item
		connect(n_itm, SIGNAL(triggered()), this, SLOT(slotItemTriggered()));
		// Return the new items index in the menu
		return (m_items.size() - 1);
	}
	catch (const aException & e) { throw aException(e, "ak::aComboButtonWidget::addItem()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aComboButtonWidget::addItem()"); }
	catch (...) { throw aException("Unknown error", "ak::aComboButtonWidget::addItem()"); }
}

int ak::aComboButtonWidget::getItemCount(void) const { return m_items.size(); }

void ak::aComboButtonWidget::slotItemTriggered() {
	// Cast the QObject to the aComboButtonWidgetItem
	ak::aComboButtonWidgetItem * itm = dynamic_cast<ak::aComboButtonWidgetItem *>(sender());
	assert(itm != nullptr); // failed to cast. but item was expected
	auto obj = m_itemUids.find(itm->uid());
	assert(obj != m_itemUids.end()); // Invalid data stored
	setText(m_items.at(obj->second)->text());
	emit changed();
}

QWidget * ak::aComboButtonWidget::widget(void) { return this; }
