//! @file GraphicsVBoxLayoutItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsBoxLayoutItemCfg.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItemCfg "OT_GICLayV"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsVBoxLayoutItemCfg : public GraphicsBoxLayoutItemCfg {
	public:
		GraphicsVBoxLayoutItemCfg();
		virtual ~GraphicsVBoxLayoutItemCfg() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsVBoxLayoutItemCfg); };

	private:
		GraphicsVBoxLayoutItemCfg(const GraphicsVBoxLayoutItemCfg&) = delete;
		GraphicsVBoxLayoutItemCfg& operator = (const GraphicsVBoxLayoutItemCfg&) = delete;
	};

}