// @otlicense
// File: TabToolBarSubGroup.h
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
#include "OTWidgets/WidgetTypes.h"

class QWidget;
namespace tt { class SubGroup; }

namespace ot {

	class ToolButton;
	class TabToolBarGroup;

	class OT_WIDGETS_API_EXPORT TabToolBarSubGroup {
		OT_DECL_NOCOPY(TabToolBarSubGroup)
		OT_DECL_NODEFAULT(TabToolBarSubGroup)
	public:
		explicit TabToolBarSubGroup(TabToolBarGroup* _parentGroup, tt::SubGroup* _subGroup, const std::string& _name);
		virtual ~TabToolBarSubGroup();

		const std::string& getName(void) const { return m_name; };

		void setParentTabToolBarGroup(TabToolBarGroup* _parentTabToolBarGroup) { m_parentGroup = _parentTabToolBarGroup; };
		TabToolBarGroup* getParentTabToolBarGroup(void) { return m_parentGroup; };
		const TabToolBarGroup* getParentTabToolBarGroup(void) const { return m_parentGroup; };

		tt::SubGroup* getSubGroup(void) const { return m_subGroup; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Child management

		//! @brief Creates and adds a default tool button.
		//! @param _iconPath Button icon path that will be used in the icon manager to load the icon.
		//! @param _text Button text.
		//! @param _menu Menu to set.
		//! @return Created ToolButton.
		//! @ref ToolButton
		ToolButton* addToolButton(const QString& _iconPath, const QString& _text, QMenu* _menu);

		//! @brief Creates and adds a default tool button.
		//! @param _icon Button icon.
		//! @param _text Button text.
		//! @param _menu Menu to set.
		//! @return Created ToolButton.
		//! @ref ToolButton
		ToolButton* addToolButton(const QIcon& _icon, const QString& _text, QMenu* _menu);

	private:
		std::string m_name;
		tt::SubGroup* m_subGroup;

		TabToolBarGroup* m_parentGroup;
	};

}