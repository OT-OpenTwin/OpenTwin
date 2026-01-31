// @otlicense
// File: SequenceDiaDirectCallCfg.h
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
#include "OTGui/Diagram/SequenceDiaAbstractCallCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT SequenceDiaDirectCallCfg : public SequenceDiaAbstractCallCfg {
		OT_DECL_NOCOPY(SequenceDiaDirectCallCfg)
		OT_DECL_NOMOVE(SequenceDiaDirectCallCfg)
	public:
		SequenceDiaDirectCallCfg() = default;
		virtual ~SequenceDiaDirectCallCfg() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		static std::string className() { return "SequenceDiaDirectCallCfg"; };
		virtual std::string getClassName() const override { return SequenceDiaDirectCallCfg::className(); };
		virtual bool isValid() const override;

		void setFunctionName(const std::string& _name) { m_functionName = _name; };
		const std::string& getFunctionName() const { return m_functionName; };

	private:
		std::string m_functionName;

	};

}