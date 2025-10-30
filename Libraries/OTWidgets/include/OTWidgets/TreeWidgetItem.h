// @otlicense

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