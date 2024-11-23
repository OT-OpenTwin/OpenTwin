/*
 *	File:		aSignalLinker.cpp
 *	Package:	akGui
 *
 *  Created on: February 21, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

 // AK Core header
#include <akCore/aException.h>
#include <akCore/aMessenger.h>
#include <akCore/aSingletonAllowedMessages.h>

// AK GUI header
#include <akGui/aAction.h>
#include <akGui/aSignalLinker.h>
#include <akGui/aTimer.h>

// AK Widgets header
#include <akWidgets/aCheckBoxWidget.h>
#include <akWidgets/aComboBoxWidget.h>
#include <akWidgets/aComboButtonWidget.h>
#include <akWidgets/aLineEditWidget.h>
#include <akWidgets/aNiceLineEditWidget.h>
#include <akWidgets/aPushButtonWidget.h>
#include <akWidgets/aTabWidget.h>
#include <akWidgets/aToolButtonWidget.h>

// Qt header
#include <qmessagebox.h>			// QMessageBox
#include <qtreewidget.h>
#include <qstring.h>
#include <qevent.h>

ak::aSignalLinker::aSignalLinker(
	aMessenger *							_messanger,
	aUidManager *							_uidManager
)	: m_messanger(nullptr),
	m_uidManager(nullptr),
	m_uid(0)
{
	try {
		if (_uidManager == nullptr) { throw aException("Is nullptr", "Check UID manager"); }
		if (_messanger == nullptr) { throw aException("Is nullptr", "Check messanger"); }
		m_uidManager = _uidManager;
		m_messanger = _messanger;
		m_uid = m_uidManager->getId();
	}
	catch (const aException & e) { throw aException(e, "ak::aSignalLinker::aSignalLinker()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aSignalLinker::aSignalLinker()"); }
	catch (...) { throw aException("Unknown error", "ak::aSignalLinker::aSignalLinker()"); }
}

ak::aSignalLinker::~aSignalLinker()
{
	for (m_objectsIterator itm = m_objects.begin(); itm != m_objects.end(); itm++) {
		switch (itm->second.type)
		{
		case otAction:
			itm->second.object->disconnect(itm->second.object, SIGNAL(changed()), this, SLOT(slotChanged()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(triggered()), this, SLOT(slotClicked()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(hovered()), this, SLOT(slotFocused()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
			break;
		case otCheckBox:
			itm->second.object->disconnect(itm->second.object, SIGNAL(clicked()), this, SLOT(slotClicked()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(stateChanged(int)), this, SLOT(slotStateChanged(int)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotKeyPressed(QKeyEvent *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyReleased(QKeyEvent *)), this, SLOT(slotKeyReleased(QKeyEvent *)));
			break;
		case otComboBox:
			itm->second.object->disconnect(itm->second.object, SIGNAL(activated(int)), this, SLOT(slotIndexActivated(int)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(currentIndexChanged(int)), this, SLOT(slotIndexChanged(int)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotKeyPressed(QKeyEvent *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyReleased(QKeyEvent *)), this, SLOT(slotKeyReleased(QKeyEvent *)));
			break;
		case otComboButton:
			itm->second.object->disconnect(itm->second.object, SIGNAL(clicked()), this, SLOT(slotClicked()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(changed()), this, SLOT(slotChanged()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotKeyPressed(QKeyEvent *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyReleased(QKeyEvent *)), this, SLOT(slotKeyReleased(QKeyEvent *)));
			break;
		case otLineEdit:
			itm->second.object->disconnect(itm->second.object, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(slotCursorPositionChangedIndex(int, int)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(textChanged()), this, SLOT(slotChanged()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotKeyPressed(QKeyEvent *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyReleased(QKeyEvent *)), this, SLOT(slotKeyReleased(QKeyEvent *)));
			break;
		case otNiceLineEdit:
			itm->second.object->disconnect(itm->second.object, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(slotCursorPositionChangedIndex(int, int)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(textChanged(const QString &)), this, SLOT(slotChanged()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotKeyPressed(QKeyEvent *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyReleased(QKeyEvent *)), this, SLOT(slotKeyReleased(QKeyEvent *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));
			break;
		case otPushButton:
			itm->second.object->disconnect(itm->second.object, SIGNAL(clicked()), this, SLOT(slotClicked()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotKeyPressed(QKeyEvent *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyReleased(QKeyEvent *)), this, SLOT(slotKeyReleased(QKeyEvent *)));
			break;
		case otTabView:
			itm->second.object->disconnect(itm->second.object, SIGNAL(currentChanged(int)), this, SLOT(slotItemChanged(int)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(tabBarClicked(int)), this, SLOT(slotItemClicked(int)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(tabCloseRequested(int)), this, SLOT(slotItemClicked(int)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(slotItemDoubleClicked(int)));
			break;
		case otTextEdit:
			itm->second.object->disconnect(itm->second.object, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(textChanged()), this, SLOT(slotChanged()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotKeyPressed(QKeyEvent *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyReleased(QKeyEvent *)), this, SLOT(slotKeyReleased(QKeyEvent *)));
			break;
		case otTimer:
			itm->second.object->disconnect(itm->second.object, SIGNAL(timeout()), this, SLOT(slotTimeout()));
			break;
		case otToolButton:
			itm->second.object->disconnect(itm->second.object, SIGNAL(btnClicked()), this, SLOT(slotClicked()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(slotKeyReleased(QKeyEvent *)), this, SLOT(slotKeyReleased(QKeyEvent *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotKeyPressed(QKeyEvent *)));
			break;
		case otTree:
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotKeyPressed(QKeyEvent *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(keyReleased(QKeyEvent *)), this, SLOT(slotKeyReleased(QKeyEvent *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(cleared()), this, SLOT(slotCleared()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(focusLost()), this, SLOT(slotFocusLost()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
			itm->second.object->disconnect(itm->second.object, SIGNAL(itemActivated(QTreeWidgetItem *, int)), this, SLOT(slotTreeItemActivated(QTreeWidgetItem *, int)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(slotTreeItemChanged(QTreeWidgetItem *, int)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(slotTreeItemClicked(QTreeWidgetItem *, int)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(itemCollapsed(QTreeWidgetItem *)), this, SLOT(slotTreeItemCollapsed(QTreeWidgetItem *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(slotTreeItemDoubleClicked(QTreeWidgetItem *, int)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(itemFocused(QTreeWidgetItem *)), this, SLOT(slotTreeItemFocused(QTreeWidgetItem *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(slotTreeItemExpanded(QTreeWidgetItem *)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(itemTextChanged(QTreeWidgetItem *, int)), this, SLOT(slotTreeItemChanged(QTreeWidgetItem *, int)));
			itm->second.object->disconnect(itm->second.object, SIGNAL(itemLocationChanged(QTreeWidgetItem *, int)), this, SLOT(slotTreeItemLocationChanged(QTreeWidgetItem *, int)));
			break;
		default:
			assert(0); // Not implemented object type
			break;
		}
	}
}

// ###################################################################################

// Add objects

ak::UID ak::aSignalLinker::addLink(
	aAction *								_object,
	UID												_objectUid
) {
	if (_objectUid == ak::invalidUID) { _objectUid = m_uidManager->getId(); }
	assert(m_objects.count(_objectUid) == 0); // Object with the provided UID already exists
	_object->setUid(_objectUid);
	m_objects.insert_or_assign(_objectUid, struct_object{ _object, otAction });

	_object->connect(_object, &aAction::changed, this, &aSignalLinker::slotChanged);
	_object->connect(_object, &aAction::triggered, this, &aSignalLinker::slotClicked);
	_object->connect(_object, &aAction::hovered, this, &aSignalLinker::slotFocused);
	_object->connect(_object, &aAction::toggled, this, &aSignalLinker::slotToggled);

	return _objectUid;
}

ak::UID ak::aSignalLinker::addLink(
	aCheckBoxWidget *								_object,
	UID												_objectUid
) {
	if (_objectUid == ak::invalidUID) { _objectUid = m_uidManager->getId(); }
	assert(m_objects.count(_objectUid) == 0); // Object with the provided UID already exists
	_object->setUid(_objectUid);
	m_objects.insert_or_assign(_objectUid, struct_object{ _object, otCheckBox });

	_object->connect(_object, &aCheckBoxWidget::clicked, this, &aSignalLinker::slotClicked);
	_object->connect(_object, &aCheckBoxWidget::toggled, this, &aSignalLinker::slotToggled);
	_object->connect(_object, &aCheckBoxWidget::stateChanged, this, &aSignalLinker::slotStateChanged);
	_object->connect(_object, &aCheckBoxWidget::keyPressed, this, &aSignalLinker::slotKeyPressed);
	_object->connect(_object, &aCheckBoxWidget::keyReleased, this, &aSignalLinker::slotKeyReleased);

	return _objectUid;
}

ak::UID ak::aSignalLinker::addLink(
	aComboBoxWidget *								_object,
	UID												_objectUid
) {
	if (_objectUid == ak::invalidUID) { _objectUid = m_uidManager->getId(); }
	assert(m_objects.count(_objectUid) == 0); // Object with the provided UID already exists
	_object->setUid(_objectUid);
	m_objects.insert_or_assign(_objectUid, struct_object{ _object, otComboBox });

	_object->connect(_object, qOverload<int>(&aComboBoxWidget::activated), this, &aSignalLinker::slotIndexActivated);
	_object->connect(_object, qOverload<int>(&aComboBoxWidget::currentIndexChanged), this, &aSignalLinker::slotIndexChanged);
	_object->connect(_object, &aComboBoxWidget::keyPressed, this, &aSignalLinker::slotKeyPressed);
	_object->connect(_object, &aComboBoxWidget::keyReleased, this, &aSignalLinker::slotKeyReleased);

	return _objectUid;
}

ak::UID ak::aSignalLinker::addLink(
	aComboButtonWidget *							_object,
	UID												_objectUid
) {
	if (_objectUid == ak::invalidUID) { _objectUid = m_uidManager->getId(); }
	assert(m_objects.count(_objectUid) == 0); // Object with the provided UID already exists
	_object->setUid(_objectUid);
	m_objects.insert_or_assign(_objectUid, struct_object{ _object, otComboButton });

	_object->connect(_object, &aComboButtonWidget::clicked, this, &aSignalLinker::slotClicked);
	_object->connect(_object, &aComboButtonWidget::toggled, this, &aSignalLinker::slotToggled);
	_object->connect(_object, &aComboButtonWidget::changed, this, &aSignalLinker::slotChanged);
	_object->connect(_object, &aComboButtonWidget::keyPressed, this, &aSignalLinker::slotKeyPressed);
	_object->connect(_object, &aComboButtonWidget::keyReleased, this, &aSignalLinker::slotKeyReleased);

	return _objectUid;
}

ak::UID ak::aSignalLinker::addLink(
	aLineEditWidget *								_object,
	UID												_objectUid
) {
	if (_objectUid == ak::invalidUID) { _objectUid = m_uidManager->getId(); }
	assert(m_objects.count(_objectUid) == 0); // Object with the provided UID already exists
	_object->setUid(_objectUid);
	m_objects.insert_or_assign(_objectUid, struct_object{ _object, otLineEdit });
	_object->connect(_object, &QLineEdit::cursorPositionChanged, this, &aSignalLinker::slotCursorPositionChangedIndex);
	_object->connect(_object, &QLineEdit::selectionChanged, this, &aSignalLinker::slotSelectionChanged);
	_object->connect(_object, &QLineEdit::textChanged, this, &aSignalLinker::slotChanged);
	_object->connect(_object, &aLineEditWidget::editingFinished, this, &aSignalLinker::slotEditingFinished);
	_object->connect(_object, &aLineEditWidget::keyPressed, this, &aSignalLinker::slotKeyPressed);
	_object->connect(_object, &aLineEditWidget::keyReleased, this, &aSignalLinker::slotKeyReleased);
	return _objectUid;
}

ak::UID ak::aSignalLinker::addLink(
	aNiceLineEditWidget *							_object,
	UID												_objectUid
) {
	if (_objectUid == ak::invalidUID) { _objectUid = m_uidManager->getId(); }
	assert(m_objects.count(_objectUid) == 0); // Object with the provided UID already exists
	_object->setUid(_objectUid);
	m_objects.insert_or_assign(_objectUid, struct_object{ _object, otNiceLineEdit });
	_object->connect(_object, &aNiceLineEditWidget::cursorPositionChanged, this, &aSignalLinker::slotCursorPositionChangedIndex);
	_object->connect(_object, &aNiceLineEditWidget::selectionChanged, this, &aSignalLinker::slotSelectionChanged);
	_object->connect(_object, &aNiceLineEditWidget::textChanged, this, &aSignalLinker::slotChanged);
	_object->connect(_object, &aNiceLineEditWidget::keyPressed, this, &aSignalLinker::slotKeyPressed);
	_object->connect(_object, &aNiceLineEditWidget::keyReleased, this, &aSignalLinker::slotKeyReleased);
	_object->connect(_object, &aNiceLineEditWidget::editingFinished, this, &aSignalLinker::slotEditingFinished);
	_object->connect(_object, &aNiceLineEditWidget::returnPressed, this, &aSignalLinker::slotReturnPressed);
	return _objectUid;
}

ak::UID ak::aSignalLinker::addLink(
	aPushButtonWidget *									_object,
	UID													_objectUid
) {
	if (_objectUid == ak::invalidUID) { _objectUid = m_uidManager->getId(); }
	assert(m_objects.count(_objectUid) == 0); // Object with the provided UID already exists
	_object->setUid(_objectUid);
	m_objects.insert_or_assign(_objectUid, struct_object{ _object, otPushButton });

	_object->connect(_object, &aPushButtonWidget::clicked, this, &aSignalLinker::slotClicked);
	_object->connect(_object, &aPushButtonWidget::toggled, this, &aSignalLinker::slotToggled);
	_object->connect(_object, &aPushButtonWidget::keyPressed, this, &aSignalLinker::slotKeyPressed);
	_object->connect(_object, &aPushButtonWidget::keyReleased, this, &aSignalLinker::slotKeyReleased);

	return _objectUid;
}

ak::UID ak::aSignalLinker::addLink(
	aTabWidget *										_object,
	UID													_objectUid
) {
	if (_objectUid == ak::invalidUID) { _objectUid = m_uidManager->getId(); }
	assert(m_objects.count(_objectUid) == 0); // Object with the provided UID already exists
	_object->setUid(_objectUid);
	m_objects.insert_or_assign(_objectUid, struct_object{ _object, otTabView });
	_object->connect(_object, &QTabWidget::currentChanged, this, &aSignalLinker::slotItemChanged);
	_object->connect(_object, &QTabWidget::tabBarClicked, this, &aSignalLinker::slotItemClicked);
	_object->connect(_object, &QTabWidget::tabCloseRequested, this, &aSignalLinker::slotItemClicked);
	_object->connect(_object, &QTabWidget::tabBarDoubleClicked, this, &aSignalLinker::slotItemDoubleClicked);
	return _objectUid;
}

ak::UID ak::aSignalLinker::addLink(
	aTimer *											_object,
	UID													_objectUid
) {
	if (_objectUid == ak::invalidUID) { _objectUid = m_uidManager->getId(); }
	assert(m_objects.count(_objectUid) == 0); // Object with the provided UID already exists
	_object->setUid(_objectUid);
	m_objects.insert_or_assign(_objectUid, struct_object{ _object, otTimer });

	_object->connect(_object, &aTimer::timeout, this, &aSignalLinker::slotTimeout);

	return _objectUid;
}

ak::UID ak::aSignalLinker::addLink(
	aToolButtonWidget *									_object,
	UID													_objectUid
) {
	if (_objectUid == ak::invalidUID) { _objectUid = m_uidManager->getId(); }
	assert(m_objects.count(_objectUid) == 0); // Object with the provided UID already exists
	_object->setUid(_objectUid);
	m_objects.insert_or_assign(_objectUid, struct_object{ _object, otToolButton });
	_object->connect(_object, &aToolButtonWidget::btnClicked, this, &aSignalLinker::slotClicked);
	_object->connect(_object, &aToolButtonWidget::keyPressed, this, &aSignalLinker::slotKeyPressed);
	_object->connect(_object, &aToolButtonWidget::keyReleased, this, &aSignalLinker::slotKeyReleased);
	return _objectUid;
}

// ###################################################################################

// SLOTS

void ak::aSignalLinker::slotChanged() {
	if (!aSingletonAllowedMessages::instance()->changedEvent()) { return; }
	// Cast object
	aObject * obj = nullptr;
	obj = dynamic_cast<aObject *>(sender());
	assert(obj != nullptr); // Cast failed
	raiseEvent(obj->uid(), etChanged, 0, 0);
}

void ak::aSignalLinker::slotCleared() {
	if (!aSingletonAllowedMessages::instance()->clearedEvent()) { return; }
	// Cast object
	aObject * obj = nullptr;
	obj = dynamic_cast<aObject *>(sender());
	assert(obj != nullptr); // Cast failed
	raiseEvent(obj->uid(), etCleared, 0, 0);
}

void ak::aSignalLinker::slotClicked() {
	if (!aSingletonAllowedMessages::instance()->clickedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etClicked, 0, 0);
}

void ak::aSignalLinker::slotCursorPositionChanged() {
	if (!aSingletonAllowedMessages::instance()->cursorPositionChangedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etCursorPosotionChanged, 0, 0);
}

void ak::aSignalLinker::slotCursorPositionChangedIndex(int _oldPos, int _newPos) {
	if (!aSingletonAllowedMessages::instance()->cursorPositionChangedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etCursorPosotionChanged, _oldPos, _newPos);
}

void ak::aSignalLinker::slotFocused() {
	if (!aSingletonAllowedMessages::instance()->focusedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etFocused, 0, 0);
}

void ak::aSignalLinker::slotFocusLost() {
	if (!aSingletonAllowedMessages::instance()->focusedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etFocusLeft, 0, 0);
}

void ak::aSignalLinker::slotIndexActivated(int _index) {
	if (!aSingletonAllowedMessages::instance()->activatedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etActivated, _index, 0);
}

void ak::aSignalLinker::slotIndexChanged(int _index) {
	if (!aSingletonAllowedMessages::instance()->indexChangedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etIndexChanged, _index, 0);
}

void ak::aSignalLinker::slotKeyPressed(QKeyEvent * _key) {
	if (!aSingletonAllowedMessages::instance()->keyPressedEvent()) { return; }
	keyType k = toKeyType(_key);
	if (k != keyUnknown) { raiseEvent(getSenderUid(sender()), etKeyPressed, 0, k); }
}

void ak::aSignalLinker::slotKeyReleased(QKeyEvent * _key) {
	if (!aSingletonAllowedMessages::instance()->keyPressedEvent()) { return; }
	keyType k = toKeyType(_key);
	if (k != keyUnknown) { raiseEvent(getSenderUid(sender()), etKeyReleased, 0, k); }
}

void ak::aSignalLinker::slotSelectionChanged() {
	if (!aSingletonAllowedMessages::instance()->selectionChangedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etSelectionChanged, 0, 0);
}

void ak::aSignalLinker::slotStateChanged(int _state) {
	if (!aSingletonAllowedMessages::instance()->stateChangedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etStateChanged, _state, 0);
}

void ak::aSignalLinker::slotToggled(bool _checked)
{
	if (_checked) {
		if (!aSingletonAllowedMessages::instance()->toggledCheckedEvent()) { return; }
		raiseEvent(getSenderUid(sender()), etToggeledChecked, 0, 0);
	}
	else {
		if (!aSingletonAllowedMessages::instance()->toggledUncheckedEvent()) { return; }
		raiseEvent(getSenderUid(sender()), etToggeledUnchecked, 0, 0);
	}
}

void ak::aSignalLinker::slotTimeout(void) { raiseEvent(getSenderUid(sender()), etTimeout, 0, 0); }

void ak::aSignalLinker::slotClosing(void) {
	raiseEvent(getSenderUid(sender()), etClosing, 0, 0);
}

void ak::aSignalLinker::slotReturnPressed(void) {
	raiseEvent(getSenderUid(sender()), etReturnPressed, 0, 0);
}

void ak::aSignalLinker::slotEditingFinished(void) {
	raiseEvent(getSenderUid(sender()), etEditingFinished, 0, 0);
}

void ak::aSignalLinker::slotKeyCombinationPressed(void) {
	raiseEvent(getSenderUid(sender()), etActivated, 0, 0);
}

// ##### Items

void ak::aSignalLinker::slotItemChanged(ak::ID _itemID) {
	if (!aSingletonAllowedMessages::instance()->changedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etChanged, _itemID, 0);
}

void ak::aSignalLinker::slotItemDeleted(ak::ID _itemID) {
	if (!aSingletonAllowedMessages::instance()->changedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etDeleted, _itemID, 0);
}

void ak::aSignalLinker::slotItemClicked(ak::ID _itemID) {
	if (!aSingletonAllowedMessages::instance()->clickedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etClicked, _itemID, 0);
}

void ak::aSignalLinker::slotItemCloseRequested(ak::ID _itemID) {
	if (!aSingletonAllowedMessages::instance()->closeRequestedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etClosing, _itemID, 0);
}

void ak::aSignalLinker::slotItemDoubleClicked(ak::ID _itemID) {
	if (!aSingletonAllowedMessages::instance()->doubleClickedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etDoubleClicked, _itemID, 0);
}

// ##### Table

void ak::aSignalLinker::tableCellActivated(int _row, int _coloumn) {
	if (!aSingletonAllowedMessages::instance()->activatedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etActivated, _row, _coloumn);
}

void ak::aSignalLinker::tableCellChanged(int _row, int _coloumn) {
	if (!aSingletonAllowedMessages::instance()->changedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etChanged, _row, _coloumn);
}

void ak::aSignalLinker::tableCellClicked(int _row, int _coloumn) {
	if (!aSingletonAllowedMessages::instance()->clickedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etClicked, _row, _coloumn);
}

void ak::aSignalLinker::tableCellDoubleClicked(int _row, int _coloumn) {
	if (!aSingletonAllowedMessages::instance()->doubleClickedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etDoubleClicked, _row, _coloumn);
}

void ak::aSignalLinker::tableCellEntered(int _row, int _coloumn) {
	if (!aSingletonAllowedMessages::instance()->focusedEvent()) { return; }
	raiseEvent(getSenderUid(sender()), etFocused, _row, _coloumn);
}

// ###################################################################################

// Private members

void ak::aSignalLinker::raiseEvent(
	UID													_senderUid,
	eventType											_eventType,
	int													_info1,
	int													_info2
) {
	try { 
		if (m_messanger == nullptr) { assert(0); throw aException("Is nullptr", "Check messanger"); }
		m_messanger->sendMessage(_senderUid, _eventType, _info1, _info2);
	}
	catch (const std::exception & e) {
		assert(0);
		QMessageBox msg;
		msg.setWindowTitle("Signal error");
		QString txt(e.what());
		txt.append(AK_CSTR_NL "@ ak::aSignalLinker::raiseEvent()");
		msg.setText(txt);
		msg.exec();
	}
	catch (...) {
		assert(0);
		QMessageBox msg;
		msg.setWindowTitle("Signal error");
		msg.setText("Unknown error at ak::aSignalLinker::raiseEvent()");
		msg.exec();
	}
}

ak::UID ak::aSignalLinker::getSenderUid(
	QObject * _sender
) { 
	aObject * obj = dynamic_cast<aObject *>(_sender);
	assert(obj != nullptr); // Invalid object stored
	return obj->uid();
}