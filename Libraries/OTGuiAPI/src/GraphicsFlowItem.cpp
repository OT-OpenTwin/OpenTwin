// @otlicense
// File: GraphicsFlowItem.cpp
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

void ot::GraphicsFlowItem::addConnector(const std::string& _name, FlowConnectorType _type, const std::string& _toolTip) {
	this->addConnector(_name, _name, _type, _toolTip);
}

void ot::GraphicsFlowItem::addConnector(const std::string& _name, const std::string& _title, FlowConnectorType _type, const std::string& _toolTip) {
	GraphicsFlowItemConnector newConnector = m_builder.getDefaultConnectorStyle();
	newConnector.setName(_name);
	newConnector.setText(_title);
	newConnector.setToolTip(_toolTip);

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
	m_builder.setBackgroundImageAlignment(Alignment::Center);
	m_builder.setBackgroundImageMargins(MarginsD( 5., 2., 2., 2.));
	m_builder.setBackgroundImageInsertMode(GraphicsFlowItemBuilder::OnLayout);
	m_builder.setBackgroundImageMaintainAspectRatio(true);

	// Connector style
	GraphicsFlowItemConnector connectorStyle;
	connectorStyle.setTextColor(Color(White));

	m_builder.setDefaultConnectorStyle(connectorStyle);
}