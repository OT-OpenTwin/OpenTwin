// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Point2D.h"
#include "OTCore/InClassFactory.h"
#include "OTCore/BasicEntityInformation.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/Widgets/WidgetViewBase.h"

// std header
#include <map>
#include <string>
#include <functional>

namespace ot {

	//! @brief The ContextRequestData class is used to request the creation of a new context menus in the frontend.
	class OT_GUI_API_EXPORT ContextRequestData : public BasicEntityInformation
	{
		OT_DECL_DEFCOPY(ContextRequestData)
		OT_DECL_DEFMOVE(ContextRequestData)
		OT_DECL_INCLASS_FACTORY(ContextRequestData, ContextRequestData, Registrar)
	public:

		static ContextRequestData* fromJson(const std::string& _jsonString);
		static ContextRequestData* fromJson(const ConstJsonObject& _jsonObject);

		ContextRequestData(WidgetViewBase::ViewType _viewType = WidgetViewBase::CustomView);
		ContextRequestData(const BasicEntityInformation& _basicEntityInformation, WidgetViewBase::ViewType _viewType = WidgetViewBase::CustomView);
		ContextRequestData(BasicEntityInformation&& _basicEntityInformation, WidgetViewBase::ViewType _viewType = WidgetViewBase::CustomView) noexcept;
		~ContextRequestData() = default;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief JSON key used to identify the class name in a JSON object.
		static constexpr const char* classNameJsonKey() noexcept { return "ClassName"; };

		//! @brief Get the name of the class.
		static constexpr const char* className() noexcept { return "ContextRequestData"; };

		//! @brief Get the name of the object class.
		virtual std::string getClassName() const { return ContextRequestData::className(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Set the view type for which the context menu is requested.
		//! @param _viewType The view type for which the context menu is requested.
		void setViewType(WidgetViewBase::ViewType _viewType) { m_viewType = _viewType; };

		//! @brief Get the view type for which the context menu is requested.
		WidgetViewBase::ViewType getViewType() const { return m_viewType; };

	private:
		WidgetViewBase::ViewType m_viewType;
	};

} // namespace ot