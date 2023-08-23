//! @file OTQtConverter.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/OTQtConverter.h"

QColor ot::OTQtConverter::toQt(const ot::Color& _color) {
	return QColor(_color.rInt(), _color.gInt(), _color.bInt(), _color.aInt());
}