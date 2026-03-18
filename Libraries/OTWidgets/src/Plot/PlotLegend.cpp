// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Plot/PlotBase.h"
#include "OTWidgets/Plot/PlotLegend.h"
#include "OTWidgets/Plot/PlotLegendItem.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollarea.h>

ot::PlotLegend::PlotLegend(PlotBase* _plot, QWidget* _parentWidget)
	: m_plot(_plot), m_rootWidget(nullptr), m_itemLayout(nullptr)
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

	m_itemLayout = new QVBoxLayout;
	entriesContainerLayout->addLayout(m_itemLayout, 0);
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
	m_itemLayout->addWidget(_item->getItemWidget(), 0);
}

void ot::PlotLegend::removeItem(PlotLegendItem* _item)
{
	_item->forgetLegend();

	auto it = std::find(m_items.begin(), m_items.end(), _item);
	if (it == m_items.end())
	{
		return;
	}

	m_items.erase(it);
	m_itemLayout->removeWidget(_item->getItemWidget());
	_item->getItemWidget()->setParent(nullptr);
}
