//! @file WidgetViewTab.h
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt ADS header
#include <ads/DockWidgetTab.h>

namespace ads { class CDockWidget; }

namespace ot {

	class ToolButton;

	class OT_WIDGETS_API_EXPORT WidgetViewTab : public ads::CDockWidgetTab {
		Q_OBJECT
		OT_DECL_NOCOPY(WidgetViewTab)
		OT_DECL_NODEFAULT(WidgetViewTab)
	public:
		WidgetViewTab(ads::CDockWidget* _dockWidget);
		virtual ~WidgetViewTab();

		void setIsPinned(bool _pinned);
		bool getIsPinned(void) const { return m_isPinned; };

		void setCloseButtonVisible(bool _vis);
		void setPinButtonVisible(bool _vis);

	Q_SIGNALS:
		void viewCloseRequested(void);
		void viewPinnedChanged(bool _isPinned);

	private Q_SLOTS:
		void slotClose(void);
		void slotTogglePinned(void);

	private:
		bool m_isPinned;

		ToolButton* m_closeButton;
		ToolButton* m_pinButton;
		
	};

}