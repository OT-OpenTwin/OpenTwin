// @otlicense

// OpenTwin header
#include "OTWidgets/OnOffSlider.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtCore/qpropertyanimation.h>
#include <QtGui/qpainter.h>

ot::OnOffSlider::OnOffSlider(QWidget* _parent) :
	QWidget(_parent), m_checked(false), m_showLabels(true), m_knobPos(0.0)
{
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	
	m_animation = new QPropertyAnimation(this, "knobPos", this);
	m_animation->setDuration(150);
	m_animation->setEasingCurve(QEasingCurve::InOutQuad);
}

void ot::OnOffSlider::setChecked(bool _checked) {
	if (m_checked != _checked) {
		m_checked = _checked;
		startAnimation();
		Q_EMIT toggled(m_checked);
	}
}

void ot::OnOffSlider::setShowLabels(bool _on) {
	if (m_showLabels != _on) {
		m_showLabels = _on;
		update();
	}
}

void ot::OnOffSlider::toggle() {
	setChecked(!m_checked);
}

void ot::OnOffSlider::mousePressEvent(QMouseEvent* _event) {
	setChecked(!m_checked);
}

void ot::OnOffSlider::paintEvent(QPaintEvent* _event) {
	OT_UNUSED(_event);

	const ColorStyle& style = GlobalColorStyle::instance().getCurrentStyle();

	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing, true);

	// Dimensions
	const qreal w = width();
	const qreal h = height();
	const qreal radius = h / 2.;
	const qreal margin = h * 0.08;

	const QRectF trackRect(0, 0, w, h);

	const qreal knobDiameter = h - 2 * margin;
	const qreal knobXMin = margin;
	const qreal knobXMax = w - knobDiameter - margin;
	const qreal knobX = knobXMin + (knobXMax - knobXMin) * m_knobPos;

	const QRectF knobRect(knobX, margin, knobDiameter, knobDiameter);

	style.getValue(ColorStyleValueEntry::WidgetBackground).toBrush();

	// Track background
	QBrush trackBrush = m_checked ? style.getValue(ColorStyleValueEntry::SuccessForeground).toBrush() : style.getValue(ColorStyleValueEntry::ErrorForeground).toBrush();

	p.setBrush(trackBrush);
	QPen borderPen(style.getValue(ColorStyleValueEntry::Border).toBrush(), 1.);
	p.setPen(borderPen);
	p.drawRoundedRect(trackRect, radius, radius);

	// Optional: Labels
	if (m_showLabels) {
		QFont f = font();
		f.setBold(true);
		f.setPixelSize(int(h * 0.45));
		
		p.setFont(f);

		QRectF leftRect(0, 0, w * 0.5, h);
		QRectF rightRect(w * 0.5, 0, w * 0.5, h);
		p.drawText(leftRect, Qt::AlignCenter, "O");
		p.drawText(rightRect, Qt::AlignCenter, "I");
	}

	QBrush knobBrush;

	if (underMouse()) {
		knobBrush = style.getValue(ColorStyleValueEntry::WidgetHoverBackground).toBrush();
	}
	else {
		knobBrush = style.getValue(ColorStyleValueEntry::InputBackground).toBrush();
	}
	
	// Knob
	p.setBrush(knobBrush);
	p.drawEllipse(knobRect);
}

void ot::OnOffSlider::startAnimation() {
	m_animation->stop();
	m_animation->setStartValue(m_knobPos);
	m_animation->setEndValue(m_checked ? 1.0 : 0.0);
	m_animation->start();
}

void ot::OnOffSlider::setKnobPos(qreal _pos) {
	if (m_knobPos != _pos) {
		m_knobPos = _pos;
		update();
	}
}
