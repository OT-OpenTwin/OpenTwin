// @otlicense

// OpenTwin header
#include "OTWidgets/Plot/PlotLegendItemWidget.h"
#include "OTWidgets/Widgets/Painter2DPreview.h"
#include "OTWidgets/Widgets/InteractiveLabel.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>

ot::PlotLegendItemWidget::PlotLegendItemWidget()
{
	m_rootWidget = new QWidget;
	m_rootWidget->setContentsMargins(0, 0, 0, 0);
	QHBoxLayout* layout = new QHBoxLayout(m_rootWidget);
	layout->setContentsMargins(0, 0, 0, 0);

	m_colorPreview = new Painter2DPreview(m_rootWidget);
	m_colorPreview->setFixedSize(10, 10);
	layout->addWidget(m_colorPreview, 0);

	m_label = new InteractiveLabel(m_rootWidget);
	layout->addWidget(m_label, 1);

	connect(m_label, &InteractiveLabel::mouseEntered, this, &PlotLegendItemWidget::slotMouseEntered);
	connect(m_label, &InteractiveLabel::mouseLeft, this, &PlotLegendItemWidget::slotMouseLeft);
	connect(m_label, &InteractiveLabel::mouseDoubleClicked, this, &PlotLegendItemWidget::slotMouseClicked);
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

void ot::PlotLegendItemWidget::slotMouseEntered()
{
	Q_EMIT hovered();
}

void ot::PlotLegendItemWidget::slotMouseLeft()
{
	Q_EMIT unhovered();
}

void ot::PlotLegendItemWidget::slotMouseClicked(Qt::KeyboardModifiers _modifiers)
{
	Q_EMIT clicked(_modifiers);
}
