//! @file GraphicsConnectionConnectorItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin Widgets header
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTWidgets/GraphicsConnectionConnectorItem.h"

ot::GraphicsConnectionConnectorItem::GraphicsConnectionConnectorItem() {
	GraphicsEllipseItemCfg cfg;
	cfg.setBackgroundPainer(new StyleRefPainter2D(ColorStyleValueEntry::GraphicsItemConnectionConnectable));
	cfg.setConnectionDirection(ConnectOut);
	cfg.setFixedSize(24., 24.);
	PenFCfg pen;
	pen.setStyle(LineStyle::NoLine);
	cfg.setOutline(pen);
	cfg.setRadiusX(12.);
	cfg.setRadiusY(12.);
	cfg.setGraphicsItemFlags(GraphicsItemCfg::ItemIsMoveable | GraphicsItemCfg::ItemHandlesState | 
		GraphicsItemCfg::ItemSnapsToGridCenter | GraphicsItemCfg::ItemSilencesNotifcations
	);
	
	this->setupFromConfig(&cfg);
}

ot::GraphicsConnectionConnectorItem::~GraphicsConnectionConnectorItem() {

}
