#include "EntityBlockConnection.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsPackage.h"

EntityBlockConnection::EntityBlockConnection(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBase(ID, parent, obs, ms, factory, owner), m_lineStyle(2., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder))
{
	_navigationTreeIconName = "connection";
	_navigationTreeIconNameHidden = "connection";
}

EntityBlockConnection::~EntityBlockConnection()
{

}

ot::GraphicsConnectionCfg EntityBlockConnection::getConnectionCfg()
{
	ot::GraphicsConnectionCfg cfg(_blockIDOrigin, _connectorNameOrigin, _blockIDDestination, _connectorNameDestination);
	ot::OutlineF outlineCfg;

	EntityPropertiesGuiPainter* painterProperty = dynamic_cast<EntityPropertiesGuiPainter*>(this->getProperties().getProperty("Line Painter"));
	outlineCfg.setPainter(painterProperty->getValue()->createCopy());
	
	auto lineWidthProperty = dynamic_cast<EntityPropertiesDouble*>(this->getProperties().getProperty("Line Width"));
	outlineCfg.setWidth(lineWidthProperty->getValue());

	auto lineStyleProperty = dynamic_cast<EntityPropertiesSelection*>(this->getProperties().getProperty("Line Style"));
	outlineCfg.setStyle(ot::stringToLineStyle(lineStyleProperty->getValue()));

	cfg.setLineStyle(outlineCfg);

	auto lineShapeProperty = dynamic_cast<EntityPropertiesSelection*>(this->getProperties().getProperty("Line Shape"));
	cfg.setLineShape(ot::GraphicsConnectionCfg::stringToShape(lineShapeProperty->getValue()));
	
	cfg.setUid(getEntityID());
	return cfg;
}

void EntityBlockConnection::setConnectionCfg(const ot::GraphicsConnectionCfg& connectionCfg)
{
	EntityPropertiesGuiPainter* color = dynamic_cast<EntityPropertiesGuiPainter*>(this->getProperties().getProperty("Line Painter"));
	color->setValue(connectionCfg.getLineStyle().painter()->createCopy());

	EntityPropertiesDouble* lineWidth = dynamic_cast<EntityPropertiesDouble*>(this->getProperties().getProperty("Line Width"));
	lineWidth->setValue(connectionCfg.getLineStyle().width());

	EntityPropertiesSelection* lineStyle = dynamic_cast<EntityPropertiesSelection*>(this->getProperties().getProperty("Line Style"));
	lineStyle->setValue(ot::toString(connectionCfg.getLineStyle().style()));
	
	EntityPropertiesSelection* lineShape = dynamic_cast<EntityPropertiesSelection*>(this->getProperties().getProperty("Line Shape"));
	lineShape->setValue(ot::GraphicsConnectionCfg::shapeToString(connectionCfg.getLineShape()));
	
	_blockIDOrigin = connectionCfg.getOriginUid();
	_blockIDDestination = connectionCfg.getDestinationUid();
	_connectorNameDestination = connectionCfg.getDestConnectable();
	_connectorNameOrigin = connectionCfg.getOriginConnectable();
}

void EntityBlockConnection::CreateConnections()
{
	
	ot::GraphicsConnectionPackage connectionPckg(_graphicsScenePackage);
	ot::GraphicsConnectionCfg connectionCfg = this->getConnectionCfg();


	connectionPckg.addConnection(connectionCfg);

	ot::JsonDocument reqDoc;
	reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, reqDoc.GetAllocator()), reqDoc.GetAllocator());
	_info.addToJsonObject(reqDoc, reqDoc.GetAllocator());

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
	EntityPropertiesSelection::createProperty("Settings", "Line Shape", { ot::GraphicsConnectionCfg::shapeToString(ot::GraphicsConnectionCfg::ConnectionShape::DirectLine), ot::GraphicsConnectionCfg::shapeToString(ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine) }, ot::GraphicsConnectionCfg::shapeToString(ot::GraphicsConnectionCfg::ConnectionShape::DirectLine), "default", getProperties());
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
	if (_navigationTreeIconName != "" && _navigationTreeIconNameHidden != "")
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;

		treeIcons.visibleIcon = _navigationTreeIconName;
		treeIcons.hiddenIcon = _navigationTreeIconNameHidden;

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

		treeIcons.addToJsonDoc(doc);
		getObserver()->sendMessageToViewer(doc);
	}
}

//
//void EntityBlockConnection::StoreToDataBase(void)
//{
//	EntityBase::StoreToDataBase();
//}

void EntityBlockConnection::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::AddStorageData(storage);

	// Store the ConnectionCfg

	
	storage.append(
		
	   bsoncxx::builder::basic::kvp("GraphicPackageName", _graphicsScenePackage),
	   bsoncxx::builder::basic::kvp("ServiceName", _info.serviceName()),
	   bsoncxx::builder::basic::kvp("ServiceType", _info.serviceType()),

		bsoncxx::builder::basic::kvp("FromConnectable", _connectorNameOrigin),
		bsoncxx::builder::basic::kvp("ToConnectable", _connectorNameDestination),
		bsoncxx::builder::basic::kvp("FromUID", static_cast<int64_t>(_blockIDOrigin)),
		bsoncxx::builder::basic::kvp("ToUID", static_cast<int64_t>(_blockIDDestination))
);
	
}

void EntityBlockConnection::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	//Now we read the information about the ConnectionCfg
	_graphicsScenePackage = doc_view["GraphicPackageName"].get_utf8().value.data();
	_info.setServiceName(doc_view["ServiceName"].get_utf8().value.data());
	_info.setServiceType(doc_view["ServiceType"].get_utf8().value.data());
	
	_connectorNameOrigin = doc_view["FromConnectable"].get_utf8().value.to_string();
	_connectorNameDestination = doc_view["ToConnectable"].get_utf8().value.to_string();
	_blockIDOrigin = static_cast<ot::UID>(doc_view["FromUID"].get_int64());
	_blockIDDestination = static_cast<ot::UID>(doc_view["ToUID"].get_int64());	
}