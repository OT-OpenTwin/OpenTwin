// @otlicense

// OpenTwin header
#include "OTWidgets/Widgets/InteractiveLabel.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qevent.h>

ot::InteractiveLabel::InteractiveLabel(QWidget* _parentWidget) 
	: Label(_parentWidget), m_mouseIsPressed(false)
{}

ot::InteractiveLabel::InteractiveLabel(const QString& _text, QWidget* _parentWidget)
	: Label(_text, _parentWidget), m_mouseIsPressed(false)
{}

void ot::InteractiveLabel::mousePressEvent(QMouseEvent* _event)
{
	if ((_event->button() == Qt::LeftButton) && this->isEnabled() && this->rect().contains(_event->pos()))
	{
		m_mouseIsPressed = true;
	}
}

void ot::InteractiveLabel::mouseReleaseEvent(QMouseEvent* _event)
{
	if ((_event->button() == Qt::LeftButton))
	{
		if (m_mouseIsPressed && this->isEnabled() && this->rect().contains(_event->pos()))
		{
			Q_EMIT mouseClicked(_event->modifiers());
		}

		m_mouseIsPressed = false;
	}
}

void ot::InteractiveLabel::mouseDoubleClickEvent(QMouseEvent* _event)
{
	if (_event->button() == Qt::LeftButton)
	{
		Q_EMIT mouseDoubleClicked(_event->modifiers());
	}
}

void ot::InteractiveLabel::enterEvent(QEnterEvent* _event)
{
	OT_UNUSED(_event);
	Q_EMIT mouseEntered();
}

void ot::InteractiveLabel::leaveEvent(QEvent* _event)
{
	OT_UNUSED(_event);
	Q_EMIT mouseLeft();
}
