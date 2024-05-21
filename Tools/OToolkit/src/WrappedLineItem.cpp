//! @file WrappedLineItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedLineItem.h"

WrappedLineItem::WrappedLineItem() {

}

WrappedLineItem::~WrappedLineItem() {

}

bool WrappedLineItem::rebuildItem(void) {
	if (this->controlPoints().size() != 2) return false;
	this->setLine(this->controlPoints().front(), this->controlPoints().back());
	return true;
}