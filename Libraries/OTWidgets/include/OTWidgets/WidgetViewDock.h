//! @file WidgetViewDock.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// ads header
#include <ads/DockWidget.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT WidgetViewDock : public ads::CDockWidget {
		Q_OBJECT
		OT_DECL_NOCOPY(WidgetViewDock)
	public:
		WidgetViewDock();

		void openView(void);
		void closeView(void);
		
		void setCloseButtonVisible(bool _vis);
		void setLockButtonVisible(bool _vis);

	Q_SIGNALS:
		void dockResized(const QSize& _newSize);
		void dockCloseRequested(void);
		void dockLockedChanged(bool _isLocked);

	protected:
		virtual void resizeEvent(QResizeEvent* _event) override;

	private Q_SLOTS:
		void slotCloseRequested(void);
		void slotLockedChanged(bool _isLocked);
	};

}