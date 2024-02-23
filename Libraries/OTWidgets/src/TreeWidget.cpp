//! @file TreeWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/IconManager.h"

ot::TreeWidgetItemInfo::TreeWidgetItemInfo() : m_flags(ot::NoNavigationTreeItemFlags) {}

ot::TreeWidgetItemInfo::TreeWidgetItemInfo(const NavigationTreeItem& _config)
	: m_text(QString::fromStdString(_config.text())), m_flags(_config.flags())
{
	if (!_config.iconPath().empty()) {
		m_icon = IconManager::instance().getIcon(QString::fromStdString(_config.iconPath()));
	}
	
	for (const NavigationTreeItem& child : _config.childItems()) {
		m_childs.push_back(TreeWidgetItemInfo(child));
	}
}

ot::TreeWidgetItemInfo::TreeWidgetItemInfo(const TreeWidgetItemInfo& _other) 
	: m_text(_other.m_text), m_icon(_other.m_icon), m_flags(_other.m_flags), m_childs(_other.m_childs)
{}

ot::TreeWidgetItemInfo::~TreeWidgetItemInfo() {

}

ot::TreeWidgetItemInfo& ot::TreeWidgetItemInfo::operator = (const TreeWidgetItemInfo& _other) {
	m_text = _other.m_text;
	m_icon = _other.m_icon;
	m_flags = _other.m_flags;
	m_childs = _other.m_childs;
	return *this;
}

void ot::TreeWidgetItemInfo::addChildItem(const TreeWidgetItemInfo& _info) {
	m_childs.push_back(_info);
}

void ot::TreeWidgetItemInfo::clearChildItems(void) {
	m_childs.clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::TreeWidget::TreeWidget(QWidget * _parentWidget) : QTreeWidget(_parentWidget) {

}

ot::TreeWidget::~TreeWidget() {

}

QTreeWidgetItem* ot::TreeWidget::findItem(const QString& _itemPath, char _delimiter) const {
	QStringList lst = _itemPath.split(_delimiter, Qt::SkipEmptyParts);
	return this->findItem(this->invisibleRootItem(), lst);
}

QString ot::TreeWidget::itemPath(QTreeWidgetItem* _item, char _delimiter) const {
	QString pth;
	QTreeWidgetItem* itm = _item;
	while (itm && itm != this->invisibleRootItem()) {
		if (pth.isEmpty()) {
			pth = itm->text(0);
		}
		else {
			pth = itm->text(0) + _delimiter + pth;
		}
		itm = itm->parent();
	}
	return pth;
}

QTreeWidgetItem* ot::TreeWidget::addItem(const TreeWidgetItemInfo& _item) {
	return this->addItem(this->invisibleRootItem(), _item);
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

QTreeWidgetItem* ot::TreeWidget::addItem(QTreeWidgetItem* _parent, const TreeWidgetItemInfo& _item) {
	for (int i = 0; i < _parent->childCount(); i++) {
		if (_parent->child(i)->text(0) == _item.text()) {
			for (const TreeWidgetItemInfo& child : _item.childItems()) {
				this->addItem(_parent->child(i), child);
			}
			return _parent->child(i);
		}
	}

	TreeWidgetItem* newItem = new TreeWidgetItem(_item);
	_parent->addChild(newItem);
	return newItem;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::TreeWidgetItem::TreeWidgetItem(int _type)
	: QTreeWidgetItem(_type), m_flags(ot::NoNavigationTreeItemFlags)
{

}

ot::TreeWidgetItem::TreeWidgetItem(const TreeWidgetItemInfo& _item, int _type)
	: QTreeWidgetItem(_type), m_flags(_item.flags())
{
	this->setText(0, _item.text());
	this->setIcon(0, _item.icon());

	for (const TreeWidgetItemInfo& c : _item.childItems()) {
		this->addChild(new TreeWidgetItem(c));
	}
}

ot::TreeWidgetItem::~TreeWidgetItem() {}

ot::TreeWidgetItemInfo ot::TreeWidgetItem::getFullInfo(void) const {
	ot::TreeWidgetItemInfo info;
	info.setText(this->text(0));
	info.setIcon(this->icon(0));
	info.setFlags(this->navigationItemFlags());

	const QTreeWidgetItem* itm = this->parent();

	while (itm) {
		// Mark info as child
		ot::TreeWidgetItemInfo child = info;

		info.clearChildItems();

		info.setText(itm->text(0));
		info.setIcon(itm->icon(0));
		const ot::TreeWidgetItem* castItem = dynamic_cast<const ot::TreeWidgetItem*>(itm);
		if (castItem) {
			info.setFlags(castItem->navigationItemFlags());
		}
		else {
			info.setFlags(ot::NoNavigationTreeItemFlags);
		}

		info.addChildItem(child);
		itm = itm->parent();
	}

	return info;
}

void ot::TreeWidgetItem::expandAllParents(bool _expandThis) {
	if (_expandThis) { this->setExpanded(true); }
	QTreeWidgetItem* itm = parent();
	while (itm) {
		itm->setExpanded(true);
		itm = itm->parent();
	}
}