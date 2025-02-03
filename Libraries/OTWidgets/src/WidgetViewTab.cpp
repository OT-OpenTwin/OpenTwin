//! @file WidgetViewTab.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/WidgetViewTab.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtWidgets/qlayout.h>

ot::WidgetViewTab::WidgetViewTab(ads::CDockWidget* _dockWidget)
	: ads::CDockWidgetTab(_dockWidget)
{
	QBoxLayout* layout = dynamic_cast<QBoxLayout*>(this->layout());
	if (!layout) {
		OT_LOG_E("Unexpected layout");
		return;
	}

	m_pinButton = new ToolButton;
	m_pinButton->setObjectName("OT_ViewTabPinButton");

	m_lockButton = new ToolButton;
	m_lockButton->setObjectName("OT_ViewTabLockButton");

	int ix = std::max(layout->count(), layout->count() - 2);
	layout->insertWidget(ix++, m_pinButton, Qt::AlignVCenter);
	layout->insertSpacing(ix++, qRound(1.5 * layout->contentsMargins().left() / 2.0));
	layout->insertWidget(ix++, m_lockButton, Qt::AlignVCenter);
	layout->insertSpacing(ix++, qRound(1.5 * layout->contentsMargins().left() / 2.0));
}

ot::WidgetViewTab::~WidgetViewTab() {

}
