// @otlicense

//! @file SelectionData.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/SelectionData.h"

ot::SelectionData::SelectionData() : m_keyboardModifiers(Qt::NoModifier), m_selectionOrigin(SelectionOrigin::Custom), m_viewHandlingFlags(ViewHandlingFlag::NoFlags) {

}

ot::SelectionData::~SelectionData() {

}
