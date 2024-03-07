#include "EntityBlockConnection.h"
#include "OTCommunication/ActionTypes.h"
#include "BlockConnectionBSON.h"
#include "OTGui/GraphicsPackage.h"

EntityBlockConnection::EntityBlockConnection(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBase(ID, parent, obs, ms, factory, owner)
{
	_navigationTreeIconName = "connection";
	_navigationTreeIconNameHidden = "connection";
	_blockTitle = "Connection";	
}

EntityBlockConnection::~EntityBlockConnection()
{

}

ot::GraphicsConnectionCfg& EntityBlockConnection::getConnectionCfg()
{
	return this->connection;
}

void EntityBlockConnection::setConnectionCfg(const ot::GraphicsConnectionCfg& connectionCfg)
{
	this->connection = connectionCfg;
}

void EntityBlockConnection::CreateConnections()
{
	ot::GraphicsConnectionPackage connectionPckg(_graphicsScenePackage);
	ot::GraphicsConnectionCfg connectionCfg = this->getConnectionCfg();
	connectionCfg.setStyle(ot::GraphicsConnectionCfg::SmoothLine);

	connectionPckg.addConnection(connectionCfg);

	ot::JsonDocument reqDoc;
	reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, reqDoc.GetAllocator()), reqDoc.GetAllocator());
	_info.addToJsonObject(reqDoc, reqDoc.GetAllocator());

	ot::JsonObject pckgObj;
	connectionPckg.addToJsonObject(pckgObj, reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, reqDoc.GetAllocator());

	getObserver()->sendMessageToViewer(reqDoc);
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

	ot::BlockConnectionBSON serializeableConnection(this->connection);
	auto subDocument = serializeableConnection.SerializeBSON();
	storage.append(bsoncxx::builder::basic::kvp("ConnectionCfg", subDocument),
				   bsoncxx::builder::basic::kvp("GraphicPackageName", _graphicsScenePackage),
				   bsoncxx::builder::basic::kvp("ServiceName", _info.serviceName()),
				   bsoncxx::builder::basic::kvp("ServiceType", _info.serviceType())
				  );
	
}

void EntityBlockConnection::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	//Now we read the information about the ConnectionCfg
	_graphicsScenePackage = doc_view["GraphicPackageName"].get_utf8().value.data();
	_info.setServiceName(doc_view["ServiceName"].get_utf8().value.data());
	_info.setServiceType(doc_view["ServiceType"].get_utf8().value.data());
	auto connectionCfg = doc_view["ConnectionCfg"].get_document();
	
	auto& subDocument = connectionCfg;
	ot::BlockConnectionBSON connection;
	connection.DeserializeBSON(subDocument);
	ot::GraphicsConnectionCfg graphicsConnection = connection.getConnection();
	this->connection = graphicsConnection;
	
}