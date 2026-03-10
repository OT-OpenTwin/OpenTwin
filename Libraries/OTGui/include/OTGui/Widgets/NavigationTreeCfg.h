// @otlicense
// File: NavigationTreeCfg.h
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
#include "OTSystem/Flags.h"
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/Widgets/NavigationTreeItemCfg.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT NavigationTreeCfg : public ot::Serializable {
	public:
		enum TreeFlag {
			NoFlags = 0x00,
			ItemsDefaultExpanded = 0x01 //! @brief When opening the dialog the items are expanded
		};
		typedef Flags<TreeFlag> TreeFlags;

		static std::string toString(TreeFlag _flag);
		static TreeFlag stringToFlag(const std::string& _flag);

		NavigationTreeCfg();
		virtual ~NavigationTreeCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setFlag(TreeFlag _flag, bool _active = true) { m_flags.set(_flag, _active); };
		void setFlags(const TreeFlags& _flags) { m_flags = _flags; };
		TreeFlags flags(void) const { return m_flags; };

		void setRootItems(const std::list<NavigationTreeItemCfg>& _items) { m_rootItems = _items; };
		void addRootItem(const NavigationTreeItemCfg& _item);
		const std::list<NavigationTreeItemCfg>& rootItems(void) const { return m_rootItems; };

		//! @brief Will merge items with the same name into one
		//! @param _mergeAllChilds If true merge all child items aswell
		void mergeItems(bool _mergeAllChilds = true);

	private:
		TreeFlags m_flags;
		std::list<NavigationTreeItemCfg> m_rootItems;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::NavigationTreeCfg::TreeFlag, ot::NavigationTreeCfg::TreeFlags)