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

	class OT_GUI_API_EXPORT WidgetViewCfg : public ot::WidgetViewBase {
		OT_DECL_NOCOPY(WidgetViewCfg)
	public:
		// ###########################################################################################################################################################################################################################################################################################################################

		WidgetViewCfg();
		virtual ~WidgetViewCfg();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Pure virtual functions

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

		void setParentViewName(const std::string& _parentName) { m_parentViewName = _parentName; };
		const std::string& parentViewName(void) const { return m_parentViewName; };

	private:
		std::string m_parentViewName;
	};

}
