// @otlicense
// File: PlotLegend.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Plot/PlotBase.h"
#include "OTWidgets/Plot/PlotLegend.h"
#include "OTWidgets/Plot/PlotDataset.h"
#include "OTWidgets/Plot/PlotLegendItem.h"
#include "OTWidgets/Widgets/VerticalScrollArea.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>

ot::PlotLegend::PlotLegend(PlotBase* _plot, QWidget* _parentWidget)
	: m_plot(_plot)
{
	m_rootWidget = new QWidget(_parentWidget);
	QVBoxLayout* rootLayout = new QVBoxLayout(m_rootWidget);
	rootLayout->setContentsMargins(0, 0, 0, 0);

	VerticalScrollArea* scrollArea = new VerticalScrollArea(m_rootWidget);
	rootLayout->addWidget(scrollArea, 1);

	QWidget* entriesWidget = new QWidget(scrollArea);
	QVBoxLayout* entriesContainerLayout = new QVBoxLayout(entriesWidget);
	entriesContainerLayout->setContentsMargins(0, 0, 0, 0);

	m_selectedLayout = new QVBoxLayout;
	m_selectedLayout->setContentsMargins(0, 0, 0, 0);
	entriesContainerLayout->addLayout(m_selectedLayout, 0);

	m_dimmedLayout = new QVBoxLayout;
	m_dimmedLayout->setContentsMargins(0, 0, 0, 0);
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
		OT_LOG_E("Item already exists in legend");
		return;
	}

	_item->setLegend(this);
	m_items.push_back(_item);

	PlotDataset* dataset = _item->getDataset();
	if (!dataset)
	{
		OT_LOG_E("Legend item has no dataset");
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
