//! @file TreeWidgetItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/TreeWidgetItem.h"


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

QString ot::TreeWidgetItem::getTreeWidgetItemPath(char _delimiter, int _nameColumn) const {
	QString path;
	const QTreeWidgetItem* itm = this;
	while (itm) {
		if (path.isEmpty()) {
			path = itm->text(_nameColumn);
		}
		else {
			path = itm->text(_nameColumn) % _delimiter % path;
		}

		itm = itm->parent();
	}
	return path;
}