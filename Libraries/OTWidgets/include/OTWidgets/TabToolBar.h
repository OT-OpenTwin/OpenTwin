// @otlicense
// File: TabToolBar.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTWidgets/AbstractToolBar.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtGui/qicon.h>

// std header
#include <list>
#include <string>

class QMenu;
namespace tt { class TabToolbar; }

namespace ot {

	class MainWindow;
	class ToolButton;
	class TabToolBarPage;
	class TabToolBarGroup;
	class TabToolBarSubGroup;

	class OT_WIDGETS_API_EXPORT TabToolBar : public AbstractToolBar {
		OT_DECL_NOCOPY(TabToolBar)
		OT_DECL_NOMOVE(TabToolBar)
	public:
		//! @brief Creates a default tool button that can be placed in a group or subgroup.
		//! @param _iconPath Button icon path that will be used in the icon manager to load the icon.
		//! @param _text Button text.
		//! @param _menu Menu to set.
		//! @return Created ToolButton.
		//! @ref ToolButton
		static ToolButton* createButton(QWidget* _parent, const QString& _iconPath, const QString& _text, QMenu* _menu = (QMenu*)nullptr);

		//! @brief Creates a default tool button that can be placed in a group or subgroup.
		//! @param _icon Button icon.
		//! @param _text Button text.
		//! @param _menu Menu to set.
		//! @return Created ToolButton.
		//! @ref ToolButton
		static ToolButton* createButton(QWidget* _parent, const QIcon& _icon, const QString& _text, QMenu* _menu = (QMenu*)nullptr);

		TabToolBar(MainWindow* _window = (MainWindow*)nullptr);
		virtual ~TabToolBar();

		virtual QToolBar* getToolBar(void) override;
		virtual const QToolBar* getToolBar(void) const override;

		//! @brief Set current page.
		void setCurrentPage(TabToolBarPage* _page);

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

		void setParentWidget(QWidget* _parent);

	private:
		tt::TabToolbar* m_toolBar;

		std::list<TabToolBarPage*> m_pages;
	};

}