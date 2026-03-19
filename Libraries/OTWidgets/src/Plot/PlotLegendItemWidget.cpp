// @otlicense

// OpenTwin header
#include "OTWidgets/Plot/PlotLegendItemWidget.h"
#include "OTWidgets/Widgets/Label.h"
#include "OTWidgets/Widgets/Painter2DPreview.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>

ot::PlotLegendItemWidget::PlotLegendItemWidget()
{
	m_rootWidget = new QWidget;
	QHBoxLayout* layout = new QHBoxLayout(m_rootWidget);

	m_colorPreview = new Painter2DPreview(m_rootWidget);
	m_colorPreview->setFixedSize(10, 10);
	layout->addWidget(m_colorPreview, 0);

	m_label = new Label(m_rootWidget);
	layout->addWidget(m_label, 1);
}

ot::PlotLegendItemWidget::~PlotLegendItemWidget()
{
	OTAssertNullptr(m_rootWidget);
	delete m_rootWidget;
}

void ot::PlotLegendItemWidget::setLabel(const QString& _text)
{
	m_label->setText(_text);
}

void ot::PlotLegendItemWidget::setPainter(const ot::Painter2D* _painter)
{
	m_colorPreview->setFromPainter(_painter);
}