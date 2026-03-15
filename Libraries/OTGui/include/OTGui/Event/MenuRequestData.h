// @otlicense

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