// @otlicense
// File: SequenceDiaAbstractCallCfg.h
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

namespace ot {

	class OT_GUI_API_EXPORT SequenceDiaAbstractCallCfg : public Serializable {
		OT_DECL_NOCOPY(SequenceDiaAbstractCallCfg)
		OT_DECL_NOMOVE(SequenceDiaAbstractCallCfg)
	public:
		static std::string callClassNameJsonKey() { return "CallType"; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor / Destructor

		SequenceDiaAbstractCallCfg() = default;
		virtual ~SequenceDiaAbstractCallCfg() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		virtual std::string getClassName() const = 0;
		virtual bool isValid() const = 0;

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText() const { return m_text; };

	private:
		std::string m_text;

	};

}