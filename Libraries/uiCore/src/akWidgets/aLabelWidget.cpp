// @otlicense

// AK header
#include <akWidgets/aLabelWidget.h>

#include <qevent.h>

ak::aLabelWidget::aLabelWidget(QWidget * _parent)
	: QLabel(_parent), aWidget(otLabel)
{
}

ak::aLabelWidget::aLabelWidget(const QString & _text, QWidget * _parent)
	: QLabel(_text, _parent), aWidget(otLabel)
{}

ak::aLabelWidget::~aLabelWidget() { A_OBJECT_DESTROYING }

QWidget * ak::aLabelWidget::widget(void) { return this; }

bool ak::aLabelWidget::event(QEvent *myEvent)  
{
	switch(myEvent->type())
	{        
	case(QEvent :: MouseButtonRelease):   // Identify Mouse press Event
		Q_EMIT clicked();
		break;
	}
	return QWidget::event(myEvent);
}
