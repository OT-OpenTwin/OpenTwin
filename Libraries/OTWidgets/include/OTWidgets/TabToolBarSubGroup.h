//! @file TabToolBarSubGroup.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace tt { class SubGroup; }

namespace ot {

	class TabToolBarGroup;

	class OT_WIDGETS_API_EXPORT TabToolBarSubGroup {
		OT_DECL_NOCOPY(TabToolBarSubGroup)
		OT_DECL_NODEFAULT(TabToolBarSubGroup)
	public:
		TabToolBarSubGroup(TabToolBarGroup* _parentGroup, tt::SubGroup* _subGroup, const std::string& _name);
		virtual ~TabToolBarSubGroup();

		const std::string& getName(void) const { return m_name; };

		void setParentTabToolBarGroup(TabToolBarGroup* _parentTabToolBarGroup) { m_parentGroup = _parentTabToolBarGroup; };
		TabToolBarGroup* getParentTabToolBarGroup(void) { return m_parentGroup; };
		const TabToolBarGroup* getParentTabToolBarGroup(void) const { return m_parentGroup; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Child management



	private:
		std::string m_name;
		tt::SubGroup* m_subGroup;

		TabToolBarGroup* m_parentGroup;
	};

}