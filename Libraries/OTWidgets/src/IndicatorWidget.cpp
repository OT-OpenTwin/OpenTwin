//! @file IndicatorWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/IndicatorWidget.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qpainterpath.h>
#include <QtWidgets/qstyle.h>
#include <QtWidgets/qstyleoption.h>

ot::IndicatorWidget::IndicatorWidget(QWidget* _parent) : IndicatorWidget(false, _parent) {}

ot::IndicatorWidget::IndicatorWidget(bool _checked, QWidget* _parent)
	: QAbstractButton(_parent), WidgetBase(), 
	m_checkedSymbol(Checkmark), m_uncheckedSymbol(Cross), m_drawFrame(true)
{
	this->setCheckable(true);
	this->setChecked(_checked);
	this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	this->setFixedSize(this->sizeHint());
    setMouseTracking(true);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Vitual methods

QSize ot::IndicatorWidget::sizeHint() const {
	return QSize(16, 16);
}

QSize ot::IndicatorWidget::minimumSizeHint() const {
	return this->sizeHint();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::IndicatorWidget::setCheckedSymbol(Symbol _symbol) {
	if (m_checkedSymbol == _symbol) {
		return;
	}
	m_checkedSymbol = _symbol;
	this->update();
}

void ot::IndicatorWidget::setUncheckedSymbol(Symbol _symbol) {
	if (m_uncheckedSymbol == _symbol) {
		return;
	}
	m_uncheckedSymbol = _symbol;
	this->update();
}

void ot::IndicatorWidget::setDrawFrame(bool enable) {
	if (m_drawFrame == enable) {
		return;
	}
	m_drawFrame = enable;
	this->update();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Overridden methods

void ot::IndicatorWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

	// Draw frame if required
    if (m_drawFrame) {
        QStyleOptionFrame opt;
        opt.initFrom(this);
        opt.rect = rect().adjusted(0, 0, -1, -1);
        this->style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, this);
    }
    
    // Get widget rect with margins
    QRectF r = rect().adjusted(4, 4, -4, -4);

    // Get pen/brush from stylesheet
	QStyleOptionButton option;
	option.initFrom(this);
    
    QColor col = option.palette.color(QPalette::ButtonText);

    QPen pen(col, 2);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);

	Symbol symbol = (this->isChecked() ? m_checkedSymbol : m_uncheckedSymbol);

    switch (symbol) {
    case None:
        break;

    case Checkmark:
    {
        QPainterPath path(QPointF(r.left(), r.center().y()));
		path.lineTo(QPointF(r.center().x() - (r.width() / 6), r.bottom()));
		path.lineTo(QPointF(r.right(), r.top()));
		p.drawPath(path);
        break;
    }

    case Cross:
    {
        p.drawLine(r.topLeft(), r.bottomRight());
        p.drawLine(r.topRight(), r.bottomLeft());
        break;
    }

    case Circle:
    {
        p.drawEllipse(r);
        break;
    }

    default:
		OT_LOG_E("Unknown symbol type (" + std::to_string(static_cast<int>(symbol)) + ")");
        break;
    }
}

QStyle::State ot::IndicatorWidget::getButtonState() const {
    QStyle::State state = QStyle::State_None;

    if (this->isEnabled()) {
        state |= QStyle::State_Enabled;
    }
    if (this->isChecked()) {
        state |= QStyle::State_On;
    }
    else {
        state |= QStyle::State_Off;
    }
    if (this->isDown()) {
        state |= QStyle::State_Sunken;
    }
    if (this->underMouse()) {
        state |= QStyle::State_MouseOver;
    }
    if (this->hasFocus()) {
        state |= QStyle::State_HasFocus;
    }

    return state;
}
