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

		void setLocked(bool _locked) { m_isLocked = _locked; };
		bool isLocked(void) const { return m_isLocked; };

		void setCloseButtonVisible(bool _vis);
		void setLockButtonVisible(bool _vis);

	Q_SIGNALS:
		void viewCloseRequested(void);
		void viewLockedChanged(bool _isLocked);

	private Q_SLOTS:
		void slotClose(void);
		void slotToggleLocked(void);

	private:
		bool m_isLocked;

		ToolButton* m_closeButton;
		ToolButton* m_lockButton;
		
	};

}