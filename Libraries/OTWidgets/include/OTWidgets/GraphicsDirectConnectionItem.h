//! @file GraphicsConnectionItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTWidgets/GraphicsLineItem.h"
#include "OTWidgets/GraphicsConnectionItem.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsDirectConnectionItem "OT_GIDirCon"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsDirectConnectionItem : public GraphicsConnectionItem, public GraphicsLineItem {
	public:
		GraphicsDirectConnectionItem();
		virtual ~GraphicsDirectConnectionItem();

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsDirectConnectionItem); };

		virtual void updateConnection(void) override;
	};

}