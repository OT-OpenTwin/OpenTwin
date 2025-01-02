//! @file CentralWidgetManager.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/DelayedCallHandler.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT DelayedShowHideHandler : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(DelayedShowHideHandler)
	public:
		DelayedShowHideHandler();
		~DelayedShowHideHandler();

		void show(void);
		void setShowDelay(int _delayInMs) { m_showHandler.setDelay(_delayInMs); };
		int getShowDelay(void) const { return m_showHandler.getDelay(); };

		void hide(void);
		void setHideDelay(int _delayInMs) { m_hideHandler.setDelay(_delayInMs); };
		int getHideDelay(void) const { return m_hideHandler.getDelay(); };

		void stop(void);

	Q_SIGNALS:
		void showRequest(void);
		void hideRequest(void);

	private Q_SLOTS:
		void slotShow(void);
		void slotHide(void);

	private:
		DelayedCallHandler m_showHandler;
		DelayedCallHandler m_hideHandler;
	};

}