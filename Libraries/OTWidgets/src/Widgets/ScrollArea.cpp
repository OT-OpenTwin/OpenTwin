// @otlicense

// OpenTwin header
#include "OTWidgets/Widgets/ScrollArea.h"

ot::ScrollArea::ScrollArea(QWidget* _parent) : QScrollArea(_parent)
{
	this->setWidgetResizable(true);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

bool ot::ScrollArea::eventFilter(QObject* _watched, QEvent* _event)
{
	return QScrollArea::eventFilter(_watched, _event);
}
