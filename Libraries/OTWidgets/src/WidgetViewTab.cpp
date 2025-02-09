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

ot::WidgetViewTab::WidgetViewTab(ads::CDockWidget * _dockWidget)
	: ads::CDockWidgetTab(_dockWidget), m_isLocked(false)
{
	const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();

	m_closeButton = new ToolButton;
	m_closeButton->setHidden(true);
	m_closeButton->setObjectName("OT_ViewTabCloseButton");
	m_closeButton->setFixedSize(14, 14);
	m_closeButton->setIcon(QIcon(cs.getFile(ColorStyleFileEntry::WindowCloseIcon)));

	m_lockButton = new ToolButton;
	m_lockButton->setHidden(true);
	m_lockButton->setObjectName("OT_ViewTabLockButton");
	m_lockButton->setFixedSize(14, 14);
	m_lockButton->setIcon(QIcon(cs.getFile(ColorStyleFileEntry::ArrowDownIcon)));

	this->connect(m_closeButton, &ToolButton::clicked, this, &WidgetViewTab::slotClose);
	this->connect(m_lockButton, &ToolButton::clicked, this, &WidgetViewTab::slotToggleLocked);
}

ot::WidgetViewTab::~WidgetViewTab() {

}

void ot::WidgetViewTab::setCloseButtonVisible(bool _vis) {
	if (_vis == !m_closeButton->isHidden()) {
		return;
	}

	QBoxLayout* layout = dynamic_cast<QBoxLayout*>(this->layout());
	if (!layout) {
		OT_LOG_E("Unexpected layout");
		return;
	}

	int ix = std::max(layout->count(), layout->count() - 2);
	layout->insertWidget(ix++, m_closeButton, Qt::AlignVCenter);
	layout->insertSpacing(ix++, qRound(1.5 * layout->contentsMargins().left() / 2.0));

	m_closeButton->setHidden(!_vis);
}

void ot::WidgetViewTab::setLockButtonVisible(bool _vis) {
	if (_vis == !m_lockButton->isHidden()) {
		return;
	}

	QBoxLayout* layout = dynamic_cast<QBoxLayout*>(this->layout());
	if (!layout) {
		OT_LOG_E("Unexpected layout");
		return;
	}

	int ix = std::max(layout->count(), layout->count() - 2);
	if (!m_lockButton->isHidden()) {
		ix -= 2;
	}
	layout->insertWidget(ix++, m_lockButton, Qt::AlignVCenter);
	layout->insertSpacing(ix++, qRound(1.5 * layout->contentsMargins().left() / 2.0));

	m_lockButton->setHidden(!_vis);
}

void ot::WidgetViewTab::slotClose(void) {
	Q_EMIT viewCloseRequested();
}

void ot::WidgetViewTab::slotToggleLocked(void) {
	m_isLocked = !m_isLocked;
	Q_EMIT viewLockedChanged(m_isLocked);
}
