// @otlicense
// File: SelectEntitiesDialogCfg.h
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
#include "OTGui/NavigationTreeItem.h"
#include "OTGui/NavigationTreePackage.h"

// std header
#include <list>
#include <string>

namespace ot {

	class OT_GUI_API_EXPORT SelectEntitiesDialogCfg : public NavigationTreePackage {
	public:
		SelectEntitiesDialogCfg();
		virtual ~SelectEntitiesDialogCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& title(void) const { return m_title; };

	private:
		std::string m_title;
	};

}