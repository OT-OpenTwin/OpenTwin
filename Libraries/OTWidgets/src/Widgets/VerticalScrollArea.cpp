// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Widgets/VerticalScrollArea.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtWidgets/qscrollbar.h>

ot::VerticalScrollArea::VerticalScrollArea(QWidget* _parent) : QScrollArea(_parent)
{
	this->setWidgetResizable(true);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

bool ot::VerticalScrollArea::eventFilter(QObject* _watched, QEvent* _event)
{
	if (_watched && _watched == widget() && _event->type() == QEvent::Resize)
	{
		OTAssertNullptr(verticalScrollBar());
		setMinimumWidth(widget()->minimumSizeHint().width() + verticalScrollBar()->sizeHint().width());
	}

	return QScrollArea::eventFilter(_watched, _event);
}
