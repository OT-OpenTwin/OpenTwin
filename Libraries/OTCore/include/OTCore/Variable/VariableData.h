// @otlicense
// File: VariableData.h
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
#include "OTCore/Variable/Variable.h"

// std map
#include <map>

namespace ot {

	class OT_CORE_API_EXPORT VariableData : public Serializable
	{
		OT_DECL_DEFCOPY(VariableData)
		OT_DECL_DEFMOVE(VariableData)
	public:
		VariableData() = default;
		VariableData(const ConstJsonObject& _jsonObject);
		virtual ~VariableData() = default;

		void addVariable(const std::string& _name, const Variable& _variable);
		bool hasVariable(const std::string& _name) const;
		const Variable& getVariable(const std::string& _name) const;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

	private:
		std::map<std::string, Variable> m_variables;

	};

}