// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetViewManager.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT GlobalWidgetViewManager : public WidgetViewManager {
		OT_DECL_NOCOPY(GlobalWidgetViewManager)
		OT_DECL_NOMOVE(GlobalWidgetViewManager)
	public:
		//! @brief Return the clobal instance
		static GlobalWidgetViewManager& instance();

	private:
		GlobalWidgetViewManager() = default;
		virtual ~GlobalWidgetViewManager() = default;

	};

}