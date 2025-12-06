// @otlicense
// File: EntityBlockConnection.cpp
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

#include "EntityBlockConnection.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsPackage.h"
#include "ConfigurationHelper.h"

static EntityFactoryRegistrar<EntityBlockConnection> registrar(EntityBlockConnection::className());

EntityBlockConnection::EntityBlockConnection(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	:EntityBase(ID, parent, obs, ms), m_lineStyle(2., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)),
	_blockIDOrigin(-1), _blockIDDestination(-1)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/connection");
	treeItem.setHiddenIcon("Default/connection");
	this->setDefaultTreeItem(treeItem);
}

EntityBlockConnection::~EntityBlockConnection()
{

}

ot::GraphicsConnectionCfg EntityBlockConnection::getConnectionCfg()
{
	ot::GraphicsConnectionCfg cfg(_blockIDOrigin, _connectorNameOrigin, _blockIDDestination, _connectorNameDestination);
	ot::PenFCfg outlineCfg;

	EntityPropertiesGuiPainter* painterProperty = dynamic_cast<EntityPropertiesGuiPainter*>(this->getProperties().getProperty("Line Painter"));
	outlineCfg.setPainter(painterProperty->getValue()->createCopy());
	
	auto lineWidthProperty = dynamic_cast<EntityPropertiesDouble*>(this->getProperties().getProperty("Line Width"));
	outlineCfg.setWidth(lineWidthProperty->getValue());

	auto lineStyleProperty = dynamic_cast<EntityPropertiesSelection*>(this->getProperties().getProperty("Line Style"));
	outlineCfg.setStyle(ot::stringToLineStyle(lineStyleProperty->getValue()));

	cfg.setLineStyle(outlineCfg);

	auto lineShapeProperty = dynamic_cast<EntityPropertiesSelection*>(this->getProperties().getProperty("Line Shape"));
	cfg.setLineShape(ot::GraphicsConnectionCfg::stringToShape(lineShapeProperty->getValue()));

	cfg.setDestPos(m_destPos);
	cfg.setOriginPos(m_originPos);
	
	cfg.setUid(getEntityID());
	return cfg;
}

void EntityBlockConnection::setConnectionCfg(const ot::GraphicsConnectionCfg& connectionCfg)
{
	EntityPropertiesGuiPainter* color = dynamic_cast<EntityPropertiesGuiPainter*>(this->getProperties().getProperty("Line Painter"));
	color->setValue(connectionCfg.getLineStyle().getPainter()->createCopy());

	EntityPropertiesDouble* lineWidth = dynamic_cast<EntityPropertiesDouble*>(this->getProperties().getProperty("Line Width"));
	lineWidth->setValue(connectionCfg.getLineStyle().getWidth());

	EntityPropertiesSelection* lineStyle = dynamic_cast<EntityPropertiesSelection*>(this->getProperties().getProperty("Line Style"));
	lineStyle->setValue(ot::toString(connectionCfg.getLineStyle().getStyle()));
	
	EntityPropertiesSelection* lineShape = dynamic_cast<EntityPropertiesSelection*>(this->getProperties().getProperty("Line Shape"));
	lineShape->setValue(ot::GraphicsConnectionCfg::shapeToString(connectionCfg.getLineShape()));

	_blockIDOrigin = connectionCfg.getOriginUid();
	_blockIDDestination = connectionCfg.getDestinationUid();
	_connectorNameDestination = connectionCfg.getDestConnectable();
	_connectorNameOrigin = connectionCfg.getOriginConnectable();
	m_originPos = connectionCfg.getOriginPos();
	m_destPos = connectionCfg.getDestPos();
	setModified();
}

void EntityBlockConnection::setGraphicsPickerKey(const std::string& _key) {
	if (_key != m_pickerKey) {
		m_pickerKey = _key;
		setModified();
	}
}

void EntityBlockConnection::CreateConnections()
{
	const std::string graphicsSceneName = ot::ConfigurationHelper::getGraphicSceneName(getName(), m_graphicsScenePackageChildName);
	
	ot::GraphicsConnectionPackage connectionPckg(graphicsSceneName);
	ot::GraphicsConnectionCfg connectionCfg = this->getConnectionCfg();

	connectionPckg.setPickerKey(m_pickerKey);
	connectionPckg.addConnection(connectionCfg);

	ot::JsonDocument reqDoc;
	reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, reqDoc.GetAllocator()), reqDoc.GetAllocator());
	
	ot::VisualisationCfg visualisationCfg;
	ot::JsonObject visualisationCfgJson;
	visualisationCfg.addToJsonObject(visualisationCfgJson, reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_VisualisationConfig, visualisationCfgJson, reqDoc.GetAllocator());

	ot::JsonObject pckgObj;
	connectionPckg.addToJsonObject(pckgObj, reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, reqDoc.GetAllocator());

	getObserver()->sendMessageToViewer(reqDoc);
}

void EntityBlockConnection::createProperties()
{
	ot::Color* color = new ot::Color(ot::Black);
	EntityPropertiesGuiPainter::createProperty("Settings", "Line Painter", new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder), "default", getProperties());
	EntityPropertiesDouble::createProperty("Settings", "Line Width", 2., "default", getProperties());
	EntityPropertiesSelection::createProperty("Settings", "Line Style", { ot::toString(ot::LineStyle::NoLine), ot::toString(ot::LineStyle::SolidLine), ot::toString(ot::LineStyle::DotLine), ot::toString(ot::LineStyle::DashLine), ot::toString(ot::LineStyle::DashDotLine), ot::toString(ot::LineStyle::DashDotDotLine) }, ot::toString(ot::LineStyle::SolidLine), "default", getProperties());
	EntityPropertiesSelection::createProperty("Settings", "Line Shape", { 
		ot::GraphicsConnectionCfg::shapeToString(ot::GraphicsConnectionCfg::ConnectionShape::DirectLine),
		ot::GraphicsConnectionCfg::shapeToString(ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine),
		ot::GraphicsConnectionCfg::shapeToString(ot::GraphicsConnectionCfg::ConnectionShape::XYLine),
		ot::GraphicsConnectionCfg::shapeToString(ot::GraphicsConnectionCfg::ConnectionShape::YXLine),
		ot::GraphicsConnectionCfg::shapeToString(ot::GraphicsConnectionCfg::ConnectionShape::AutoXYLine)
		}, ot::GraphicsConnectionCfg::shapeToString(ot::GraphicsConnectionCfg::ConnectionShape::DirectLine), "default", getProperties());
}

bool EntityBlockConnection::updateFromProperties()
{
	CreateConnections();
	getProperties().forceResetUpdateForAllProperties();
	return true;
}


void EntityBlockConnection::addVisualizationNodes(void)
{
	CreateNavigationTreeEntry();
	CreateConnections();
}



void EntityBlockConnection::CreateNavigationTreeEntry()
{
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
		
		doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

		getObserver()->sendMessageToViewer(doc);
}

void EntityBlockConnection::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::addStorageData(storage);

	// Store the ConnectionCfg

	
	storage.append(
		
	   bsoncxx::builder::basic::kvp("GraphicPackageChildName", m_graphicsScenePackageChildName),
		bsoncxx::builder::basic::kvp("GraphicsPickerKey", m_pickerKey),

		bsoncxx::builder::basic::kvp("FromConnectable", _connectorNameOrigin),
		bsoncxx::builder::basic::kvp("ToConnectable", _connectorNameDestination),
		bsoncxx::builder::basic::kvp("FromUID", static_cast<int64_t>(_blockIDOrigin)),
		bsoncxx::builder::basic::kvp("ToUID", static_cast<int64_t>(_blockIDDestination)),
		bsoncxx::builder::basic::kvp("OriginPosX", m_originPos.x()),
		bsoncxx::builder::basic::kvp("OriginPosY", m_originPos.y()),
		bsoncxx::builder::basic::kvp("DestinationPosX", m_destPos.x()),
		bsoncxx::builder::basic::kvp("DestinationPosY", m_destPos.y())
);
	
}

void EntityBlockConnection::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	//Now we read the information about the ConnectionCfg
	m_graphicsScenePackageChildName = std::string(doc_view["GraphicPackageChildName"].get_string().value.data());
	
	_connectorNameOrigin = std::string(doc_view["FromConnectable"].get_string().value.data());
	_connectorNameDestination = std::string(doc_view["ToConnectable"].get_string().value.data());
	_blockIDOrigin = static_cast<ot::UID>(doc_view["FromUID"].get_int64());
	_blockIDDestination = static_cast<ot::UID>(doc_view["ToUID"].get_int64());

	double originX = 0.0;
	double originY = 0.0;
	double destX = 0.0;
	double destY = 0.0;

	auto itOriginPosX = doc_view.find("OriginPosX");
	if (itOriginPosX != doc_view.end()) {
		originX = itOriginPosX->get_double();
	}

	auto itOriginPosY = doc_view.find("OriginPosY");
	if (itOriginPosY != doc_view.end()) {
		originY = itOriginPosY->get_double();
	}

	auto itDestPosX = doc_view.find("DestinationPosX");
	if (itDestPosX != doc_view.end()) {
		destX = itDestPosX->get_double();
	}

	auto itDestPosY = doc_view.find("DestinationPosY");
	if (itDestPosY != doc_view.end()) {
		destY = itDestPosY->get_double();
	}

	m_originPos.setX(originX);
	m_originPos.setY(originY);
	m_destPos.setX(destX);
	m_destPos.setY(destY);

	auto pickerIt = doc_view.find("GraphicsPickerKey");
	if (pickerIt != doc_view.end())
	{
		m_pickerKey = pickerIt->get_string().value.data();
	}
	else {
		// Legacy support
		auto lst = getServicesForCallback(Callback::DataNotify);
		if (!lst.empty()) {
			m_pickerKey = lst.front();
		}
		else {
			OT_LOG_W("Block connection entity has no GraphicsPickerKey and no callback service set { \"ID\": " + std::to_string(getEntityID()) + ", \"Name\": \"" + getName() + "\" }");
			m_pickerKey = OT_INFO_SERVICE_TYPE_MODEL;
		}
	}
}