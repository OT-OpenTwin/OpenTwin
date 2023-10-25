//! @file GraphicsConnectionItem.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsLineItem.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsConnectionItem "OT_GIConnection"

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsConnectionItem : public GraphicsLineItem {
	public:
		GraphicsConnectionItem();
		virtual ~GraphicsConnectionItem();

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsConnectionItem); };

		void connectItems(GraphicsItem* _origin, GraphicsItem* _dest);
		void disconnectItems(void);

		void updateConnection(void);

		GraphicsItem* originItem(void) { return m_origin; };
		GraphicsItem* destItem(void) { return m_dest; };

	private:
		GraphicsItem* m_origin;
		GraphicsItem* m_dest;
	};

}