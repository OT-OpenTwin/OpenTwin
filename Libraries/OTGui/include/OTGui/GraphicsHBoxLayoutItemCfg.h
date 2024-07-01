//! @file GraphicsHBoxLayoutItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsBoxLayoutItemCfg.h"

#define OT_FactoryKey_GraphicsHBoxLayoutItem "OT_GILayH"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsHBoxLayoutItemCfg : public GraphicsBoxLayoutItemCfg {
	public:
		GraphicsHBoxLayoutItemCfg();
		GraphicsHBoxLayoutItemCfg(const GraphicsHBoxLayoutItemCfg& _other);
		virtual ~GraphicsHBoxLayoutItemCfg() {};

		//! \brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy(void) const override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_GraphicsHBoxLayoutItem); };

	private:
		GraphicsHBoxLayoutItemCfg& operator = (const GraphicsHBoxLayoutItemCfg&) = delete;
	};

}