// @otlicense
// File: QueryInformation.h
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
#include "OTCore/QueryDescription/DataPointDecoder.h"
#include "OTGui/GuiTypes.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot
{
	class OT_GUI_API_EXPORT QueryInformation : public ot::Serializable
	{
		OT_DECL_DEFCOPY(QueryInformation)
		OT_DECL_DEFMOVE(QueryInformation)
	public:
		QueryInformation() = default;
		~QueryInformation() = default;

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		inline void setQuery(const std::string& _query) { m_query = _query; };
		inline void setQuery(std::string&& _query) { m_query = std::move(_query); };
		const std::string& getQuery() const { return m_query; };

		inline void setProjection(const std::string& _projection) { m_projection = _projection; };
		inline void setProjection(std::string&& _projection) { m_projection = std::move(_projection); };
		const std::string& getProjection() const { return m_projection; };

		//! @brief Set the quantity description for the main quantity of interest (e.g. Y-Axis in a 1D plot).
		//! This is also used for the curve family parameter in a 1D plot if no parameter descriptions are provided.
		inline void setQuantityDescription(const DataPointDecoder& _quantityDescription) { m_quantityDescription = _quantityDescription; };
		inline void setQuantityDescription(DataPointDecoder&& _quantityDescription) { m_quantityDescription = std::move(_quantityDescription); };
		const DataPointDecoder& getQuantityDescription() const { return m_quantityDescription; };

		inline void addParameterDescription(const DataPointDecoder& _parameterDescription) { m_parameterDescriptions.push_back(_parameterDescription); };
		inline void addParameterDescription(DataPointDecoder&& _parameterDescription) { m_parameterDescriptions.push_back(std::move(_parameterDescription)); };
		inline void addParameterDescriptionFront(const DataPointDecoder& _parameterDescription) { m_parameterDescriptions.push_front(_parameterDescription); };
		inline void addParameterDescriptionFront(DataPointDecoder&& _parameterDescription) { m_parameterDescriptions.push_front(std::move(_parameterDescription)); };
		inline void setParameterDescriptions(const std::list<DataPointDecoder>& _parameterDescriptions) { m_parameterDescriptions = _parameterDescriptions; };
		inline void setParameterDescriptions(std::list<DataPointDecoder>&& _parameterDescriptions) { m_parameterDescriptions = std::move(_parameterDescriptions); };
		const std::list<DataPointDecoder>& getParameterDescriptions() const { return m_parameterDescriptions; };

	private:
		std::string m_query;
		std::string m_projection;

		DataPointDecoder m_quantityDescription;

		// Curves are using the parameter prioritised by their order in this list. E.g. the first entry is used in a 1D plot as X-Axis, the remaining are used for plotting the curve family.
		std::list<DataPointDecoder> m_parameterDescriptions;
	};	
}
