//! @file TreeWidgetItem.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/TreeWidgetItemInfo.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

namespace ot {
	class OT_WIDGETS_API_EXPORT TreeWidgetItem : public QTreeWidgetItem {
	public:
		TreeWidgetItem(int _type = 0);
		TreeWidgetItem(const TreeWidgetItemInfo& _itemInfo, int _type = 0);
		virtual ~TreeWidgetItem();

		void setNavigationItemFlag(NavigationTreeItemFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setNavigationItemFlags(NavigationTreeItemFlags _flags) { m_flags = _flags; };
		NavigationTreeItemFlags navigationItemFlags(void) const { return m_flags; };

		//! @brief Returns the item info
		//! The information returned only contains the path to this item (other childs of parent items are ignored)
		TreeWidgetItemInfo getFullInfo(void) const;

		std::string getTreeWidgetItemPath(char _delimiter = '/', int _nameColumn = 0) const;

		void expandAllParents(bool _expandThis = false);

	private:
		NavigationTreeItemFlags m_flags;
	};
}