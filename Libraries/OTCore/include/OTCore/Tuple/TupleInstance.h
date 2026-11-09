// @otlicense
// File: TupleInstance.h
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

// std header
#include <list>
#include <string>
#include <string>
#include <vector>

namespace ot {

	class OT_CORE_API_EXPORT TupleInstance : public ot::Serializable
	{
		OT_DECL_DEFCOPY(TupleInstance)
		OT_DECL_DEFMOVE(TupleInstance)
	public:
		explicit TupleInstance() = default;
		explicit TupleInstance(const std::string& _tupleTypeName);
		virtual ~TupleInstance() = default;

		bool operator==(const TupleInstance& _other) const;
		bool operator!=(const TupleInstance& _other) const;

		void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;

		inline void setTupleTypeName(const std::string& _tupleTypeName);
		inline const std::string& getTupleTypeName() const { return m_tupleTypeName; }

		inline void setTupleFormatName(const std::string& _formatName);
		inline const std::string& getTupleFormatName() const { return m_tupleFormatName; }

		inline void setTupleUnits(const std::vector<std::string>& _units);
		inline const std::vector<std::string>& getTupleUnits() const { return m_tupleUnits; }

		inline void setTupleElementDataTypes(const std::vector<std::string>& _dataTypes);
		inline const std::vector<std::string>& getTupleElementDataTypes() const { return m_tupleElementDataTypes; }
		
		inline bool isSingle() const { return m_tupleTypeName.empty(); }

	private:
		std::string m_tupleTypeName;
		std::string m_tupleFormatName;
		std::vector<std::string> m_tupleUnits;
		std::vector<std::string> m_tupleElementDataTypes;
	};

}