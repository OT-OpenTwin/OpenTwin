//! @file TabToolBarGroup.h
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

namespace tt { class Group; }

namespace ot {

	class TabToolBarPage;
	class TabToolBarSubGroup;

	class OT_WIDGETS_API_EXPORT TabToolBarGroup {
		OT_DECL_NOCOPY(TabToolBarGroup)
			OT_DECL_NODEFAULT(TabToolBarGroup)
	public:
		TabToolBarGroup(TabToolBarPage* _parentPage, const QString& _name);
		virtual ~TabToolBarGroup();

		const std::string& getName(void) const { return m_name; };

		void setParentTabToolBarPage(TabToolBarPage* _parentTabToolBarPage) { m_parentPage = _parentTabToolBarPage; };
		TabToolBarPage* getParentTabToolBarPage(void) { return m_parentPage; };
		const TabToolBarPage* getParentTabToolBarPage(void) const { return m_parentPage; };

		//! @brief Removes the specified sub group from the lists.
		//! Caller keeps ownership of the sub group.
		void forgetSubGroup(TabToolBarSubGroup* _subGroup);

	private:
		std::string m_name;
		tt::Group* m_group;

		TabToolBarPage* m_parentPage;
		std::list<TabToolBarSubGroup*> m_subGroups;
	};

}