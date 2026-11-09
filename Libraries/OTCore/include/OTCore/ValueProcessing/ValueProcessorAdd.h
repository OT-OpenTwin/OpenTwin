// @otlicense
// File: ValueProcessorAdd.h
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
#include "OTCore/ValueProcessing/ValueProcessor.h"

namespace ot {

	class OT_CORE_API_EXPORT ValueProcessorAdd : public ValueProcessor
	{
		OT_DECL_DEFCOPY(ValueProcessorAdd)
		OT_DECL_DEFMOVE(ValueProcessorAdd)
	public:
		ValueProcessorAdd(double _summand = 0.);
		virtual ~ValueProcessorAdd() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Processing

		ot::Variable execute(const ot::Variable& _input) override;
		OT_DECL_NODISCARD ValueProcessor* inverse() const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		OT_DECL_NODISCARD ValueProcessor* createCopy() const override { return new ValueProcessorAdd(*this); };
		static std::string className() { return "ValueProcessorAdd"; };
		std::string getClassName() const override { return ValueProcessorAdd::className(); };

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		inline void setSummand(double _summand) { m_summand = _summand; };
		inline double getSummand() const { return m_summand; };

	private:
		double m_summand;

	};

}