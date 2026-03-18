// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Plot/PlotLegend.h"
#include "OTWidgets/Plot/PlotLegendItem.h"
#include "OTWidgets/Widgets/Label.h"
#include "OTWidgets/Widgets/Painter2DPreview.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>

ot::PlotLegendItem::PlotLegendItem() 
	: m_legend(nullptr)
{
	m_itemWidget = new QWidget;
	QHBoxLayout* layout = new QHBoxLayout(m_itemWidget);

	m_colorPreview = new Painter2DPreview(nullptr);
	layout->addWidget(m_colorPreview);
	m_colorPreview->setFixedSize(10, 10);

	m_label = new Label(nullptr);
	layout->addWidget(m_label, 1);
}

ot::PlotLegendItem::~PlotLegendItem()
{
	if (m_legend)
	{
		m_legend->removeItem(this);
	}
}

void ot::PlotLegendItem::setLabel(const QString& _text)
{
	m_label->setText(_text);
}

void ot::PlotLegendItem::setPainter(const ot::Painter2D* _painter)
{
	m_colorPreview->setFromPainter(_painter);
}

void ot::PlotLegendItem::attach()
{
	if (m_legend)
	{
		m_legend->addItem(this);
	}
}

void ot::PlotLegendItem::detach()
{
	if (m_legend)
	{
		m_legend->removeItem(this);
	}
}