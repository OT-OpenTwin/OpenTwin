//! @file TabToolBarManager.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/AbstractToolBar.h"

namespace tt {
	class TabToolbar;
}

namespace ot {

	class OT_WIDGETS_API_EXPORT TabToolBarManager : public AbstractToolBar {
		OT_DECL_NOCOPY(TabToolBarManager)
	public:
		static TabToolBarManager* createDefault(void);

		TabToolBarManager();
		virtual ~TabToolBarManager();

		virtual QToolBar* getToolBar(void) override;
		virtual const QToolBar* getToolBar(void) const override;

	private:
		tt::TabToolbar* m_toolBar;

	};

}