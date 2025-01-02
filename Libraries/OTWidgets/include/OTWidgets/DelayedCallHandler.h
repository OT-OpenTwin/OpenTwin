//! @file DelayedCallHandler.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qtimer.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT DelayedCallHandler : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(DelayedCallHandler)
	public:
		DelayedCallHandler(int _delayInMs = 0);
		
		void setDelay(int _delayInMs);
		int getDelay(void) const;

		void callDelayed(bool _waitUntilCalled = false);
		void stop(void);

	Q_SIGNALS:
		void timeout(void);

	private Q_SLOTS:
		void slotTimeout(void);

	private:
		QTimer m_timer;
		bool m_waitForCall;
	};

}