// @otlicense
// File: TreeWidgetItem.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTWidgets/TreeWidgetItem.h"


ot::TreeWidgetItem::TreeWidgetItem(int _type)
	: QTreeWidgetItem(_type), m_flags(NavigationTreeItem::NoItemFlags)
{
	
}

ot::TreeWidgetItem::TreeWidgetItem(const TreeWidgetItemInfo& _item, int _type)
	: QTreeWidgetItem(_type), m_flags(_item.getFlags())
{
	this->setText(0, _item.getText());
	this->setIcon(0, _item.getIcon());

	for (const TreeWidgetItemInfo& c : _item.getChildItems()) {
		this->addChild(new TreeWidgetItem(c));
	}
}

ot::TreeWidgetItem::~TreeWidgetItem() {}

ot::TreeWidgetItemInfo ot::TreeWidgetItem::getFullInfo(void) const {
	ot::TreeWidgetItemInfo info;
	info.setText(this->text(0));
	info.setIcon(this->icon(0));
	info.setFlags(this->getNavigationItemFlags());

	const QTreeWidgetItem* itm = this->parent();

	while (itm) {
		// Mark info as child
		ot::TreeWidgetItemInfo child = info;

		info.clearChildItems();

		info.setText(itm->text(0));
		info.setIcon(itm->icon(0));
		const ot::TreeWidgetItem* castItem = dynamic_cast<const ot::TreeWidgetItem*>(itm);
		if (castItem) {
			info.setFlags(castItem->getNavigationItemFlags());
		}
		else {
			info.setFlags(NavigationTreeItem::NoItemFlags);
		}

		info.addChildItem(child);
		itm = itm->parent();
	}

	return info;
}

bool ot::TreeWidgetItem::isChildOf(const QTreeWidgetItem* _parent) const {
	const QTreeWidgetItem* parentItem = this->parent();
	while (parentItem) {
		if (parentItem == _parent) {
			return true;
		}
		parentItem = parentItem->parent();
	}
	return false;
}

bool ot::TreeWidgetItem::hasChild(const QTreeWidgetItem* _child) const {
	return hasChildRecursive(this, _child);
}

bool ot::TreeWidgetItem::hasChild(const QString& _childPath, char _delimiter, int _nameColumn) const {
	return hasChildRecursive(this, _childPath.split(_delimiter, Qt::SkipEmptyParts), _nameColumn);
}

bool ot::TreeWidgetItem::hasDirectChild(const QTreeWidgetItem* _child) const {
	for (int i = 0; i < this->childCount(); ++i) {
		if (this->child(i) == _child) {
			return true;
		}
	}
}

bool ot::TreeWidgetItem::hasDirectChild(const QString& _childText, int _nameColumn) const {
	for (int i = 0; i < this->childCount(); ++i) {
		if (this->child(i)->text(_nameColumn) == _childText) {
			return true;
		}
	}
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

bool ot::TreeWidgetItem::hasChildRecursive(const QTreeWidgetItem* _item, const QTreeWidgetItem* _child) const {
	for (int i = 0; i < _item->childCount(); ++i) {
		QTreeWidgetItem* c = _item->child(i);
		if (c == _child) {
			return true;
		}
		if (hasChildRecursive(c, _child)) {
			return true;
		}
	}
	return false;
}

bool ot::TreeWidgetItem::hasChildRecursive(const QTreeWidgetItem* _item, const QStringList& _childPath, int _nameColumn) const {
	if (_childPath.isEmpty()) {
		return false;
	}
	
	QStringList remainingPath = _childPath;
	QString currentLevelText = remainingPath.takeFirst();
	for (int i = 0; i < _item->childCount(); ++i) {
		const QTreeWidgetItem* child = _item->child(i);
		if (child->text(_nameColumn) == currentLevelText) {
			if (remainingPath.isEmpty()) {
				return true;
			}
			else {
				return hasChildRecursive(child, remainingPath, _nameColumn);
			}
		}
	}
	return false;
}
