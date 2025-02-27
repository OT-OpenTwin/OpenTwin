//! @file TreeItemViewLink.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

class QTreeWidgetItem;

namespace ot {

	class WidgetView;

	class OT_WIDGETS_API_EXPORT TreeItemViewLink : public QObject {
		Q_OBJECT
	public:
		enum LinkFlag {
			NoLink              = 0 << 0,
			OpenOnSelect        = 1 << 0,
			CloseOnDeselect     = 1 << 1,
			SelectOnFocus       = 1 << 2,
			DeselectOnFocusLost = 1 << 3,

			DefaultFlags = OpenOnSelect | DeselectOnFocusLost | SelectOnFocus
		};
		typedef ot::Flags<LinkFlag> LinkFlags;

		TreeItemViewLink(QTreeWidgetItem* _treeItem, WidgetView* _view, const LinkFlags& _flags = LinkFlag::DefaultFlags);
		virtual ~TreeItemViewLink();

		void setLinkFlag(LinkFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setLinkFlags(const LinkFlags& _flags) { m_flags = _flags; };
		const LinkFlags& getLinkFlags(void) const { return m_flags; };

	private Q_SLOTS:
		void slotTreeSelectionChanged(void);
		void slotViewFocusChanged(WidgetView* _focusedView, WidgetView* _previousView);

	private:
		WidgetView* m_view;
		QTreeWidgetItem* m_treeItem;
		LinkFlags m_flags;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::TreeItemViewLink::LinkFlag)