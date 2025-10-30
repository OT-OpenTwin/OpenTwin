// @otlicense

// OpenTwin header
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/TreeWidgetItem.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/TreeItemDelegate.h"

// Qt header
#include <QtGui/qevent.h>

ot::TreeWidget::TreeWidget(QWidget * _parentWidget) 
	: QTreeWidget(_parentWidget) 
{
	this->setObjectName("OT_Tree");

	m_itemDeletegate = new TreeItemDelegate(this);

	this->connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleAboutToChange, this, &TreeWidget::slotColorStyleAboutToChange);
	this->connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &TreeWidget::slotColorStyleChanged);
}

ot::TreeWidget::~TreeWidget() {
	this->blockSignals(true);
	
	delete m_itemDeletegate;
	m_itemDeletegate = nullptr;

	std::list<QTreeWidgetItem*> items;
	for (int i = 0; i < this->topLevelItemCount(); i++) items.push_back(this->topLevelItem(i));
	for (QTreeWidgetItem* item : items) this->destroyRecursive(item);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

QTreeWidgetItem* ot::TreeWidget::findItem(const QString& _itemPath, char _delimiter) const {
	QStringList lst = _itemPath.split(_delimiter, Qt::SkipEmptyParts);
	return this->findItem(this->invisibleRootItem(), lst);
}

bool ot::TreeWidget::itemTextExists(const QString& _itemText) const {
	for (int i = 0; i < this->topLevelItemCount(); i++) {
		if (this->topLevelItem(i)->text(0) == _itemText) return true;
		if (this->itemTextExists(this->topLevelItem(i), _itemText)) return true;
	}
	return false;
}

QString ot::TreeWidget::getItemPath(QTreeWidgetItem* _item, char _delimiter) const {
	QString pth;
	QTreeWidgetItem* itm = _item;
	while (itm && itm != this->invisibleRootItem()) {
		if (pth.isEmpty()) {
			pth = itm->text(0);
		}
		else {
			pth = itm->text(0) + QString(QChar(_delimiter)) + pth;
		}
		itm = itm->parent();
	}
	return pth;
}

QTreeWidgetItem* ot::TreeWidget::addItem(const TreeWidgetItemInfo& _item) {
	return this->addItem(this->invisibleRootItem(), _item);
}

void ot::TreeWidget::deselectAll(void) {
	this->blockSignals(true);
	QList<QTreeWidgetItem*> selection = this->selectedItems();
	for (QTreeWidgetItem* itm : selection) {
		itm->setSelected(false);
	}
	this->blockSignals(false);
	Q_EMIT itemSelectionChanged();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Event handler

void ot::TreeWidget::mousePressEvent(QMouseEvent* _event) {
	QTreeWidget::mousePressEvent(_event);
}

void ot::TreeWidget::drawRow(QPainter* _painter, const QStyleOptionViewItem& _options, const QModelIndex& _index) const {
	QTreeWidget::drawRow(_painter, _options, _index);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Helper

bool ot::TreeWidget::itemTextExists(QTreeWidgetItem* _parent, const QString& _itemText) const {
	OTAssertNullptr(_parent);
	for (int i = 0; i < _parent->childCount(); i++) {
		if (_parent->child(i)->text(0) == _itemText) return true;
		if (this->itemTextExists(_parent->child(i), _itemText)) return true;
	}
	return false;
}

QTreeWidgetItem* ot::TreeWidget::findItem(QTreeWidgetItem* _item, const QStringList& _childPath) const {
	if (_childPath.isEmpty()) return nullptr;

	for (int i = 0; i < _item->childCount(); i++) {
		if (_item->child(i)->text(0) == _childPath.front()) {
			if (_childPath.count() == 1)
			{
				return _item->child(i);
			}
			else {
				QStringList lst = _childPath;
				lst.pop_front();
				return this->findItem(_item->child(i), lst);
			}
		}
	}
	return nullptr;
}

QTreeWidgetItem* ot::TreeWidget::findItemByText(QTreeWidgetItem* _parent, const QString& _itemText) const {
	OTAssertNullptr(_parent);
	QTreeWidgetItem* ret = nullptr;
	for (int i = 0; i < _parent->childCount(); i++) {
		if (_parent->child(i)->text(0) == _itemText) return _parent->child(i);
		ret = this->findItemByText(_parent->child(i), _itemText);
		if (ret) return ret;
	}
	return ret;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ot::TreeWidget::slotColorStyleAboutToChange(void) {
	m_columnWidths.clear();
	for (int i = 0; i < this->columnCount(); i++) {
		m_columnWidths.push_back(this->columnWidth(i));
	}
	if (!m_columnWidths.empty()) {
		m_columnWidths.pop_back();
	}
}

void ot::TreeWidget::slotColorStyleChanged(void) {
	int i = 0;
	for (int w : m_columnWidths) {
		this->setColumnWidth(i++, w);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

QTreeWidgetItem* ot::TreeWidget::addItem(QTreeWidgetItem* _parent, const TreeWidgetItemInfo& _item) {
	for (int i = 0; i < _parent->childCount(); i++) {
		if (_parent->child(i)->text(0) == _item.getText()) {
			for (const TreeWidgetItemInfo& child : _item.getChildItems()) {
				this->addItem(_parent->child(i), child);
			}
			return _parent->child(i);
		}
	}

	TreeWidgetItem* newItem = new TreeWidgetItem(_item);
	_parent->addChild(newItem);
	return newItem;
}

void ot::TreeWidget::destroyRecursive(QTreeWidgetItem* _item) {
	std::list<QTreeWidgetItem*> items;
	for (int i = 0; i < _item->childCount(); i++) items.push_back(_item->child(i));
	for (QTreeWidgetItem* item : items) this->destroyRecursive(item);
	delete _item;
}