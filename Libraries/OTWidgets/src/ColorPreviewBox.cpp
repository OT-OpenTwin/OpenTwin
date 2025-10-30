// @otlicense

// OpenTwin header
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/ColorPreviewBox.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>

ot::ColorPreviewBox::ColorPreviewBox(const ot::Color& _color, QWidget* _parent) : QFrame(_parent), m_color(QtFactory::toQColor(_color)) {
	this->setObjectName("OT_ColorPreviewBox");
	this->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

ot::ColorPreviewBox::ColorPreviewBox(const QColor& _color, QWidget* _parent) : QFrame(_parent), m_color(_color) {
	this->setObjectName("OT_ColorPreviewBox");
	this->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

ot::ColorPreviewBox::~ColorPreviewBox() {

}

void ot::ColorPreviewBox::setColor(const ot::Color& _color) {
	this->setColor(QtFactory::toQColor(_color));
}

void ot::ColorPreviewBox::setColor(const QColor& _color) {
	m_color = _color;
	this->update();
}

void ot::ColorPreviewBox::paintEvent(QPaintEvent* _event) {
	QPainter painter(this);
	QRect r = _event->rect();

	double m = ((double)std::min(r.width(), r.height())) / 2.;

	QRect r1(QPoint(((double)r.left() + ((double)r.width() / 2.)) - m, ((double)r.top() + ((double)r.height() / 2.)) - m), 
		QPoint(((double)r.right() - ((double)r.width() / 2.)) + m, ((double)r.bottom() - ((double)r.height() / 2.)) + m));

	const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();
	
	QRect r2(r1.topLeft() + QPoint(1, 1), r1.bottomRight() - QPoint(1, 1));
	painter.fillRect(r1, cs.getValue(ColorStyleValueEntry::Border).toBrush());
	painter.fillRect(r2, m_color);
}