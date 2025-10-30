// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

class QToolBar;

namespace ot {

	class OT_WIDGETS_API_EXPORTONLY AbstractToolBar {
		OT_DECL_NOCOPY(AbstractToolBar)
	public:
		AbstractToolBar() {};
		AbstractToolBar(AbstractToolBar&&) = default;
		virtual ~AbstractToolBar() {};
		AbstractToolBar& operator = (AbstractToolBar&&) = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual QToolBar* getToolBar(void) = 0;
		virtual const QToolBar* getToolBar(void) const = 0;
	};

}