//! @file GraphicsHBoxLayoutItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsBoxLayoutItemCfg.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItemCfg "OT_GICLayH"

namespace ot {

	class OT_GUI_API_EXPORTONLY GraphicsHBoxLayoutItemCfg : public GraphicsBoxLayoutItemCfg {
	public:
		GraphicsHBoxLayoutItemCfg();
		virtual ~GraphicsHBoxLayoutItemCfg() {};

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsHBoxLayoutItemCfg); };

	private:
		GraphicsHBoxLayoutItemCfg(const GraphicsHBoxLayoutItemCfg&) = delete;
		GraphicsHBoxLayoutItemCfg& operator = (const GraphicsHBoxLayoutItemCfg&) = delete;
	};

}