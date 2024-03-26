//! @file Painter2DEditButton.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/FillPainter2D.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/Painter2DPreview.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTGui/RadialGradientPainter2D.h"
#include "OTWidgets/Painter2DEditButton.h"
#include "OTWidgets/Painter2DEditDialog.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>

ot::Painter2DEditButton::Painter2DEditButton(Painter2D* _painter) : m_painter(_painter) {
	m_rootWidget = new QWidget;
	m_rootWidget->setContentsMargins(QMargins(0, 0, 0, 0));
	QHBoxLayout* rootLay = new QHBoxLayout(m_rootWidget);
	rootLay->setContentsMargins(QMargins(0, 0, 0, 0));
	m_btn = new PushButton;

	if (!m_painter) m_painter = new FillPainter2D;

	m_preview = new Painter2DPreview(m_painter);
	m_preview->setMinimumHeight(16);
	m_preview->setMinimumWidth(16);
	rootLay->addWidget(m_preview);
	rootLay->addWidget(m_btn, 1);

	this->updateText();
	this->connect(m_btn, &PushButton::clicked, this, &Painter2DEditButton::slotClicked);
}

ot::Painter2DEditButton::Painter2DEditButton(const Painter2D* _painter) : m_painter(nullptr) {
	m_rootWidget = new QWidget;
	m_rootWidget->setContentsMargins(QMargins(0, 0, 0, 0));
	QHBoxLayout* rootLay = new QHBoxLayout(m_rootWidget);
	rootLay->setContentsMargins(QMargins(0, 0, 0, 0));
	m_btn = new PushButton;

	if (_painter) m_painter = _painter->createCopy();
	else m_painter = new FillPainter2D;

	m_preview = new Painter2DPreview(m_painter);
	m_preview->setMinimumHeight(12);
	m_preview->setMinimumWidth(12);
	rootLay->addWidget(m_preview);
	rootLay->addWidget(m_btn, 1);

	this->updateText();
	this->connect(m_btn, &PushButton::clicked, this, &Painter2DEditButton::slotClicked);
}

ot::Painter2DEditButton::~Painter2DEditButton() {
	delete m_painter;
	delete m_rootWidget;
}

void ot::Painter2DEditButton::setPainter(Painter2D* _painter) {
	if (_painter == m_painter) return;
	OTAssertNullptr(_painter);
	if (m_painter) delete m_painter;
	m_painter = _painter;
	this->updateText();
}

void ot::Painter2DEditButton::setPainter(const Painter2D* _painter) {
	if (_painter == m_painter) return;
	OTAssertNullptr(_painter);
	if (m_painter) delete m_painter;
	m_painter = _painter->createCopy();
	this->updateText();
}

void ot::Painter2DEditButton::slotClicked(void) {
	Painter2DEditDialog dia(m_painter);
	if (dia.showDialog() == Dialog::Ok) {
		if (m_painter) delete m_painter;
		m_painter = dia.createPainter();
		m_preview->setFromPainter(m_painter);
		this->updateText();
		Q_EMIT painter2DChanged();
	}
}

void ot::Painter2DEditButton::updateText(void) {
	OTAssertNullptr(m_painter);
	if (m_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_FillPainter2DCfg) {
		FillPainter2D* actualPainter = dynamic_cast<FillPainter2D*>(m_painter);
		OTAssertNullptr(actualPainter);
		m_btn->setText("Fill");
		m_btn->setToolTip("Color { r: " + QString::number(actualPainter->color().rInt()) +
			"; g: " + QString::number(actualPainter->color().gInt()) +
			"; b: " + QString::number(actualPainter->color().bInt()) +
			"; a: " + QString::number(actualPainter->color().aInt()) + " }"
		);
	}
	else if (m_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg) {
		LinearGradientPainter2D* actualPainter = dynamic_cast<LinearGradientPainter2D*>(m_painter);
		OTAssertNullptr(actualPainter);
		m_btn->setText("Linear");
		QString tip;

		tip = "LinearGradient { X1: " + QString::number(actualPainter->start().x()) +
			"; Y1: " + QString::number(actualPainter->start().y()) +
			"; X2: " + QString::number(actualPainter->finalStop().x()) +
			"; Y2: " + QString::number(actualPainter->finalStop().y()) +
			"; Spread: " + QString::fromStdString(toString(actualPainter->spread())) +
			"; ";

		int ct = 0;
		for (const GradientPainterStop2D& s : actualPainter->stops()) {
			tip.append("Stop: " + QString::number(ct++) + " rgba(" + QString::number(s.color().rInt()) +
				", " + QString::number(s.color().gInt()) + ", " + QString::number(s.color().bInt()) +
				", " + QString::number(s.color().aInt()) + "); ");
		}
		m_btn->setToolTip(tip);
	}
	else if (m_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_RadialGradientPainter2DCfg) {
		RadialGradientPainter2D* actualPainter = dynamic_cast<RadialGradientPainter2D*>(m_painter);
		OTAssertNullptr(actualPainter);
		m_btn->setText("Radial");
		QString tip;

		if (actualPainter->isFocalPointSet()) {
			tip = "RadialGradient { CX: " + QString::number(actualPainter->centerPoint().x()) +
				"; CY: " + QString::number(actualPainter->centerPoint().y()) +
				"; Radius: " + QString::number(actualPainter->centerRadius()) +
				"; FX: " + QString::number(actualPainter->focalPoint().x()) +
				"; FY: " + QString::number(actualPainter->focalPoint().y()) +
				"; FR: " + QString::number(actualPainter->focalRadius()) +
				"; Spread: " + QString::fromStdString(toString(actualPainter->spread())) +
				"; ";
		}
		else {
			tip = "RadialGradient { CX: " + QString::number(actualPainter->centerPoint().x()) +
				"; CY: " + QString::number(actualPainter->centerPoint().y()) +
				"; Radius: " + QString::number(actualPainter->centerRadius()) +
				"; Spread: " + QString::fromStdString(toString(actualPainter->spread())) +
				"; ";
		}

		int ct = 0;
		for (const GradientPainterStop2D& s : actualPainter->stops()) {
			tip.append("Stop: " + QString::number(ct++) + " rgba(" + QString::number(s.color().rInt()) +
				", " + QString::number(s.color().gInt()) + ", " + QString::number(s.color().bInt()) +
				", " + QString::number(s.color().aInt()) + "); ");
		}
		m_btn->setToolTip(tip);
	}
	else {
		OT_LOG_EA("Unknown painter type");
	}
}