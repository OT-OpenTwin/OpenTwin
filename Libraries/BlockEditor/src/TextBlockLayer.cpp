//! @file TextBlockLayer.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/TextBlockLayer.h"

ot::TextBlockLayer::TextBlockLayer(ot::DefaultBlock* _block) : ot::BlockLayer(_block) {

}

ot::TextBlockLayer::~TextBlockLayer() {

}

void ot::TextBlockLayer::paintLayer(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) {
	otAssert(0, "Not implemented yet");
}