// @otlicense
// File: GraphicsConnectionConnectorItem.cpp
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

// OpenTwin Widgets header
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTWidgets/GraphicsConnectionConnectorItem.h"

ot::GraphicsConnectionConnectorItem::GraphicsConnectionConnectorItem() {
	const double r = 8.;
	GraphicsEllipseItemCfg cfg;
	cfg.setBackgroundPainer(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemConnectionConnectable));
	cfg.setConnectionDirection(ConnectionDirection::Any);
	cfg.setFixedSize(2. * r, 2. * r);
	PenFCfg pen;
	pen.setStyle(LineStyle::SolidLine);
	pen.setWidth(1.);
	pen.setColor(ColorStyleValueEntry::GraphicsItemConnectionConnectable);
	cfg.setOutline(pen);
	cfg.setRadiusX(r);
	cfg.setRadiusY(r);
	cfg.setGraphicsItemFlags(GraphicsItemCfg::ItemIsMoveable | GraphicsItemCfg::ItemHandlesState | 
		GraphicsItemCfg::ItemSnapsToGridCenter //| GraphicsItemCfg::ItemSilencesNotifcations
	);
	
	this->setupFromConfig(&cfg);
	this->setZValue(GraphicsZValues::Connector);
}

ot::GraphicsConnectionConnectorItem::~GraphicsConnectionConnectorItem() {

}
