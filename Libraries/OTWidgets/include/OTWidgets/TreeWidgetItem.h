// @otlicense
// File: TreeWidgetItem.h
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

#pragma once

// OpenTwin header
#include "OTWidgets/TreeWidgetItemInfo.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

namespace ot {
	class OT_WIDGETS_API_EXPORT TreeWidgetItem : public QTreeWidgetItem {
	public:
		TreeWidgetItem(int _type = 0);
		TreeWidgetItem(const TreeWidgetItemInfo& _itemInfo, int _type = 0);
		virtual ~TreeWidgetItem();

		void setNavigationItemFlag(NavigationTreeItem::ItemFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setNavigationItemFlags(const NavigationTreeItem::ItemFlags& _flags) { m_flags = _flags; };
		const NavigationTreeItem::ItemFlags getNavigationItemFlags() const { return m_flags; };

		//! @brief Returns the item info
		//! The information returned only contains the path to this item (other childs of parent items are ignored)
		TreeWidgetItemInfo getFullInfo() const;

		QString getTreeWidgetItemPath(char _delimiter = '/', int _nameColumn = 0) const;

		void expandAllParents(bool _expandThis = false);

	private:
		NavigationTreeItem::ItemFlags m_flags;
	};
}