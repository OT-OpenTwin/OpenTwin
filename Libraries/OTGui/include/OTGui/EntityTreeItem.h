// @otlicense
// File: EntityTreeItem.h
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
#include "OTCore/BasicEntityInformation.h"
#include "OTGui/NavigationTreeItemIcon.h"

namespace ot {

	class OT_GUI_API_EXPORT EntityTreeItem : public BasicEntityInformation {
		OT_DECL_DEFCOPY(EntityTreeItem)
		OT_DECL_DEFMOVE(EntityTreeItem)
	public:
		EntityTreeItem();
		EntityTreeItem(const BasicEntityInformation& _entityInfo);
		virtual ~EntityTreeItem() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operators

		bool operator == (const EntityTreeItem& _other) const;
		bool operator != (const EntityTreeItem& _other) const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Virtual methods

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void resetModified() {
			m_iconsChanged = false;
			m_isEditableChanged = false;
			m_selectChildsChanged = false;
		}

		void setVisibleIcon(const std::string& _visibleIcon);
		void setHiddenIcon(const std::string& _hiddenIcon);
		void setIcons(const NavigationTreeItemIcon& _icons);
		const NavigationTreeItemIcon& getIcons() const { return m_icons; };
		bool getIconsChanged() const { return m_iconsChanged; };

		void setIsEditable(bool _isEditable);
		bool getIsEditable() const { return m_isEditable; };
		bool getIsEditableChanged() const { return m_isEditableChanged; };

		void setSelectChilds(bool _selectChilds);
		bool getSelectChilds() const { return m_selectChilds; };
		bool getSelectChildsChanged() const { return m_selectChildsChanged; };
		
	private:
		bool m_iconsChanged;
		NavigationTreeItemIcon m_icons;

		bool m_isEditableChanged;
		bool m_isEditable;

		bool m_selectChildsChanged;
		bool m_selectChilds;
	};

}