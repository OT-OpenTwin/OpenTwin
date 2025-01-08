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
		TabToolBarGroup(TabToolBarPage* _parentPage, tt::Group* _group, const std::string& _name);
		virtual ~TabToolBarGroup();

		const std::string& getName(void) const { return m_name; };

		void setParentTabToolBarPage(TabToolBarPage* _parentTabToolBarPage) { m_parentPage = _parentTabToolBarPage; };
		TabToolBarPage* getParentTabToolBarPage(void) { return m_parentPage; };
		const TabToolBarPage* getParentTabToolBarPage(void) const { return m_parentPage; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Child management
		
		//! @brief Adds a new sub group to the TabToolBar group and returns it.
		//! @param _subGroupName Name/Title of the sub group.
		//! @param _returnExisting If true the sub group will be returned if it already exists. Otherwise nullptr will be returned in this case.
		TabToolBarSubGroup* addSubGroup(const std::string& _subGroupName, bool _returnExisting = false);

		//! @brief Removes the specified sub group from the lists.
		//! Caller keeps ownership of the sub group.
		void forgetSubGroup(TabToolBarSubGroup* _subGroup);

		TabToolBarSubGroup* findSubGroup(const std::string& _subGroupName);
		const TabToolBarSubGroup* findSubGroup(const std::string& _subGroupName) const;
		bool hasSubGroup(const std::string& _subGroupName) const { return this->findSubGroup(_subGroupName) != nullptr; };

	private:
		std::string m_name;
		tt::Group* m_group;

		TabToolBarPage* m_parentPage;
		std::list<TabToolBarSubGroup*> m_subGroups;
	};

}