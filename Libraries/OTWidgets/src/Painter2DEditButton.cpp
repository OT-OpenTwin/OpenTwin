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
#include <QtGui/qevent.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>

ot::Painter2DEditButton::Painter2DEditButton(Painter2D* _painter) : m_painter(_painter) {
	if (!m_painter) m_painter = new FillPainter2D;

	this->ini();
}

ot::Painter2DEditButton::Painter2DEditButton(const Painter2D* _painter) : m_painter(nullptr) {
	if (_painter) m_painter = _painter->createCopy();
	else m_painter = new FillPainter2D;

	this->ini();
}

ot::Painter2DEditButton::~Painter2DEditButton() {
	delete m_painter;
	delete m_rootWidget;
}

bool ot::Painter2DEditButton::eventFilter(QObject* _obj, QEvent* _event) {
	if (_event->type() == QEvent::Type::Resize) {
		this->updateText();
	}
	return false;
}

void ot::Painter2DEditButton::setPainter(Painter2D* _painter) {
	if (_painter == m_painter) return;
	OTAssertNullptr(_painter);
	if (m_painter) delete m_painter;
	m_painter = _painter;
	m_preview->setFromPainter(m_painter);
	m_preview->repaint();
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
		m_preview->repaint();
		Q_EMIT painter2DChanged();
	}
}

void ot::Painter2DEditButton::ini(void) {
	m_rootWidget = new QWidget;
	m_rootWidget->setContentsMargins(QMargins(0, 0, 0, 0));
	QHBoxLayout* rootLay = new QHBoxLayout(m_rootWidget);
	rootLay->setContentsMargins(QMargins(0, 0, 0, 0));
	m_btn = new PushButton;

	m_preview = new Painter2DPreview(m_painter);
	m_preview->setContentsMargins(QMargins(0, 0, 0, 0));
	m_preview->setMinimumHeight(24);	
	m_preview->setMinimumWidth(24);
	m_preview->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
	m_preview->setMaintainAspectRatio(true);
	rootLay->addWidget(m_preview);
	rootLay->addWidget(m_btn, 1);

	this->updateText();
	m_btn->installEventFilter(this);
	this->connect(m_btn, &PushButton::clicked, this, &Painter2DEditButton::slotClicked);
}

void ot::Painter2DEditButton::updateText(void) {
	OTAssertNullptr(m_painter);
	if (m_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_FillPainter2DCfg) {
		FillPainter2D* actualPainter = dynamic_cast<FillPainter2D*>(m_painter);
		OTAssertNullptr(actualPainter);
		m_btnTip = "Color { r: " + QString::number(actualPainter->color().rInt()) +
			"; g: " + QString::number(actualPainter->color().gInt()) +
			"; b: " + QString::number(actualPainter->color().bInt()) +
			"; a: " + QString::number(actualPainter->color().aInt()) + " }";

		m_btnText = "Fill";
		QFontMetrics metrics(m_btn->font());
		m_btn->setText(metrics.size(Qt::TextSingleLine, m_btnTip).width() < m_btn->width() ? m_btnTip : m_btnText);
		m_btn->setToolTip(m_btnTip);
	}
	else if (m_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg) {
		LinearGradientPainter2D* actualPainter = dynamic_cast<LinearGradientPainter2D*>(m_painter);
		OTAssertNullptr(actualPainter);
		m_btn->setText("Linear");
		
		m_btnText = "Linear";
		m_btnTip = "LinearGradient { X1: " + QString::number(actualPainter->start().x()) +
			"; Y1: " + QString::number(actualPainter->start().y()) +
			"; X2: " + QString::number(actualPainter->finalStop().x()) +
			"; Y2: " + QString::number(actualPainter->finalStop().y()) +
			"; Spread: " + QString::fromStdString(toString(actualPainter->spread())) +
			"; ";

		int ct = 0;
		for (const GradientPainterStop2D& s : actualPainter->stops()) {
			m_btnTip.append("Stop: " + QString::number(ct++) + " rgba(" + QString::number(s.color().rInt()) +
				", " + QString::number(s.color().gInt()) + ", " + QString::number(s.color().bInt()) +
				", " + QString::number(s.color().aInt()) + "); ");
		}
		QFontMetrics metrics(m_btn->font());
		m_btn->setText(metrics.size(0, m_btnTip).width() < m_btn->width() ? m_btnTip : m_btnText);
		m_btn->setToolTip(m_btnTip);
	}
	else if (m_painter->simpleFactoryObjectKey() == OT_SimpleFactoryJsonKeyValue_RadialGradientPainter2DCfg) {
		RadialGradientPainter2D* actualPainter = dynamic_cast<RadialGradientPainter2D*>(m_painter);
		OTAssertNullptr(actualPainter);
		m_btnText = "Radial";
		
		if (actualPainter->isFocalPointSet()) {
			m_btnTip = "RadialGradient { CX: " + QString::number(actualPainter->centerPoint().x()) +
				"; CY: " + QString::number(actualPainter->centerPoint().y()) +
				"; Radius: " + QString::number(actualPainter->centerRadius()) +
				"; FX: " + QString::number(actualPainter->focalPoint().x()) +
				"; FY: " + QString::number(actualPainter->focalPoint().y()) +
				"; FR: " + QString::number(actualPainter->focalRadius()) +
				"; Spread: " + QString::fromStdString(toString(actualPainter->spread())) +
				"; ";
		}
		else {
			m_btnTip = "RadialGradient { CX: " + QString::number(actualPainter->centerPoint().x()) +
				"; CY: " + QString::number(actualPainter->centerPoint().y()) +
				"; Radius: " + QString::number(actualPainter->centerRadius()) +
				"; Spread: " + QString::fromStdString(toString(actualPainter->spread())) +
				"; ";
		}

		int ct = 0;
		for (const GradientPainterStop2D& s : actualPainter->stops()) {
			m_btnTip.append("Stop: " + QString::number(ct++) + " rgba(" + QString::number(s.color().rInt()) +
				", " + QString::number(s.color().gInt()) + ", " + QString::number(s.color().bInt()) +
				", " + QString::number(s.color().aInt()) + "); ");
		}

		QFontMetrics metrics(m_btn->font());
		m_btn->setText(metrics.size(0, m_btnTip).width() < m_btn->width() ? m_btnTip : m_btnText);
		m_btn->setToolTip(m_btnTip);
	}
	else {
		OT_LOG_EA("Unknown painter type");
	}
}