//! @file GraphicsFlowItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGuiAPI/GraphicsFlowItem.h"

ot::GraphicsFlowItem::GraphicsFlowItem() {
	this->ini();
}

ot::GraphicsFlowItem::GraphicsFlowItem(const std::string& _name) {
	m_builder.setName(_name);
	this->ini();
}

ot::GraphicsFlowItem::GraphicsFlowItem(const std::string& _name, const std::string& _title) {
	m_builder.setName(_name);
	m_builder.setTitle(_title);
	this->ini();
}

ot::GraphicsFlowItem::~GraphicsFlowItem() {

}

void ot::GraphicsFlowItem::addConnector(const std::string& _name, FlowConnectorType _type, const std::string& _tip) {
	this->addConnector(_name, _name, _type, _tip);
}

void ot::GraphicsFlowItem::addConnector(const std::string& _name, const std::string& _title, FlowConnectorType _type, const std::string& _tip) {
	GraphicsFlowItemConnector newConnector = m_builder.defaultConnectorStyle();
	newConnector.setName(_name);
	newConnector.setText(_title);
	newConnector.setToolTip(_tip);

	switch (_type)
	{
	case ot::GraphicsFlowItem::Input:
		newConnector.setFigure(GraphicsFlowItemConnector::TriangleRight);
		m_builder.addLeft(newConnector);
		break;
	case ot::GraphicsFlowItem::InputSecondary:
		newConnector.setFigure(GraphicsFlowItemConnector::KiteRight);
		m_builder.addLeft(newConnector);
		break;
	case ot::GraphicsFlowItem::InputOptional:
		newConnector.setFigure(GraphicsFlowItemConnector::Circle);
		m_builder.addLeft(newConnector);
		break;
	case ot::GraphicsFlowItem::Output:
		newConnector.setFigure(GraphicsFlowItemConnector::TriangleRight);
		m_builder.addRight(newConnector);
		break;
	case ot::GraphicsFlowItem::OutputSecondary:
		newConnector.setFigure(GraphicsFlowItemConnector::KiteRight);
		m_builder.addRight(newConnector);
		break;
	case ot::GraphicsFlowItem::OutputOptional:
		newConnector.setFigure(GraphicsFlowItemConnector::Circle);
		m_builder.addRight(newConnector);
		break;
	default:
		OT_LOG_E("Unknown flow connector type (" + std::to_string((int)_type) + ")");
		m_builder.addRight(newConnector);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private functions

void ot::GraphicsFlowItem::ini(void) {
	// Title
	m_builder.setTitleBackgroundGradientColor(Color(Silver));
	m_builder.setTitleForegroundColor(Color(White));

	// Background image
	m_builder.setBackgroundImageAlignment(AlignCenter);
	m_builder.setBackgroundImageMargins(MarginsD( 5., 2., 2., 2.));
	m_builder.setBackgroundImageInsertMode(GraphicsFlowItemBuilder::OnLayout);
	m_builder.setBackgroundImageMaintainAspectRatio(true);

	// Connector style
	GraphicsFlowItemConnector connectorStyle;
	connectorStyle.setTextColor(Color(White));

	m_builder.setDefaultConnectorStyle(connectorStyle);
}