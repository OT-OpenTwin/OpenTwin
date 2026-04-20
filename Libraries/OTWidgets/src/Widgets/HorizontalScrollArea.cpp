// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Widgets/HorizontalScrollArea.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtWidgets/qscrollbar.h>

ot::HorizontalScrollArea::HorizontalScrollArea(QWidget* _parent) : ScrollArea(_parent)
{
	this->setWidgetResizable(true);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

bool ot::HorizontalScrollArea::eventFilter(QObject* _watched, QEvent* _event)
{
	if (_watched && _watched == widget() && _event->type() == QEvent::Resize)
	{
		OTAssertNullptr(horizontalScrollBar());
		setMinimumHeight(widget()->minimumSizeHint().height() + horizontalScrollBar()->sizeHint().height());
	}

	return ScrollArea::eventFilter(_watched, _event);
}
