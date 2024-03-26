//! @file Painter2DPreview.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2D.h"
#include "OTGui/Painter2D.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/Painter2DPreview.h"
#include "OTWidgets/Painter2DFactory.h"

// Qt header
#include <QtGui/qpainter.h>

ot::Painter2DPreview::Painter2DPreview(const ot::Painter2D* _painter) {
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
	m_brush = Painter2DFactory::brushFromPainter2D(_painter);
}

void ot::Painter2DPreview::paintEvent(QPaintEvent* _event) {
	Q_UNUSED(_event);
	QPainter painter(this);
	painter.fillRect(this->rect(), m_brush);

	QPen pen;
	pen.setBrush(GlobalColorStyle::instance().getCurrentStyle().getValue(OT_COLORSTYLE_VALUE_ControlsBorderColor).brush());
	pen.setWidth(1);
	painter.setPen(pen);
	painter.drawRect(this->rect());
}

void ot::Painter2DPreview::slotGlobalStyleChanged(const ColorStyle& _style) {
	this->update();
}
