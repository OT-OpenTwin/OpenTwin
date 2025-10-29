// @otlicense

//! @file GraphcisLayoutItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GuiTypes.h"

// std header
#include <list>
#include <vector>

namespace ot {

	class OT_GUI_API_EXPORT GraphicsLayoutItemCfg : public GraphicsItemCfg {
		OT_DECL_NOMOVE(GraphicsLayoutItemCfg)
	public:
		GraphicsLayoutItemCfg() {};
		GraphicsLayoutItemCfg(const GraphicsLayoutItemCfg& _other);
		virtual ~GraphicsLayoutItemCfg();

		GraphicsLayoutItemCfg& operator=(const GraphicsLayoutItemCfg&) = delete;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		virtual void addChildItem(ot::GraphicsItemCfg* _item) = 0;
	};

}