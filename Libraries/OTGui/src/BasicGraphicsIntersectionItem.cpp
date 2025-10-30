// @otlicense
// File: BasicGraphicsIntersectionItem.cpp
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
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/BasicGraphicsIntersectionItem.h"

ot::BasicGraphicsIntersectionItem::BasicGraphicsIntersectionItem() 
	: GraphicsEllipseItemCfg(5., 5., nullptr)
{
	this->setBackgroundPainer(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder));
	this->setGraphicsItemFlags(GraphicsItemFlag::ItemHandlesState | GraphicsItemFlag::ItemIsConnectable | GraphicsItemFlag::ItemIsMoveable | GraphicsItemFlag::ItemIsSelectable | GraphicsItemFlag::ItemSnapsToGridCenter);
}