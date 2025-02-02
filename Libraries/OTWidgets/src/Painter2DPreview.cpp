//! @file Painter2DPreview.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2D.h"
#include "OTGui/Painter2D.h"
#include "OTWidgets/QtFactory.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/Painter2DPreview.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtCore/qrect.h>

ot::Painter2DPreview::Painter2DPreview(const ot::Painter2D* _painter)
	: m_maintainAspectRatio(false)
{
	this->setMinimumSize(10, 10);
	this->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
	if (_painter) this->setFromPainter(_painter);
	this->connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &Painter2DPreview::slotGlobalStyleChanged);
}

ot::Painter2DPreview::~Painter2DPreview() {
	this->disconnect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &Painter2DPreview::slotGlobalStyleChanged);
}

void ot::Painter2DPreview::setFromPainter(const Painter2D* _painter) {
	OTAssertNullptr(_painter);
	m_brush = QtFactory::toQBrush(_painter);
	this->update();
}

void ot::Painter2DPreview::paintEvent(QPaintEvent* _event) {
	Q_UNUSED(_event);

	QFrame::paintEvent(_event);

	if (!this->width() || !this->height()) return;

	QRect r;
	if (m_maintainAspectRatio) {
		int mi = std::min(this->rect().width(), this->rect().height());
		r = QRect(
			QPoint(
				(this->rect().topLeft().x() + (this->rect().width() / 2)) - (mi / 2), 
				(this->rect().topLeft().y() + (this->rect().height() / 2)) - (mi / 2)
			), 
			QSize(mi, mi)
		);
	}
	else {
		r = this->rect();
	}
	
	QPainter painter(this);
	painter.fillRect(QRect(QPoint(r.x() + 1, r.y() + 1), QSize(r.width() - 2, r.height() - 2)), m_brush);
	
	QPen pen;
	pen.setBrush(GlobalColorStyle::instance().getCurrentStyle().getValue(ColorStyleValueEntry::Border).brush());
	pen.setWidth(1);
	painter.setPen(pen);
	painter.drawRect(r);
}

void ot::Painter2DPreview::slotGlobalStyleChanged(void) {
	this->update();
}
