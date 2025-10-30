// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"

namespace ot {

	class VersionGraphManager;

	class OT_WIDGETS_API_EXPORT VersionGraphManagerView : public WidgetView {
	public:
		VersionGraphManagerView(VersionGraphManager* _versionGraphManager = (VersionGraphManager*)nullptr);
		virtual ~VersionGraphManagerView();

		virtual QWidget* getViewWidget(void) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		VersionGraphManager* getVersionGraphManager(void) const { return m_versionGraphManager; };

	private:
		VersionGraphManager* m_versionGraphManager;

	};

}