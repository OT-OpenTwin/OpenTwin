//! @file BlockConnectorManager.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/Flags.h"

namespace ot {

	//! @brief Connector location, orientation and logical location on a layer
	enum BlockConnectorLocationFlag {
		ConnectorAuto      = 0x0000, //! @brief Automatic/Default connector location
		ConnectorTop       = 0x0001, //! @brief Connector on top side
		ConnectorRight     = 0x0002, //! @brief Connector on right side
		ConnectorBottom    = 0x0004, //! @brief Connector on bottom side
		ConnectorLeft      = 0x0008, //! @brief Connector on left side
		ConnectorOut       = 0x0010, //! @brief Connector outwards
		ConnectorIn        = 0x0020, //! @brief Connector inwards
		ConnectorPushBack  = 0x1000, //! @brief Connector will be added to the back of the layout
		ConnectorPushFront = 0x2000  //! @brief Connector will be added to the front of the layout
	};

	//! @brief Connector location, orientation and logical location on a layer
	typedef ot::Flags<ot::BlockConnectorLocationFlag> BlockConnectorLocationFlags;

}

OT_ADD_FLAG_FUNCTIONS(ot::BlockConnectorLocationFlag);
