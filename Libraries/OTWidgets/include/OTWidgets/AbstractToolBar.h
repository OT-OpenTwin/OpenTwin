//! @file AbstractToolBar.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

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