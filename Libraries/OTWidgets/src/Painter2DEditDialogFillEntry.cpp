// @otlicense

//! @file Painter2DEditDialogFillEntry.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/FillPainter2D.h"
#include "OTWidgets/ColorPickButton.h"
#include "OTWidgets/Painter2DEditDialogFillEntry.h"

ot::Painter2DEditDialogFillEntry::Painter2DEditDialogFillEntry(const Painter2D* _painter) {
	m_btn = new ColorPickButton;
	if (_painter) {
		const FillPainter2D* actualPainter = dynamic_cast<const FillPainter2D*>(_painter);
		OTAssertNullptr(actualPainter);
		m_btn->setColor(actualPainter->getColor());
	}
	this->connect(m_btn, &ColorPickButton::colorChanged, this, &Painter2DEditDialogEntry::slotValueChanged);
}

ot::Painter2DEditDialogFillEntry::~Painter2DEditDialogFillEntry() {
	delete m_btn;
}

QWidget* ot::Painter2DEditDialogFillEntry::getRootWidget() const {
	return m_btn;
}

ot::Painter2D* ot::Painter2DEditDialogFillEntry::createPainter() const {
	return new FillPainter2D(m_btn->otColor());
}

