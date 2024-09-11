//! @file VersionGraphManagerView.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/VersionGraphManager.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT VersionGraphManagerView : public VersionGraphManager, public WidgetView {
	public:
		VersionGraphManagerView();
		virtual ~VersionGraphManagerView();

		virtual QWidget* getViewWidget(void) override;

	private:

	};

}