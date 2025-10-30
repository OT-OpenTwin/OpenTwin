// @otlicense
// File: NavigationTreeItem.h
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
#include "OTCore/Serializable.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/NavigationTreeItemIcon.h"

// std header
#include <list>
#include <string>

#pragma warning (disable: 4251)

namespace ot {

	class OT_GUI_API_EXPORT NavigationTreeItem : public ot::Serializable {
	public:
		enum ItemFlag {
			NoItemFlags         = 0 << 0, //! @brief Empty mask, no flags set
			RemoveItemWhenEmpty = 1 << 0, //! @brief If set the item will be removed when all the child items are removed
			ItemMayBeAdded      = 1 << 1, //! @brief If set the item may be added/dragged/dropped
			ItemIsSelected      = 1 << 2, //! @brief If set this item is selected
			ItemIsInvisible     = 1 << 3  //! @brief If set the item is invisible
		};
		typedef Flags<ItemFlag> ItemFlags;

		NavigationTreeItem();
		NavigationTreeItem(const std::string& _text, const ItemFlags& _flags = NoItemFlags);
		NavigationTreeItem(const std::string& _text, const std::string& _iconPath, const ItemFlags& _flags = NoItemFlags);
		NavigationTreeItem(const std::string& _text, const std::string& _iconPath, const std::list<NavigationTreeItem>& _childItems, const ItemFlags& _flags = NoItemFlags);
		NavigationTreeItem(const NavigationTreeItem& _other);
		virtual ~NavigationTreeItem();

		NavigationTreeItem& operator = (const NavigationTreeItem& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText() const { return m_text; };

		void setIcon(const NavigationTreeItemIcon& _icon) { m_icon = _icon; };
		const NavigationTreeItemIcon& getIcon() const { return m_icon; };

		void setVisibleIconPath(const std::string& _iconPath) { m_icon.setVisibleIcon(_iconPath); };
		const std::string& getVisibleIconPath() const { return m_icon.getVisibleIcon(); };

		void setHiddenIconPath(const std::string& _iconPath) { m_icon.setHiddenIcon(_iconPath); };
		const std::string& getHiddenIconPath() const { return m_icon.getHiddenIcon(); };

		void addChildItem(const NavigationTreeItem& _item);
		void setChildItems(const std::list<NavigationTreeItem>& _items);
		const std::list<NavigationTreeItem>& getChildItems() const { return m_childs; };

		void setFlag(ItemFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setFlags(const ItemFlags& _flags) { m_flags = _flags; };
		const ItemFlags& getFlags() const { return m_flags; };

		void merge(const NavigationTreeItem& _other);

		std::string itemPath(char _delimiter = '/', const std::string& _suffix = std::string()) const;

		//! @brief Removes all child items which dont have the flag(s) set
		//! Returns true if this item or any of the childs matches the filter
		bool filter(const ItemFlags& _flags);

	protected:
		void setParentNavigationTreeItem(NavigationTreeItem* _parent) { m_parent = _parent; };
		NavigationTreeItem* parentNavigationTreeItem() const { return m_parent; };

	private:
		NavigationTreeItem* m_parent;

		std::string m_text;
		NavigationTreeItemIcon m_icon;
		std::list<NavigationTreeItem> m_childs;
		ItemFlags m_flags;

	};

}

OT_ADD_FLAG_FUNCTIONS(ot::NavigationTreeItem::ItemFlag)