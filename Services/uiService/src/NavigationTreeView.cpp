// @otlicense

// OpenTwin header
#include "NavigationTreeView.h"

// uiCore header
#include "akWidgets/aTreeWidget.h"

// Qt header
#include <QtWidgets/qshortcut.h>

ot::NavigationTreeView::NavigationTreeView()
	: WidgetView(WidgetViewBase::ViewNavigation)
{
	m_tree = new ak::aTreeWidget;
	this->addWidgetInterfaceToDock(m_tree);

	QShortcut* projectNavCopy = new QShortcut(QKeySequence("Ctrl+C"), m_tree->getQWidget());
	projectNavCopy->setContext(Qt::WidgetWithChildrenShortcut);
	this->connect(projectNavCopy, &QShortcut::activated, this, &NavigationTreeView::slotCopyRequested);
	QShortcut* projectNavPaste = new QShortcut(QKeySequence("Ctrl+P"), m_tree->getQWidget());
	projectNavPaste->setContext(Qt::WidgetWithChildrenShortcut);
	this->connect(projectNavPaste, &QShortcut::activated, this, &NavigationTreeView::slotPasteRequested);
}

ot::NavigationTreeView::~NavigationTreeView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::NavigationTreeView::getViewWidget(void) {
	return m_tree->widget();
}

void ot::NavigationTreeView::slotCopyRequested(void) {
	CopyInformation info;
	info.setOriginViewInfo(this->getViewData());

	Q_EMIT copyRequested(info);
}

void ot::NavigationTreeView::slotPasteRequested(void) {
	CopyInformation info;
	info.setDestinationViewInfo(this->getViewData());

	Q_EMIT pasteRequested(info);
}
