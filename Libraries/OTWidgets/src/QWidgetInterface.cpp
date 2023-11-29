//! @file QWidgetInterface.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtWidgets/qwidget.h>

void ot::QWidgetInterface::centerOnParent(const QWidget* _parentWidget) {
	const QSize parentSize = _parentWidget->size();
	const QPoint parentCenter = _parentWidget->pos() + QPoint(parentSize.width() / 2, parentSize.height() / 2);
	const QSize size = this->getQWidget()->size();
	this->getQWidget()->move(QPoint(parentCenter.x() - (size.width() / 2), parentCenter.y() - (size.height() / 2)));
}
