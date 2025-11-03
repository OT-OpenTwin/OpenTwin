// @otlicense
// File: TreeItemViewLink.h
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
#include "OTSystem/Flags.h"
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
			CentralViewsOnly    = 1 << 4, //! @brief If set only central views will be concidered.

			DefaultFlags = OpenOnSelect | DeselectOnFocusLost | SelectOnFocus | CentralViewsOnly
		};
		typedef ot::Flags<LinkFlag> LinkFlags;

		TreeItemViewLink(QTreeWidgetItem* _treeItem, WidgetView* _view, const LinkFlags& _flags = LinkFlag::DefaultFlags);
		virtual ~TreeItemViewLink();

		void setLinkFlag(LinkFlag _flag, bool _active = true) { m_flags.set(_flag, _active); };
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

OT_ADD_FLAG_FUNCTIONS(ot::TreeItemViewLink::LinkFlag, ot::TreeItemViewLink::LinkFlags)