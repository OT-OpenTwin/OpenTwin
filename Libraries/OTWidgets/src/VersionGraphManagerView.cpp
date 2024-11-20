//! @file VersionGraphManagerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTWidgets/VersionGraphManagerView.h"

ot::VersionGraphManagerView::VersionGraphManagerView()
	: WidgetView(WidgetViewBase::ViewVersion)
{
	this->addWidgetToDock(this->getQWidget());
}

ot::VersionGraphManagerView::~VersionGraphManagerView() {}

QWidget* ot::VersionGraphManagerView::getViewWidget(void) {
	return this->getQWidget();
}
