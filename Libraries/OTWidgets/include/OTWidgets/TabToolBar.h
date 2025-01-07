//! @file TabToolBar.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/AbstractToolBar.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <list>
#include <string>

namespace tt { class TabToolbar; }

namespace ot {

	class MainWindow;
	class TabToolBarPage;
	class TabToolBarGroup;
	class TabToolBarSubGroup;

	class OT_WIDGETS_API_EXPORT TabToolBar : public AbstractToolBar {
		OT_DECL_NOCOPY(TabToolBar)
	public:
		TabToolBar(MainWindow* _window = (MainWindow*)nullptr);
		virtual ~TabToolBar();

		virtual QToolBar* getToolBar(void) override;
		virtual const QToolBar* getToolBar(void) const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Element creation

		//! @brief Adds a new page to the TabToolBar and returns it.
		//! @param _pageName Name/Title of the page.
		//! @param _returnExisting If true the page will be returned if it already exists. Otherwise nullptr will be returned in this case.
		TabToolBarPage* addPage(const std::string& _pageName, bool _returnExisting = false);

		//! @brief Adds a new group to the TabToolBar page and returns it.
		//! @param _groupName Name/Title of the group.
		//! @param _returnExisting If true the group will be returned if it already exists. Otherwise nullptr will be returned in this case.
		TabToolBarGroup* addGroup(const std::string& _pageName, const std::string& _groupName, bool _returnExisting = false);

		//! @brief Adds a new sub group to the TabToolBar group and returns it.
		//! @param _subGroupName Name/Title of the sub group.
		//! @param _returnExisting If true the sub group will be returned if it already exists. Otherwise nullptr will be returned in this case.
		TabToolBarSubGroup* addSubGroup(const std::string& _pageName, const std::string& _groupName, const std::string& _subGroupName, bool _returnExisting = false);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Child management

		//! @brief Removes the specified page from the lists.
		//! Caller keeps ownership of the page.
		void forgetPage(TabToolBarPage* _page);

		TabToolBarPage* findPage(const std::string& _pageName);
		const TabToolBarPage* findPage(const std::string& _pageName) const;
		bool hasPage(const std::string& _pageName) const { return this->findPage(_pageName) != nullptr; };

		TabToolBarGroup* findGroup(const std::string& _pageName, const std::string& _groupName);
		const TabToolBarGroup* findGroup(const std::string& _pageName, const std::string& _groupName) const;
		bool hasGroup(const std::string& _pageName, const std::string& _groupName) const { return this->findGroup(_pageName, _groupName) != nullptr; };

		TabToolBarSubGroup* findSubGroup(const std::string& _pageName, const std::string& _groupName, const std::string& _subGroupName);
		const TabToolBarSubGroup* findSubGroup(const std::string& _pageName, const std::string& _groupName, const std::string& _subGroupName) const;
		bool hasSubGroup(const std::string& _pageName, const std::string& _groupName, const std::string& _subGroupName) const { return this->findSubGroup(_pageName, _groupName, _subGroupName) != nullptr; };

	private:
		tt::TabToolbar* m_toolBar;

		std::list<TabToolBarPage*> m_pages;
	};

}