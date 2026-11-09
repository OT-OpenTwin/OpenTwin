// @otlicense
// File: TableFilterChangeEvent.h
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

// OpenTwin header
#include "OTCore/BasicEntityInformation.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTGui/Event/GuiEvent.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT TableFilterChangeEvent : public GuiEvent
	{
		OT_DECL_DEFCOPY(TableFilterChangeEvent)
		OT_DECL_DEFMOVE(TableFilterChangeEvent)
	public:
		TableFilterChangeEvent() = default;
		TableFilterChangeEvent(const BasicEntityInformation& _basicEntityInformation);
		TableFilterChangeEvent(BasicEntityInformation&& _basicEntityInformation) noexcept;
		TableFilterChangeEvent(const ConstJsonObject& _jsonObject);
		virtual ~TableFilterChangeEvent() = default;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		static constexpr const char* className() noexcept { return "TableFilterChangeEvent"; };
		virtual std::string getClassName() const { return TableFilterChangeEvent::className(); };

		void setTableInformation(const BasicEntityInformation& _tableInformation) { m_tableInformation = _tableInformation; };
		void setTableInformation(BasicEntityInformation&& _tableInformation) noexcept { m_tableInformation = std::move(_tableInformation); };
		const BasicEntityInformation& getTableInformation() const { return m_tableInformation; };

		void addFilterDescription(const ValueComparisonDescription& _filterDescription) { m_filterDescriptions.push_back(_filterDescription); };
		void addFilterDescription(ValueComparisonDescription&& _filterDescription) noexcept { m_filterDescriptions.push_back(std::move(_filterDescription)); };
		void setFilterDescriptions(const std::list<ValueComparisonDescription>& _filterDescriptions) { m_filterDescriptions = _filterDescriptions; };
		void setFilterDescriptions(std::list<ValueComparisonDescription>&& _filterDescriptions) noexcept { m_filterDescriptions = std::move(_filterDescriptions); };
		const std::list<ValueComparisonDescription>& getFilterDescriptions() const { return m_filterDescriptions; };

	private:
		BasicEntityInformation m_tableInformation;
		std::list<ValueComparisonDescription> m_filterDescriptions;
	};
}