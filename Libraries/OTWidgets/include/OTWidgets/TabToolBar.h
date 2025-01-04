//! @file TabToolBar.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/AbstractToolBar.h"

// std header
#include <list>

namespace tt { class TabToolbar; }

namespace ot {

	class MainWindow;
	class TabToolBarPage;

	class OT_WIDGETS_API_EXPORT TabToolBar : public AbstractToolBar {
		OT_DECL_NOCOPY(TabToolBar)
	public:
		TabToolBar(MainWindow* _window = (MainWindow*)nullptr);
		virtual ~TabToolBar();

		virtual QToolBar* getToolBar(void) override;
		virtual const QToolBar* getToolBar(void) const override;

		//! @brief Removes the specified page from the lists.
		//! Caller keeps ownership of the page.
		void forgetPage(TabToolBarPage* _page);

	private:
		tt::TabToolbar* m_toolBar;

		std::list<TabToolBarPage*> m_pages;
	};

}