// @otlicense

//! @file Splitter.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

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