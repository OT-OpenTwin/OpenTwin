//! @file GraphicsConnectionItem.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTGui/GraphicsConnectionCfg.h"

namespace ot {
	
	class GraphicsItem;

	class GraphicsConnectionItem {
	public:
		GraphicsConnectionItem();
		virtual ~GraphicsConnectionItem();

		virtual void updateConnection(void) = 0;

		//! @brief Creates a configuration object containing the items origin and destination information
		GraphicsConnectionCfg getConnectionInformation(void) const;

		void connectItems(GraphicsItem* _origin, GraphicsItem* _dest);
		void disconnectItems(void);

		GraphicsItem* originItem(void) { return m_origin; };
		GraphicsItem* destItem(void) { return m_dest; };

	private:
		GraphicsItem* m_origin;
		GraphicsItem* m_dest;
	};

}