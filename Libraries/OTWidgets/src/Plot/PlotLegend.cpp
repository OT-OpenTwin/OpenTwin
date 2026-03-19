// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Plot/PlotBase.h"
#include "OTWidgets/Plot/PlotLegend.h"
#include "OTWidgets/Plot/PlotDataset.h"
#include "OTWidgets/Plot/PlotLegendItem.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollarea.h>

ot::PlotLegend::PlotLegend(PlotBase* _plot, QWidget* _parentWidget)
	: m_plot(_plot)
{
	m_rootWidget = new QWidget(_parentWidget);
	QVBoxLayout* rootLayout = new QVBoxLayout(m_rootWidget);
	rootLayout->setContentsMargins(0, 0, 0, 0);

	QScrollArea* scrollArea = new QScrollArea(m_rootWidget);
	scrollArea->setWidgetResizable(true);
	rootLayout->addWidget(scrollArea, 1);

	QWidget* entriesWidget = new QWidget(scrollArea);
	QVBoxLayout* entriesContainerLayout = new QVBoxLayout(entriesWidget);
	entriesContainerLayout->setContentsMargins(0, 0, 0, 0);

	m_selectedLayout = new QVBoxLayout;
	entriesContainerLayout->addLayout(m_selectedLayout, 0);

	m_dimmedLayout = new QVBoxLayout;
	entriesContainerLayout->addLayout(m_dimmedLayout, 0);

	entriesContainerLayout->addStretch(1);
		
	scrollArea->setWidget(entriesWidget);
}

ot::PlotLegend::~PlotLegend() 
{
	for (PlotLegendItem* item : m_items)
	{
		OTAssertNullptr(item);
		item->forgetLegend();
	}
	m_items.clear();

	OTAssertNullptr(m_rootWidget);
	m_rootWidget->deleteLater();
}

void ot::PlotLegend::addItem(PlotLegendItem* _item)
{
	auto it = std::find(m_items.begin(), m_items.end(), _item);
	if (it != m_items.end())
	{
		OT_LOG_EAS("Item already exists in legend");
		return;
	}

	_item->setLegend(this);
	m_items.push_back(_item);

	PlotDataset* dataset = _item->getDataset();
	if (!dataset)
	{
		OT_LOG_EAS("Legend item has no dataset");
		return;
	}

	m_selectedLayout->addWidget(_item->getSelectedWidget(), 0);
	m_dimmedLayout->addWidget(_item->getDimmedWidget(), 0);
}

void ot::PlotLegend::removeItem(PlotLegendItem* _item)
{
	auto it = std::find(m_items.begin(), m_items.end(), _item);
	if (it == m_items.end())
	{
		return;
	}

	m_items.erase(it);
	
	m_selectedLayout->removeWidget(_item->getSelectedWidget());
	_item->getSelectedWidget()->setParent(nullptr);

	m_dimmedLayout->removeWidget(_item->getDimmedWidget());
	_item->getDimmedWidget()->setParent(nullptr);
}
