// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/AbstractStatusBar.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT StatusBarManager : public AbstractStatusBar {
		OT_DECL_NOCOPY(StatusBarManager)
	public:
		StatusBarManager();
		virtual ~StatusBarManager();

		virtual QStatusBar* getStatusBar(void) override;
		virtual const QStatusBar* getStatusBar(void) const override;

	private:
		QStatusBar* m_statusBar;
	};

}