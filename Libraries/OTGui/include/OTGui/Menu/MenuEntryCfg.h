// @otlicense
// File: MenuEntryCfg.h
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

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT MenuEntryCfg : public Serializable {
	public:
		enum EntryType {
			Menu,
			Button,
			Separator
		};

		static std::string toString(EntryType _type);
		static EntryType stringToEntryType(const std::string& _type);
		static const std::string& entryTypeJsonKey(void);

		MenuEntryCfg();
		MenuEntryCfg(const MenuEntryCfg& _other);
		virtual ~MenuEntryCfg();
		
		MenuEntryCfg& operator = (const MenuEntryCfg& _other);

		virtual MenuEntryCfg* createCopy(void) const = 0;
		virtual EntryType getMenuEntryType(void) const = 0;

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;
	};

}