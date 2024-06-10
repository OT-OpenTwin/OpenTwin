//! @file TreeWidget.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/NavigationTreeItem.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtGui/qicon.h>

namespace ot {
	class OT_WIDGETS_API_EXPORT TreeWidgetItemInfo {
	public:
		TreeWidgetItemInfo();
		TreeWidgetItemInfo(const QString& _text, const QIcon& _icon = QIcon(), const NavigationTreeItemFlags& _flags = NavigationTreeItemFlags());
		TreeWidgetItemInfo(const NavigationTreeItem& _config);
		TreeWidgetItemInfo(const TreeWidgetItemInfo& _other);
		virtual ~TreeWidgetItemInfo();

		TreeWidgetItemInfo& operator = (const TreeWidgetItemInfo& _other);

		void setText(const QString& _text) { m_text = _text; };
		const QString& text(void) const { return m_text; };

		void setIcon(const QIcon& _icon) { m_icon = _icon; };
		const QIcon& icon(void) const { return m_icon; };

		void setFlag(NavigationTreeItemFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setFlags(const NavigationTreeItemFlags _flags) { m_flags = _flags; };
		NavigationTreeItemFlags flags(void) const { return m_flags; };

		ot::TreeWidgetItemInfo& addChildItem(const QString& _text, const QIcon& _icon = QIcon(), const NavigationTreeItemFlags& _flags = NavigationTreeItemFlags());
		void addChildItem(const TreeWidgetItemInfo& _info);
		void setChildItems(const std::list<TreeWidgetItemInfo>& _childs) { m_childs = _childs; };
		const std::list<TreeWidgetItemInfo>& childItems(void) const { return m_childs; };
		void clearChildItems(void);

	private:
		QString m_text;
		QIcon m_icon;
		NavigationTreeItemFlags m_flags;
		std::list<TreeWidgetItemInfo> m_childs;

	};
}