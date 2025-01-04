//! @file TabToolBarPage.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <list>

namespace tt { class Page; }

namespace ot {

	class TabToolBar;
	class TabToolBarGroup;

	class OT_WIDGETS_API_EXPORT TabToolBarPage {
		OT_DECL_NOCOPY(TabToolBarPage)
		OT_DECL_NODEFAULT(TabToolBarPage)
	public:
		TabToolBarPage(TabToolBar* _parentTabToolBar, int _index, const QString& _name);
		virtual ~TabToolBarPage();

		void setParentTabToolBar(TabToolBar* _parentTabToolBar) { m_parentTabToolBar = _parentTabToolBar; };
		TabToolBar* getParentTabToolBar(void) { return m_parentTabToolBar; };
		const TabToolBar* getParentTabToolBar(void) const { return m_parentTabToolBar; };

		//! @brief Removes the specified group from the lists.
		//! Caller keeps ownership of the group.
		void forgetGroup(TabToolBarGroup* _group);

	private:
		tt::Page* m_page;

		TabToolBar* m_parentTabToolBar;
		std::list<TabToolBarGroup*> m_groups;
	};

}