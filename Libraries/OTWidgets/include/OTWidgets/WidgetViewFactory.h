//! @file WidgetViewFactory.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// std header
#include <string>

namespace ot {

	class WidgetView;
	class WidgetViewCfg;

	class WidgetViewFactory {
		OT_DECL_NOCOPY(WidgetViewFactory)
	public:
		static WidgetView* createView(WidgetViewCfg* _viewConfiguration);

		static WidgetView* createView(const std::string& _viewType);

	private:
		WidgetViewFactory() {};
		~WidgetViewFactory() {};

	};

}