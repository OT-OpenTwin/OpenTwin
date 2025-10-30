// @otlicense

// OpenTwin header
#include "OTWidgets/SelectionData.h"

ot::SelectionData::SelectionData() : m_keyboardModifiers(Qt::NoModifier), m_selectionOrigin(SelectionOrigin::Custom), m_viewHandlingFlags(ViewHandlingFlag::NoFlags) {

}

ot::SelectionData::~SelectionData() {

}
