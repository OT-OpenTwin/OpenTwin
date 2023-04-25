// OpenTwin header
#include "OTBlockEditorAPI/BlockEditorAPITypes.h"

// std header
#include <cassert>

std::string ot::toString(BlockComponentPosition _pos) {
	switch (_pos)
	{
	case ot::BlockComponentPosition::ComponentCenter: return "Center";
	case ot::BlockComponentPosition::ComponentTop: return "Top";
	case ot::BlockComponentPosition::ComponentTopRight: return "TopRight";
	case ot::BlockComponentPosition::ComponentRight: return "Right";
	case ot::BlockComponentPosition::ComponentBottomRight: return "BottomRight";
	case ot::BlockComponentPosition::ComponentBottom: return "Bottom";
	case ot::BlockComponentPosition::ComponentBottomLeft: return "BottomLeft";
	case ot::BlockComponentPosition::ComponentLeft: return "Left";
	case ot::BlockComponentPosition::ComponentTopLeft: return "TopLeft";
	default:
		//todo: replace with otAssert
		assert(0);
		return "Center";
	}
}

ot::BlockComponentPosition ot::blockComponentPositionFromString(const std::string& _string) {
	if (_string == toString(ot::BlockComponentPosition::ComponentCenter)) return ot::BlockComponentPosition::ComponentCenter;
	else if (_string == toString(ot::BlockComponentPosition::ComponentTop)) return ot::BlockComponentPosition::ComponentTop;
	else if (_string == toString(ot::BlockComponentPosition::ComponentTopRight)) return ot::BlockComponentPosition::ComponentTopRight;
	else if (_string == toString(ot::BlockComponentPosition::ComponentRight)) return ot::BlockComponentPosition::ComponentRight;
	else if (_string == toString(ot::BlockComponentPosition::ComponentBottomRight)) return ot::BlockComponentPosition::ComponentBottomRight;
	else if (_string == toString(ot::BlockComponentPosition::ComponentBottom)) return ot::BlockComponentPosition::ComponentBottom;
	else if (_string == toString(ot::BlockComponentPosition::ComponentBottomLeft)) return ot::BlockComponentPosition::ComponentBottomLeft;
	else if (_string == toString(ot::BlockComponentPosition::ComponentLeft)) return ot::BlockComponentPosition::ComponentLeft;
	else if (_string == toString(ot::BlockComponentPosition::ComponentTopLeft)) return ot::BlockComponentPosition::ComponentTopLeft;
	else {
		//todo: replace with otAssert
		assert(0);
		return ot::BlockComponentPosition::ComponentCenter;
	}
}

std::string ot::toString(BlockConnectorStyle _style) {
	switch (_style)
	{
	case ot::BlockConnectorStyle::ConnectorNone: return "None";
	case ot::BlockConnectorStyle::ConnectorImage: return "Image";
	case ot::BlockConnectorStyle::ConnectorPoint: return "Point";
	case ot::BlockConnectorStyle::ConnectorCircle: return "Circle";
	case ot::BlockConnectorStyle::ConnectorTriangleUp: return "TriangleUp";
	case ot::BlockConnectorStyle::ConnectorTriangleRight: return "TriangleRight";
	case ot::BlockConnectorStyle::ConnectorTriangleDown: return "TriangleDown";
	case ot::BlockConnectorStyle::ConnectorTriangleLeft: return "TriangleLeft";
	default:
		//todo: replace with otAssert
		assert(0);
		return "None";
	}
}

ot::BlockConnectorStyle ot::blockConnectorStyleFromString(const std::string& _string) {
	if (_string == ot::toString(ot::BlockConnectorStyle::ConnectorNone)) return ot::BlockConnectorStyle::ConnectorNone;
	else if (_string == ot::toString(ot::BlockConnectorStyle::ConnectorImage)) return ot::BlockConnectorStyle::ConnectorImage;
	else if (_string == ot::toString(ot::BlockConnectorStyle::ConnectorPoint)) return ot::BlockConnectorStyle::ConnectorPoint;
	else if (_string == ot::toString(ot::BlockConnectorStyle::ConnectorCircle)) return ot::BlockConnectorStyle::ConnectorCircle;
	else if (_string == ot::toString(ot::BlockConnectorStyle::ConnectorTriangleUp)) return ot::BlockConnectorStyle::ConnectorTriangleUp;
	else if (_string == ot::toString(ot::BlockConnectorStyle::ConnectorTriangleRight)) return ot::BlockConnectorStyle::ConnectorTriangleRight;
	else if (_string == ot::toString(ot::BlockConnectorStyle::ConnectorTriangleDown)) return ot::BlockConnectorStyle::ConnectorTriangleDown;
	else if (_string == ot::toString(ot::BlockConnectorStyle::ConnectorTriangleLeft)) return ot::BlockConnectorStyle::ConnectorTriangleLeft;
	else {
		//todo: replace with otAssert
		assert(0);
		return ot::BlockConnectorStyle::ConnectorNone;
	}
}

std::string ot::toString(BlockSizeMode _size) {
	switch (_size)
	{
	case ot::BlockSizeMode::SizeFixed: return "Fixed";
	case ot::BlockSizeMode::SizeAutomatic: return "Automatic";
	case ot::BlockSizeMode::SizeAutomaticWithMin: return "AutomaticWithMin";
	case ot::BlockSizeMode::SizeAutomaticWithMax: return "AutomaticWithMax";
	case ot::BlockSizeMode::SizeAutomaticWithMinMax: return "AutomaticWithMinMax";
	default:
		//todo: replace with otAssert
		assert(0);
		return "Fixed";
	}
}

ot::BlockSizeMode ot::BlockSizeModeFromString(const std::string& _string) {
	if (_string == ot::toString(ot::BlockSizeMode::SizeFixed)) return ot::BlockSizeMode::SizeFixed;
	else if (_string == ot::toString(ot::BlockSizeMode::SizeAutomatic)) return ot::BlockSizeMode::SizeAutomatic;
	else if (_string == ot::toString(ot::BlockSizeMode::SizeAutomaticWithMin)) return ot::BlockSizeMode::SizeAutomaticWithMin;
	else if (_string == ot::toString(ot::BlockSizeMode::SizeAutomaticWithMax)) return ot::BlockSizeMode::SizeAutomaticWithMax;
	else if (_string == ot::toString(ot::BlockSizeMode::SizeAutomaticWithMinMax)) return ot::BlockSizeMode::SizeAutomaticWithMinMax;
	else {
		//todo: replace with otAssert
		assert(0);
		return ot::BlockSizeMode::SizeFixed;
	}
}