// @otlicense
// File: MenuRequestData.h
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
#include "OTCore/Geometry/Point2D.h"
#include "OTCore/InClassFactory.h"
#include "OTCore/BasicEntityInformation.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/Widgets/WidgetViewBase.h"

// std header
#include <map>
#include <string>
#include <functional>

namespace ot {

	//! @brief The MenuRequestData class is used to define a menu for a menu in the frontend.
	class OT_GUI_API_EXPORT MenuRequestData : public BasicEntityInformation
	{
		OT_DECL_NOMOVE(MenuRequestData)
		OT_DECL_INCLASS_FACTORY(MenuRequestData, MenuRequestData, Registrar)
	public:
		static MenuRequestData* fromJson(const std::string& _jsonString);
		static MenuRequestData* fromJson(const ConstJsonObject& _jsonObject);

		MenuRequestData(WidgetViewBase::ViewType _viewType = WidgetViewBase::CustomView);
		MenuRequestData(const BasicEntityInformation& _basicEntityInformation, WidgetViewBase::ViewType _viewType = WidgetViewBase::CustomView);
		MenuRequestData(BasicEntityInformation&& _basicEntityInformation, WidgetViewBase::ViewType _viewType = WidgetViewBase::CustomView) noexcept;
		virtual ~MenuRequestData() = default;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		virtual std::string getClassName() const = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Set the view type for which the context menu is requested.
		//! @param _viewType The view type for which the context menu is requested.
		void setViewType(WidgetViewBase::ViewType _viewType) { m_viewType = _viewType; };

		//! @brief Get the view type for which the context menu is requested.
		WidgetViewBase::ViewType getViewType() const { return m_viewType; };

	protected:
		MenuRequestData(const MenuRequestData& _other) = default;
		MenuRequestData& operator=(const MenuRequestData& _other) = delete;

	private:
		WidgetViewBase::ViewType m_viewType;
	};

} // namespace ot