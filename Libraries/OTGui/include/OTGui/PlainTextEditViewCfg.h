//! @file PlainTextEditViewCfg.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/WidgetViewCfg.h"

#define OT_WIDGETTYPE_PlainTextEditView "PlainTextEditView"

namespace ot {

	class OT_GUI_API_EXPORT PlainTextEditViewCfg : public ot::WidgetViewCfg {
		OT_DECL_NOCOPY(PlainTextEditViewCfg)
	public:
		PlainTextEditViewCfg();
		virtual ~PlainTextEditViewCfg();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		virtual std::string getViewType(void) const override { return OT_WIDGETTYPE_PlainTextEditView; };

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	private:

	};

}