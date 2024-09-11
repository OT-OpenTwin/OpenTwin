//! @file VersionGraphManagerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTWidgets/VersionGraphManagerView.h"

ot::VersionGraphManagerView::VersionGraphManagerView()
{
	this->addWidgetToDock(this->getGraph());
}

ot::VersionGraphManagerView::~VersionGraphManagerView() {}

QWidget* ot::VersionGraphManagerView::getViewWidget(void) {
	return this->getGraph();
}
