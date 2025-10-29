// @otlicense

//! @file DockWatcherAction.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qaction.h>
#include <QtWidgets/qdockwidget.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT DockWatcherAction : public QAction {
		Q_OBJECT
		OT_DECL_NOCOPY(DockWatcherAction)
	public:
		DockWatcherAction();
		virtual ~DockWatcherAction();

		void watch(QDockWidget* _dock);
		void removeWatch(void);

	private Q_SLOTS:
		void slotTriggered(bool _checked);
		void slotVisibilityChanged(bool _vis);

	private:
		QDockWidget* m_dock;

	};

}
