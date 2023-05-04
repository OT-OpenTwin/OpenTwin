//! @file BlockEditorAPITypes.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockEditorAPIAPIExport.h"

// std header
#include <string>

namespace ot {

	enum BlockComponentPosition {
		ComponentCenter,
		ComponentTop,
		ComponentTopRight,
		ComponentRight,
		ComponentBottomRight,
		ComponentBottom,
		ComponentBottomLeft,
		ComponentLeft,
		ComponentTopLeft
	};

	enum BlockConnectorStyle {
		ConnectorNone,
		ConnectorImage,
		ConnectorPoint,
		ConnectorCircle,
		ConnectorTriangleIn,
		ConnectorTriangleOut,
		ConnectorTriangleUp,
		ConnectorTriangleRight,
		ConnectorTriangleDown,
		ConnectorTriangleLeft
	};

	enum BlockSizeMode {
		SizeFixed,
		SizeAutomatic,
		SizeAutomaticWithMin,
		SizeAutomaticWithMax,
		SizeAutomaticWithMinMax
	};

	BLOCKEDITORAPI_API_EXPORT std::string toString(BlockComponentPosition _pos);
	BLOCKEDITORAPI_API_EXPORT BlockComponentPosition blockComponentPositionFromString(const std::string& _string);

	BLOCKEDITORAPI_API_EXPORT std::string toString(BlockConnectorStyle _style);
	BLOCKEDITORAPI_API_EXPORT BlockConnectorStyle blockConnectorStyleFromString(const std::string& _string);

	BLOCKEDITORAPI_API_EXPORT std::string toString(BlockSizeMode _size);
	BLOCKEDITORAPI_API_EXPORT BlockSizeMode BlockSizeModeFromString(const std::string& _string);
}