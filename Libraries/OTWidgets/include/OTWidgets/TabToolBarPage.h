// @otlicense

//! @file TabToolBarPage.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <list>
#include <string>

namespace tt { class Page; }

namespace ot {

	class TabToolBar;
	class TabToolBarGroup;
	class TabToolBarSubGroup;

	class OT_WIDGETS_API_EXPORT TabToolBarPage {
		OT_DECL_NOCOPY(TabToolBarPage)
		OT_DECL_NODEFAULT(TabToolBarPage)
	public:
		TabToolBarPage(TabToolBar* _parentTabToolBar, tt::Page* _page, const std::string& _name);
		virtual ~TabToolBarPage();

		const std::string& getName(void) const { return m_name; };

		void setParentTabToolBar(TabToolBar* _parentTabToolBar) { m_parentTabToolBar = _parentTabToolBar; };
		TabToolBar* getParentTabToolBar(void) { return m_parentTabToolBar; };
		const TabToolBar* getParentTabToolBar(void) const { return m_parentTabToolBar; };

		tt::Page* getPage(void) const { return m_page; };

		//! @brief Set this page as the current page in the TabToolBar.
		void setAsCurrentPage(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Element creation

		//! @brief Adds a new group to the TabToolBar page and returns it.
		//! @param _groupName Name/Title of the group.
		//! @param _returnExisting If true the group will be returned if it already exists. Otherwise nullptr will be returned in this case.
		TabToolBarGroup* addGroup(const std::string& _groupName, bool _returnExisting = false);

		//! @brief Adds a new sub group to the TabToolBar group and returns it.
		//! @param _subGroupName Name/Title of the sub group.
		//! @param _returnExisting If true the sub group will be returned if it already exists. Otherwise nullptr will be returned in this case.
		TabToolBarSubGroup* addSubGroup(const std::string& _groupName, const std::string& _subGroupName, bool _returnExisting = false);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Child management

		//! @brief Removes the specified group from the lists.
		//! Caller keeps ownership of the group.
		void forgetGroup(TabToolBarGroup* _group);

		TabToolBarGroup* findGroup(const std::string& _groupName);
		const TabToolBarGroup* findGroup(const std::string& _groupName) const;
		bool hasGroup(const std::string& _groupName) const { return this->findGroup(_groupName) != nullptr; };

		TabToolBarSubGroup* findSubGroup(const std::string& _groupName, const std::string& _subGroupName);
		const TabToolBarSubGroup* findSubGroup(const std::string& _groupName, const std::string& _subGroupName) const;
		bool hasSubGroup(const std::string& _groupName, const std::string& _subGroupName) const { return this->findSubGroup(_groupName, _subGroupName) != nullptr; };

	private:
		std::string m_name;
		tt::Page* m_page;

		TabToolBar* m_parentTabToolBar;
		std::list<TabToolBarGroup*> m_groups;
	};

}