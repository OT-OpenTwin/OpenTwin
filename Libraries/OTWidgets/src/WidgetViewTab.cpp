//! @file WidgetViewTab.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/Logger.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/WidgetViewTab.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtWidgets/qlayout.h>

ot::WidgetViewTab::WidgetViewTab(ads::CDockWidget * _dockWidget)
	: ads::CDockWidgetTab(_dockWidget), m_isPinned(false)
{
	const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();

	m_closeButton = new ToolButton;
	m_closeButton->setHidden(true);
	m_closeButton->setObjectName("OT_ViewTabCloseButton");
	m_closeButton->setFixedSize(14, 14);
	
	m_pinButton = new ToolButton;
	m_pinButton->setHidden(true);
	m_pinButton->setObjectName("OT_ViewTabPinButton");
	m_pinButton->setFixedSize(14, 14);
	m_pinButton->setProperty("IsPinned", m_isPinned);

	this->connect(m_closeButton, &ToolButton::clicked, this, &WidgetViewTab::slotClose);
	this->connect(m_pinButton, &ToolButton::clicked, this, &WidgetViewTab::slotTogglePinned);
}

ot::WidgetViewTab::~WidgetViewTab() {
	
}

void ot::WidgetViewTab::setIsPinned(bool _pinned) {
	// Ensure pinned changed
	if (m_isPinned == _pinned) {
		return;
	}

	// Update pinned state
	m_isPinned = _pinned;
	m_pinButton->setProperty("IsPinned", m_isPinned);

	// Reapply style
	m_pinButton->style()->unpolish(m_pinButton);
	m_pinButton->style()->polish(m_pinButton);
	m_pinButton->update();

	// Emit signal
	Q_EMIT viewPinnedChanged(m_isPinned);
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

	m_closeButton->setVisible(_vis);
}

void ot::WidgetViewTab::setPinButtonVisible(bool _vis) {
	if (_vis == !m_pinButton->isHidden()) {
		return;
	}

	QBoxLayout* layout = dynamic_cast<QBoxLayout*>(this->layout());
	if (!layout) {
		OT_LOG_E("Unexpected layout");
		return;
	}

	int ix = std::max(layout->count(), layout->count() - 2);
	if (!m_closeButton->isHidden()) {
		ix -= 2;
	}
	layout->insertWidget(ix++, m_pinButton, Qt::AlignVCenter);
	layout->insertSpacing(ix++, qRound(1.5 * layout->contentsMargins().left() / 2.0));

	m_pinButton->setVisible(_vis);
}

void ot::WidgetViewTab::slotClose(void) {
	Q_EMIT viewCloseRequested();
}

void ot::WidgetViewTab::slotTogglePinned(void) {
	this->setIsPinned(!m_isPinned);
}

void ot::WidgetViewTab::mousePressEvent(QMouseEvent* _event) {
	ads::CDockWidgetTab::mousePressEvent(_event);
	Q_EMIT tabPressed();
}
