#include <akWidgets/aChecklistWidget.h>

#include <akGui/aColor.h>
#include <akWidgets/aCheckBoxWidget.h>
#include <akWidgets/aLineEditWidget.h>

#include <qlayout.h>
#include <qshortcut.h>
#include <qkeysequence.h>
#include <qevent.h>

ak::aChecklistItem::aChecklistItem(aChecklistWidget * _owner)
	: aObject(otNone), m_CheckBox(nullptr), m_TextEdit(nullptr), m_ChecklistWidget(_owner), m_Index(-1),
	m_shortcutChangeCheckedState(nullptr)
{
	m_CheckBox = new aCheckBoxWidget;
	m_TextEdit = new aLineEditWidget;

	ini();
}

ak::aChecklistItem::aChecklistItem(const QString& _text, bool _isChecked, aChecklistWidget * _owner)
	: aObject(otNone), m_CheckBox(nullptr), m_TextEdit(nullptr), m_ChecklistWidget(_owner), m_Index(-1),
	m_shortcutChangeCheckedState(nullptr), m_text(_text)
{
	m_CheckBox = new aCheckBoxWidget;
	m_CheckBox->setChecked(_isChecked);
	m_TextEdit = new aLineEditWidget(_text);
	ini();
}

ak::aChecklistItem::~aChecklistItem() {
	delete m_CheckBox;
	delete m_TextEdit;
}

// ################################################################################

void ak::aChecklistItem::setText(const QString& _text) {
	m_TextEdit->setText(_text);
	m_text = _text;
}

QString ak::aChecklistItem::text(void) const {
	return m_TextEdit->text();
}

void ak::aChecklistItem::setChecked(bool _isChecked) {
	m_CheckBox->setChecked(_isChecked);
}

bool ak::aChecklistItem::isChecked(void) const {
	return m_CheckBox->isChecked();
}

void ak::aChecklistItem::refreshAppearance(void) {
	QFont f = m_TextEdit->font();
	if (m_CheckBox->isChecked() && m_ChecklistWidget->isCrossoutCheckedItemsEnabled()) {
		f.setStrikeOut(true);
	}
	else {
		f.setStrikeOut(false);
	}
	f.setItalic(m_CheckBox->isChecked());
	f.setPointSize(m_ChecklistWidget->pointSize());
	m_TextEdit->setFont(f);
}

bool ak::aChecklistItem::checkTextModified(void) {
	if (m_TextEdit->text() != m_text) {
		m_text = m_TextEdit->text();
		return true;
	}
	return false;
}

// ################################################################################

// Private slots

void ak::aChecklistItem::slotCheckedChanged(int _state) {
	refreshAppearance();
	editingFinished(m_Index);
}

void ak::aChecklistItem::slotTextChanged(void) {
	if (m_text == m_TextEdit->text()) return;
	m_text = m_TextEdit->text();
	Q_EMIT textChanged(m_Index);
}

void ak::aChecklistItem::slotEditingFinished(void) {
	if (m_text == m_TextEdit->text()) return;
	m_text = m_TextEdit->text();
	Q_EMIT editingFinished(m_Index);
}

void ak::aChecklistItem::slotToggleChecked(void) {
	m_CheckBox->setChecked(!m_CheckBox->isChecked());
}

void ak::aChecklistItem::slotRequestDelete(void) {
	Q_EMIT requestDelete(m_Index);
}

void ak::aChecklistItem::slotReturnPressed(void) {
	slotTextChanged();
	if (m_TextEdit->text().isEmpty()) return;
	Q_EMIT returnPressed(m_Index);
}

void ak::aChecklistItem::slotKeyEvent(QKeyEvent * _event) {
	if (_event->key() == Qt::Key::Key_Up) { slotTextChanged(); Q_EMIT keyUpPressed(m_Index); }
	else if (_event->key() == Qt::Key::Key_Down) { slotTextChanged(); Q_EMIT keyDownPressed(m_Index); }
}

void ak::aChecklistItem::slotMoveUpRequested(void) {
	Q_EMIT moveUpRequested(m_Index);
}

void ak::aChecklistItem::slotMoveDownRequested(void) {
	Q_EMIT moveDownRequested(m_Index);
}

void ak::aChecklistItem::ini(void) {
	m_CheckBox->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

	m_shortcutChangeCheckedState = new QShortcut(QKeySequence("Ctrl+Space"), m_TextEdit);
	m_shortcutChangeCheckedState->setContext(Qt::ShortcutContext::WidgetShortcut);
	connect(m_shortcutChangeCheckedState, &QShortcut::activated, this, &aChecklistItem::slotToggleChecked);

	m_shortcutDeleteItem = new QShortcut(QKeySequence("Ctrl+Del"), m_TextEdit);
	m_shortcutDeleteItem->setContext(Qt::ShortcutContext::WidgetShortcut);
	connect(m_shortcutDeleteItem, &QShortcut::activated, this, &aChecklistItem::slotRequestDelete);

	m_shortcutDeleteItem2 = new QShortcut(QKeySequence("Ctrl+Backspace"), m_TextEdit);
	m_shortcutDeleteItem2->setContext(Qt::ShortcutContext::WidgetShortcut);
	connect(m_shortcutDeleteItem2, &QShortcut::activated, this, &aChecklistItem::slotRequestDelete);

	m_shortcutMoveDown = new QShortcut(QKeySequence("Ctrl+Down"), m_TextEdit);
	m_shortcutMoveDown->setContext(Qt::ShortcutContext::WidgetShortcut);
	connect(m_shortcutMoveDown, &QShortcut::activated, this, &aChecklistItem::slotMoveDownRequested);

	m_shortcutMoveUp = new QShortcut(QKeySequence("Ctrl+Up"), m_TextEdit);
	m_shortcutMoveUp->setContext(Qt::ShortcutContext::WidgetShortcut);
	connect(m_shortcutMoveUp, &QShortcut::activated, this, &aChecklistItem::slotMoveUpRequested);

	connect(m_CheckBox, &aCheckBoxWidget::stateChanged, this, &aChecklistItem::slotCheckedChanged);
	connect(m_TextEdit, &aLineEditWidget::editingFinished, this, &aChecklistItem::slotEditingFinished);
	connect(m_TextEdit, &aLineEditWidget::textChanged, this, &aChecklistItem::slotTextChanged);
	connect(m_TextEdit, &aLineEditWidget::returnPressed, this, &aChecklistItem::slotReturnPressed);
	connect(m_TextEdit, &aLineEditWidget::keyPressed, this, &aChecklistItem::slotKeyEvent);
}

// ######################################################################################################################################

// ######################################################################################################################################

// ######################################################################################################################################

ak::aChecklistWidget::aChecklistWidget() : aWidget(otNone), m_crossOutCheckedItems(false), m_pointSize(8) {
	// Create layout
	m_centralLayoutW = new QWidget;
	m_centralLayout = new QGridLayout(m_centralLayoutW);

	m_centralLayout->setRowStretch(0, 1);

	addItem(new aChecklistItem);
}

ak::aChecklistWidget::~aChecklistWidget() {

}

// ################################################################################

// Base class functions

QWidget * ak::aChecklistWidget::widget(void) {
	return m_centralLayoutW;
}

// ################################################################################

void ak::aChecklistWidget::setCrossoutCheckedItemsEnabled(bool _enabled) {
	m_crossOutCheckedItems = _enabled;
	for (auto itm : m_items) itm->refreshAppearance();
}

void ak::aChecklistWidget::setPointSize(int _pointSize) {
	m_pointSize = _pointSize;
	for (auto itm : m_items) {
		QFont f = itm->TextEdit()->font();
		f.setPointSize(m_pointSize);
		itm->TextEdit()->setFont(f);
	}
}

void ak::aChecklistWidget::refreshAppearance(void) {
	for (auto itm : m_items) itm->refreshAppearance();
}

void ak::aChecklistWidget::addItem(const QString& _text, bool _isChecked) {
	aChecklistItem * newItem = new aChecklistItem(_text, _isChecked, this);
	addItem(newItem);
}

ak::aChecklistItem * ak::aChecklistWidget::item(int _index) {
	return m_items[_index];
}

void ak::aChecklistWidget::clear(bool _addEmptyItem) {
	for (auto itm : m_items) {
		m_centralLayout->removeWidget(itm->CheckBox());
		m_centralLayout->removeWidget(itm->TextEdit());
		delete itm;
	}
	m_items.clear();

	if (_addEmptyItem) addItem(new aChecklistItem);
}

// ################################################################################

// Private slots

void ak::aChecklistWidget::slotItemChanged(int _index) {
	Q_EMIT itemChanged(_index);
}

void ak::aChecklistWidget::slotReturnPressed(int _index) {
	int r = m_items.size();
	m_centralLayout->setRowStretch(r, 0);

	int ix = _index + 1;
	for (int i = ix; i < r; i++) {
		m_centralLayout->removeWidget(m_items[i]->CheckBox());
		m_centralLayout->removeWidget(m_items[i]->TextEdit());

		m_items[i]->setIndex(i + 1);

		m_centralLayout->addWidget(m_items[i]->CheckBox(), i + 1, 0, Qt::AlignTop);
		m_centralLayout->addWidget(m_items[i]->TextEdit(), i + 1, 1, Qt::AlignTop);
	}

	aChecklistItem * newItem = new aChecklistItem;
	initializeNewItem(newItem, ix);

	newItem->TextEdit()->setFocus(Qt::FocusReason::NoFocusReason);
}

void ak::aChecklistWidget::slotFocusNext(int _index) {
	if (m_items.empty()) return;
	if (m_items[_index]->checkTextModified()) slotItemChanged(_index);
	clearEmptyItems();
	if (++_index >= m_items.size()) _index = 0;
	m_items[_index]->TextEdit()->setFocus();
}

void ak::aChecklistWidget::slotFocusPrev(int _index) {
	if (m_items.empty()) return;
	if (m_items[_index]->checkTextModified()) slotItemChanged(_index);
	clearEmptyItems();
	if (--_index < 0) _index = m_items.size() - 1;
	m_items[_index]->TextEdit()->setFocus();
}

void ak::aChecklistWidget::slotDeleteItem(int _index) {
	m_centralLayout->removeWidget(m_items[_index]->CheckBox());
	m_centralLayout->removeWidget(m_items[_index]->TextEdit());
	delete m_items[_index];
	m_items.erase(m_items.begin() + _index);

	for (int i = _index; i < m_items.size(); i++) {
		m_centralLayout->removeWidget(m_items[i]->CheckBox());
		m_centralLayout->removeWidget(m_items[i]->TextEdit());

		m_items[i]->setIndex(i);

		m_centralLayout->addWidget(m_items[i]->CheckBox(), i, 0, Qt::AlignTop);
		m_centralLayout->addWidget(m_items[i]->TextEdit(), i, 1, Qt::AlignTop);
	}

	if (m_items.size() == 0) addItem("", false);

	if (_index >= m_items.size()) m_items[0]->TextEdit()->setFocus();
	else m_items[_index]->TextEdit()->setFocus();

	Q_EMIT itemRemoved(_index);
}

void ak::aChecklistWidget::slotMoveItemUp(int _index) {
	int index2 = _index - 1;
	if (index2 < 0) moveItemToTheBottom(_index);
	else switchItemLocations(_index, index2);
	if (m_items.size() > 1) Q_EMIT itemOrderChanged();
}

void ak::aChecklistWidget::slotMoveItemDown(int _index) {
	int index2 = _index + 1;
	if (index2 >= m_items.size()) moveItemToTheTop(_index);
	else switchItemLocations(_index, index2);
	if (m_items.size() > 1) Q_EMIT itemOrderChanged();
}

// ################################################################################

// Private functions

void ak::aChecklistWidget::clearEmptyItems(void) {
	bool erased = true;
	while (erased) {
		erased = false;
		for (int i = 0; i < m_items.size() && m_items.size() > 1; i++) {
			if (m_items[i]->text().isEmpty()) {
				// Delete empty item
				m_centralLayout->removeWidget(m_items[i]->TextEdit());
				m_centralLayout->removeWidget(m_items[i]->CheckBox());
				delete m_items[i];

				// Move all items after the empty item up by 1
				for (int j = i + 1; j < m_items.size(); j++) {
					m_centralLayout->removeWidget(m_items[j]->TextEdit());
					m_centralLayout->removeWidget(m_items[j]->CheckBox());

					m_items[j]->setIndex(j - 1);

					m_centralLayout->addWidget(m_items[j]->CheckBox(), j - 1, 0);
					m_centralLayout->addWidget(m_items[j]->TextEdit(), j - 1, 1);
				}
				m_items.erase(m_items.begin() + i);
				erased = true;
				Q_EMIT itemRemoved(i);
				break;
			}
		}
	}
}

void ak::aChecklistWidget::addItem(aChecklistItem * _item) {
	initializeNewItem(_item, m_items.size());
}

void ak::aChecklistWidget::initializeNewItem(aChecklistItem * _item, int _index) {
	int r = m_items.size();
	m_centralLayout->setRowStretch(r, 0);
	m_centralLayout->setRowStretch(r + 1, 1);

	_item->setChecklistWidget(this);
	_item->setIndex(_index);

	m_centralLayout->addWidget(_item->CheckBox(), _index, 0, Qt::AlignTop);
	m_centralLayout->addWidget(_item->TextEdit(), _index, 1, Qt::AlignTop);

	m_items.insert(m_items.begin() + _index, _item);

	_item->refreshAppearance();

	connect(_item, &aChecklistItem::editingFinished, this, &aChecklistWidget::slotItemChanged, Qt::QueuedConnection);
	connect(_item, &aChecklistItem::textChanged, this, &aChecklistWidget::slotItemChanged, Qt::QueuedConnection);
	connect(_item, &aChecklistItem::returnPressed, this, &aChecklistWidget::slotReturnPressed, Qt::QueuedConnection);
	connect(_item, &aChecklistItem::keyUpPressed, this, &aChecklistWidget::slotFocusPrev, Qt::QueuedConnection);
	connect(_item, &aChecklistItem::keyDownPressed, this, &aChecklistWidget::slotFocusNext, Qt::QueuedConnection);
	connect(_item, &aChecklistItem::requestDelete, this, &aChecklistWidget::slotDeleteItem, Qt::QueuedConnection);
	connect(_item, &aChecklistItem::moveDownRequested, this, &aChecklistWidget::slotMoveItemDown, Qt::QueuedConnection);
	connect(_item, &aChecklistItem::moveUpRequested, this, &aChecklistWidget::slotMoveItemUp, Qt::QueuedConnection);

	Q_EMIT itemCreated(_index);
}

void ak::aChecklistWidget::switchItemLocations(int _index1, int _index2) {
	if (m_items.size() < 2 || _index1 == _index2) return;

	auto itm1 = m_items[_index1];
	auto itm2 = m_items[_index2];
	itm1->setIndex(_index2);
	itm2->setIndex(_index1);

	if (_index1 < _index2) {
		m_items.erase(m_items.begin() + _index2);
		m_items.erase(m_items.begin() + _index1);

		m_items.insert(m_items.begin() + _index1, itm2);
		m_items.insert(m_items.begin() + _index2, itm1);
	}
	else {
		m_items.erase(m_items.begin() + _index1);
		m_items.erase(m_items.begin() + _index2);
		
		m_items.insert(m_items.begin() + _index2, itm1);
		m_items.insert(m_items.begin() + _index1, itm2);
	}

	m_centralLayout->removeWidget(itm1->CheckBox());
	m_centralLayout->removeWidget(itm1->TextEdit());

	m_centralLayout->removeWidget(itm2->CheckBox());
	m_centralLayout->removeWidget(itm2->TextEdit());

	m_centralLayout->addWidget(itm1->CheckBox(), _index2, 0);
	m_centralLayout->addWidget(itm1->TextEdit(), _index2, 1);

	m_centralLayout->addWidget(itm2->CheckBox(), _index1, 0);
	m_centralLayout->addWidget(itm2->TextEdit(), _index1, 1);
}

void ak::aChecklistWidget::moveItemToTheTop(int _index) {
	if (m_items.size() < 2 || _index == 0) return;

	auto itm = m_items[_index];
	m_items.erase(m_items.begin() + _index);

	m_items.insert(m_items.begin(), itm);

	for (auto i : m_items) {
		m_centralLayout->removeWidget(i->CheckBox());
		m_centralLayout->removeWidget(i->TextEdit());
	}

	for (int i = 0; i < m_items.size(); i++) {
		m_items[i]->setIndex(i);
		m_centralLayout->addWidget(m_items[i]->CheckBox(), i, 0);
		m_centralLayout->addWidget(m_items[i]->TextEdit(), i, 1);
	}
}

void ak::aChecklistWidget::moveItemToTheBottom(int _index) {
	if (m_items.size() < 2 || _index == (m_items.size() - 1)) return;

	auto itm = m_items[_index];
	m_items.erase(m_items.begin() + _index);

	m_items.push_back(itm);

	for (auto i : m_items) {
		m_centralLayout->removeWidget(i->CheckBox());
		m_centralLayout->removeWidget(i->TextEdit());
	}

	for (int i = 0; i < m_items.size(); i++) {
		m_items[i]->setIndex(i);
		m_centralLayout->addWidget(m_items[i]->CheckBox(), i, 0);
		m_centralLayout->addWidget(m_items[i]->TextEdit(), i, 1);
	}
}
