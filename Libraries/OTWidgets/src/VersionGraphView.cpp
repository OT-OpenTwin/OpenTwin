//! @file VersionGraphView.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTWidgets/VersionGraphView.h"

ot::VersionGraphView::VersionGraphView()
{
	this->addWidgetToDock(this);
}

ot::VersionGraphView::~VersionGraphView() {}