// @otlicense

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
