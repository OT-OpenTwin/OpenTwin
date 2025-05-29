//! @file VersionGraphManagerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTWidgets/VersionGraphManager.h"
#include "OTWidgets/VersionGraphManagerView.h"

ot::VersionGraphManagerView::VersionGraphManagerView(VersionGraphManager* _versionGraphManager)
	: WidgetView(WidgetViewBase::ViewVersion), m_versionGraphManager(_versionGraphManager)
{
	if (!m_versionGraphManager) {
		m_versionGraphManager = new VersionGraphManager;
	}

	this->addWidgetInterfaceToDock(m_versionGraphManager);
}

ot::VersionGraphManagerView::~VersionGraphManagerView() {}

QWidget* ot::VersionGraphManagerView::getViewWidget(void) {
	return m_versionGraphManager->getQWidget();
}
