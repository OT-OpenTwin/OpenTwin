//! @file AbstractStatusBar.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

class QStatusBar;

namespace ot {

	class OT_WIDGETS_API_EXPORTONLY AbstractStatusBar {
		OT_DECL_NOCOPY(AbstractStatusBar)
	public:
		AbstractStatusBar() {};
		AbstractStatusBar(AbstractStatusBar&&) = default;
		virtual ~AbstractStatusBar() {};
		AbstractStatusBar& operator = (AbstractStatusBar&&) = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual QStatusBar* getStatusBar(void) = 0;
		virtual const QStatusBar* getStatusBar(void) const = 0;
	};

}