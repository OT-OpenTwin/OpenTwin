// @otlicense
// File: SequenceDiaLifeLine.cpp
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
#include "OTWidgets/Diagram/SequenceDiaLifeLine.h"
#include "OTWidgets/Graphics/GraphicsTextItem.h"
#include "OTWidgets/Graphics/GraphicsStackItem.h"
#include "OTWidgets/Graphics/GraphicsGroupItem.h"
#include "OTWidgets/Graphics/GraphicsLineItem.h"
#include "OTWidgets/Graphics/GraphicsGridLayoutItem.h"
#include "OTWidgets/Graphics/GraphicsRectangularItem.h"

ot::SequenceDiaLifeLine::SequenceDiaLifeLine(const std::string& _name)
{
	m_rootGroup = new GraphicsGroupItem;
	m_rootGroup->setGraphicsItemName(_name);
	m_rootGroup->setGraphicsItemFlags(GraphicsItemCfg::ItemIsMoveable | GraphicsItemCfg::ItemIsSelectable);
	
	GraphicsStackItem* titleStack = new GraphicsStackItem;
	m_rootGroup->addItem(titleStack);
	titleStack->setGraphicsItemName(_name + "_TitleStack");

	GraphicsRectangularItem* titleBackground = new GraphicsRectangularItem;
	titleStack->addItem(titleBackground, false, true);
	titleBackground->setGraphicsItemName(_name + "_TitleBackground");

	GraphicsGridLayoutItem* titleGrid = new GraphicsGridLayoutItem;
	titleStack->addItem(titleGrid, true, false);
	titleGrid->setGraphicsItemName(_name + "_TitleGrid");
	titleGrid->setRowStretchFactor(0, 1);
	titleGrid->setRowStretchFactor(2, 1);
	titleGrid->setColumnStretchFactor(0, 1);
	titleGrid->setColumnStretchFactor(2, 1);

	GraphicsTextItem* titleText = new GraphicsTextItem;
	titleGrid->addItem(titleText, 1, 1);
	titleText->setGraphicsItemName(_name + "_TitleText");
	titleText->setText(_name);
}

ot::SequenceDiaLifeLine::~SequenceDiaLifeLine()
{
}

ot::GraphicsItem* ot::SequenceDiaLifeLine::getGraphicsItem() const
{
	return m_rootGroup;
}

QRectF ot::SequenceDiaLifeLine::getBoundingRect() const
{
	return m_rootGroup->boundingRect();
}
