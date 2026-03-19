// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Plot/PlotLegend.h"
#include "OTWidgets/Plot/PlotDataset.h"
#include "OTWidgets/Plot/PlotLegendItem.h"
#include "OTWidgets/Plot/PlotLegendItemWidget.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>

ot::PlotLegendItem::PlotLegendItem(PlotDataset* _dataset)
	: m_legend(nullptr), m_dataset(_dataset)
{
	m_selectedWidget = new PlotLegendItemWidget;
	m_dimmedWidget = new PlotLegendItemWidget;
	m_dimmedWidget->getQWidget()->setVisible(false);
}

ot::PlotLegendItem::~PlotLegendItem()
{
	if (m_legend)
	{
		m_legend->removeItem(this);
	}

	delete m_selectedWidget;
	m_selectedWidget = nullptr;

	delete m_dimmedWidget;
	m_dimmedWidget = nullptr;
}

QWidget* ot::PlotLegendItem::getSelectedWidget() const
{
	OTAssertNullptr(m_selectedWidget);
	return m_selectedWidget->getQWidget();
}

QWidget* ot::PlotLegendItem::getDimmedWidget() const
{
	OTAssertNullptr(m_dimmedWidget);
	return m_dimmedWidget->getQWidget();
}

void ot::PlotLegendItem::updateVisibility()
{
	OTAssertNullptr(m_dataset);
	bool selected = m_dataset->isSelected();

	OTAssertNullptr(m_selectedWidget);
	if (selected == m_selectedWidget->getQWidget()->isVisible())
	{
		return;
	}

	m_selectedWidget->getQWidget()->setVisible(selected);

	OTAssertNullptr(m_dimmedWidget);
	m_dimmedWidget->getQWidget()->setVisible(!selected);
}

void ot::PlotLegendItem::setLabel(const QString& _text)
{
	OTAssertNullptr(m_selectedWidget);
	m_selectedWidget->setLabel(_text);

	OTAssertNullptr(m_dimmedWidget);
	m_dimmedWidget->setLabel(_text);
}

void ot::PlotLegendItem::setSelectedPainter(const ot::Painter2D* _painter)
{
	OTAssertNullptr(m_selectedWidget);
	m_selectedWidget->setPainter(_painter);
}

void ot::PlotLegendItem::setDimmedPainter(const ot::Painter2D* _painter)
{
	OTAssertNullptr(m_dimmedWidget);
	m_dimmedWidget->setPainter(_painter);
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
