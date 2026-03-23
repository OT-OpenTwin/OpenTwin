// @otlicense
// File: DataPointDecoder.h
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
#include "OTCore/Tuple/TupleInstance.h"

namespace ot
{
	class OT_CORE_API_EXPORT DataPointDecoder : public ot::Serializable
	{
		OT_DECL_DEFCOPY(DataPointDecoder)
		OT_DECL_DEFMOVE(DataPointDecoder)
	public:
		explicit DataPointDecoder() = default;
		explicit DataPointDecoder(const ConstJsonObject& _jsonObject);
		virtual ~DataPointDecoder() = default;

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		inline void setFieldName(const std::string& _fieldName) { m_fieldName = _fieldName; };
		inline void setFiledName(std::string&& _fieldName) { m_fieldName = std::move(_fieldName); };
		inline const std::string& getFieldName() const { return m_fieldName; };

		inline void setLabel(const std::string& _label) { m_label = _label; };
		inline void setLabel(std::string&& _label) { m_label = std::move(_label); };
		inline const std::string& getLabel() const { return m_label; };

		inline void addDimension(uint32_t _dimension) { m_dimension.push_back(_dimension); };
		inline void setDimension(const std::vector<uint32_t>& _dimension) { m_dimension = _dimension; };
		inline void setDimension(std::vector<uint32_t>&& _dimension) { m_dimension = std::move(_dimension); };
		inline const std::vector<uint32_t>& getDimension() const { return m_dimension; };

		inline void setTupleInstance(const TupleInstance& _tupleInstance) { m_tupleInstance = _tupleInstance; };
		inline void setTupleInstance(TupleInstance&& _tupleInstance) { m_tupleInstance = std::move(_tupleInstance); };
		inline const TupleInstance& getTupleInstance() const { return m_tupleInstance; };

	private:
		//! @brief This one is also bson serialised in the EntityResult1DCurve entity.
		std::string m_fieldName = "";
		std::string m_label = "";
		std::vector<uint32_t> m_dimension;
		TupleInstance m_tupleInstance;
	};
}