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

	class WidgetView;
	class WidgetViewTab;

	class OT_WIDGETS_API_EXPORT WidgetViewDock : public ads::CDockWidget {
		Q_OBJECT
		OT_DECL_NOCOPY(WidgetViewDock)
		OT_DECL_NODEFAULT(WidgetViewDock)
	public:
		WidgetViewDock(WidgetView* _view);
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