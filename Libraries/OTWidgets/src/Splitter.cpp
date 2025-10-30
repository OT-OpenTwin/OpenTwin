// @otlicense
// File: Splitter.cpp
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
#include "OTWidgets/Splitter.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"

ot::Splitter::Splitter(QWidget* _parent) 
	: QSplitter(_parent)
{
	this->slotGlobalColorStyleChanged();
	this->connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &Splitter::slotGlobalColorStyleChanged);
}

ot::Splitter::Splitter(Qt::Orientation _orientation, QWidget* _parent)
	: QSplitter(_orientation, _parent)
{
	this->slotGlobalColorStyleChanged();
	this->connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &Splitter::slotGlobalColorStyleChanged);
}

ot::Splitter::~Splitter() {
	this->disconnect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &Splitter::slotGlobalColorStyleChanged);
}

void ot::Splitter::slotGlobalColorStyleChanged(void) {
	const ColorStyle& gStyle = GlobalColorStyle::instance().getCurrentStyle();
	if (!gStyle.hasInteger(ColorStyleIntegerEntry::SplitterHandleWidth)) return;
	this->setHandleWidth(gStyle.getInteger(ColorStyleIntegerEntry::SplitterHandleWidth));
}