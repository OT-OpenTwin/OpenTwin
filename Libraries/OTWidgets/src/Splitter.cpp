//! @file Splitter.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/Splitter.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"

ot::Splitter::Splitter(QWidget* _parent) 
	: QSplitter(_parent)
{
	this->slotGlobalColorStyleChanged(GlobalColorStyle::instance().getCurrentStyle());
	this->connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &Splitter::slotGlobalColorStyleChanged);
}

ot::Splitter::Splitter(Qt::Orientation _orientation, QWidget* _parent)
	: QSplitter(_orientation, _parent)
{
	this->slotGlobalColorStyleChanged(GlobalColorStyle::instance().getCurrentStyle());
	this->connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &Splitter::slotGlobalColorStyleChanged);
}

ot::Splitter::~Splitter() {
	this->disconnect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &Splitter::slotGlobalColorStyleChanged);
}

void ot::Splitter::slotGlobalColorStyleChanged(const ColorStyle& _style) {
	if (!_style.hasInteger(OT_COLORSTYLE_INT_SplitterHandleWidth)) return;
	this->setHandleWidth(_style.getInteger(OT_COLORSTYLE_INT_SplitterHandleWidth));
}