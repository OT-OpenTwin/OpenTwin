// @otlicense
// File: WidgetViewDock.h
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
#include "OTWidgets/WidgetTypes.h"

// ads header
#include <ads/DockWidget.h>

namespace ot {

	class WidgetView;
	class WidgetViewTab;

	class OT_WIDGETS_API_EXPORT WidgetViewDock : public ads::CDockWidget {
		Q_OBJECT
		OT_DECL_NOCOPY(WidgetViewDock)
		OT_DECL_NOMOVE(WidgetViewDock)
		OT_DECL_NODEFAULT(WidgetViewDock)
	public:
		explicit WidgetViewDock(WidgetView* _view, QWidget* _parent);
		virtual ~WidgetViewDock();

		void openView();
		void closeView();
		
		void setCloseButtonVisible(bool _vis);
		void setPinButtonVisible(bool _vis);

		WidgetView* getWidgetView() const { return m_view; };

		void setIsPinned(bool _isPinned);
		bool getIsPinned() const;

		WidgetViewTab* getWidgetViewTab() const { return m_tab; };

	Q_SIGNALS:
		void dockResized(const QSize& _newSize);
		void dockCloseRequested(void);
		void dockPinnedChanged(bool _isPinned);

	protected:
		virtual void resizeEvent(QResizeEvent* _event) override;

	private Q_SLOTS:
		void slotCloseRequested(void);
		void slotPinnedChanged(bool _isPinned);

	private:
		WidgetView* m_view;
		WidgetViewTab* m_tab;
	};

}