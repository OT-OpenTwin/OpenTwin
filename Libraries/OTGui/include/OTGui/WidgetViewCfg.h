//! @file WidgetViewCfg.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/WidgetViewBase.h"

// std header
#include <list>
#include <string>

#pragma warning(disable:4251)

#define OT_JSON_MEMBER_WidgetViewCfgType "Type"

namespace ot {

	//! @class WidgetViewCfg
	//! @brief WidgetView configuration base class.
	//! The WidgetViewCfg class is used an interface for WidgetView configurations.
	//! Custom configuration can be implemented by overriding the pure virtual method /ref getViewType() "getViewType()".
	//! When creating a custom widget view it should be registered at the /ref WidgetViewCfgFactory "WidgetViewCfgFactory".
	//! The /ref WidgetViewCfgRegistrar "WidgetViewCfgRegistrar" can be used to register the new WidgetViewCfg at the factory.
	class OT_GUI_API_EXPORT WidgetViewCfg : public ot::WidgetViewBase {
		OT_DECL_NOCOPY(WidgetViewCfg)
	public:
		// ###########################################################################################################################################################################################################################################################################################################################

		WidgetViewCfg();
		virtual ~WidgetViewCfg();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Pure virtual functions

		//! @brief Returns the view type.
		//! The type must be unique when using the /ref WidgetViewCfgFactory "Widget View Configuration Factory".
		virtual std::string getViewType(void) const = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter/Getter

		//! @brief Set the name of the parent view.
		//! @param _parentName The parent name to set.
		void setParentViewName(const std::string& _parentName) { m_parentViewName = _parentName; };

		//! @brief Return the name of the parent view.
		const std::string& parentViewName(void) const { return m_parentViewName; };

	private:
		std::string m_parentViewName; //! @brief The parent view name.
	};

}
