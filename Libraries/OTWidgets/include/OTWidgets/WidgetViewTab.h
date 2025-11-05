// @otlicense
// File: WidgetViewTab.h
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

// Qt ADS header
#include <ads/DockWidgetTab.h>

namespace ads { class CDockWidget; }

namespace ot {

	class ToolButton;

	class OT_WIDGETS_API_EXPORT WidgetViewTab : public ads::CDockWidgetTab {
		Q_OBJECT
		OT_DECL_NOCOPY(WidgetViewTab)
		OT_DECL_NOMOVE(WidgetViewTab)
		OT_DECL_NODEFAULT(WidgetViewTab)
	public:
		explicit WidgetViewTab(ads::CDockWidget* _dockWidget, QWidget* _parent);
		virtual ~WidgetViewTab();

		void setIsPinned(bool _pinned);
		bool getIsPinned() const { return m_isPinned; };

		void setCloseButtonVisible(bool _vis);
		void setPinButtonVisible(bool _vis);

		void disableButtons();

	Q_SIGNALS:
		void viewCloseRequested();
		void viewPinnedChanged(bool _isPinned);

	private Q_SLOTS:
		void slotClose();
		void slotTogglePinned();

	protected:
		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QMouseEvent* _event) override;

	private:
		bool m_isPinned;
		bool m_isMiddleButtonPressed;

		ToolButton* m_closeButton;
		ToolButton* m_pinButton;
		
	};

}