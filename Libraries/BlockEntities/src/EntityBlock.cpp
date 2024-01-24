#include "EntityBlock.h"
#include "OTCore/Logger.h"
#include "OTGui/GraphicsPackage.h"
#include "OTCommunication/ActionTypes.h"
#include "BlockConnectionBSON.h"
#include "EntityBlock.h"

EntityBlock::EntityBlock(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBase(ID, parent, obs, ms, factory, owner){}



void EntityBlock::addVisualizationNodes(void)
{
	//Queque for ui messages needed!
	CreateNavigationTreeEntry();
	CreateBlockItem();
	CreateConnections();
}

void EntityBlock::AddConnector(const ot::Connector& connector)
{
	if (_connectorsByName.find(connector.getConnectorName()) != _connectorsByName.end())
	{
		_connectorsByName[connector.getConnectorName()] = connector;
		setModified();
	}
	else
	{
		OT_LOG_D("Connector with name: " + connector.getConnectorName() + " already exists and is not added");
	}
}

void EntityBlock::RemoveConnector(const ot::Connector& connector)
{
	if (_connectorsByName.find(connector.getConnectorName()) != _connectorsByName.end())
	{
		_connectorsByName.erase(connector.getConnectorName());
		setModified();
	}
	else
	{
		OT_LOG_D("Connector with name: " + connector.getConnectorName() + " does not exist and cannot be removed.");
	}
}

void EntityBlock::AddConnection(const ot::GraphicsConnectionCfg& connection)
{
	_connections.push_back(connection);
	setModified();
}

void EntityBlock::RemoveConnection(const ot::GraphicsConnectionCfg& connectionForRemoval)
{
	auto connection = _connections.begin();
	while(connection != _connections.end())
	{
		if (connection->buildKey() == connectionForRemoval.buildKey() || connection->buildReversedKey() == connectionForRemoval.buildKey())
		{
			connection = _connections.erase(connection);
		}
		else
		{
			connection++;
		}
	}
	setModified();
}

void EntityBlock::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::AddStorageData(storage);
	
	storage.append(
		bsoncxx::builder::basic::kvp("CoordinatesEntityID", static_cast<int64_t>(_coordinate2DEntityID)),
		bsoncxx::builder::basic::kvp("ServiceName", _info.serviceName()),
		bsoncxx::builder::basic::kvp("ServiceType", _info.serviceType()),
		bsoncxx::builder::basic::kvp("GraphicPackageName", _graphicsScenePackage)
	);

	auto connectorsArray = bsoncxx::builder::basic::array();
	for (auto& connector : _connectorsByName)
	{
		auto subDocument = connector.second.SerializeBSON();
		connectorsArray.append(subDocument);
	}
	storage.append(bsoncxx::builder::basic::kvp("Connectors", connectorsArray));

	auto connectionArray = bsoncxx::builder::basic::array();
	for (auto& connection : _connections)
	{
		ot::BlockConnectionBSON serializeableConnection(connection);
		auto subDocument = serializeableConnection.SerializeBSON();
		connectionArray.append(subDocument);
	}
	storage.append(bsoncxx::builder::basic::kvp("Connections", connectionArray));
}

void EntityBlock::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);
	
	_coordinate2DEntityID = static_cast<ot::UID>(doc_view["CoordinatesEntityID"].get_int64());
	_info.setServiceName(doc_view["ServiceName"].get_utf8().value.data());
	_info.setServiceType(doc_view["ServiceType"].get_utf8().value.data());
	_graphicsScenePackage = doc_view["GraphicPackageName"].get_utf8().value.data();

	auto allConnectors = doc_view["Connectors"].get_array();
	for (auto& element : allConnectors.value)
	{
		auto subDocument = element.get_value().get_document();
		ot::Connector connector;
		connector.DeserializeBSON(subDocument);
		_connectorsByName[connector.getConnectorName()]=(connector);
	}

	auto connections = doc_view["Connections"].get_array();
	for (auto& element : connections.value)
	{
		auto subDocument = element.get_value().get_document();
		ot::BlockConnectionBSON connection;
		connection.DeserializeBSON(subDocument);
		ot::GraphicsConnectionCfg graphicsConnection = connection.getConnection();
		_connections.push_back(graphicsConnection);
	}
}

std::string EntityBlock::CreateBlockHeadline()
{
	const std::string nameWithoutRootDirectory = getName().substr(getName().find_first_of("/") + 1, getName().size());
	//const std::string blockTitel = _blockTitle + ": " + nameWithoutRootDirectory;
	if (nameWithoutRootDirectory.empty()) return _blockTitle;
	else return nameWithoutRootDirectory;
}

void EntityBlock::CreateNavigationTreeEntry()
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


		ot::JsonDocument refDoc;
		refDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, refDoc.GetAllocator()), refDoc.GetAllocator());
		refDoc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString("Test/"+this->getName(), refDoc.GetAllocator()), refDoc.GetAllocator());
		refDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), refDoc.GetAllocator());
		refDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), refDoc.GetAllocator());
		treeIcons.addToJsonDoc(refDoc);
		getObserver()->sendMessageToViewer(refDoc);
	}
}

void EntityBlock::CreateBlockItem()
{
	std::map<ot::UID, EntityBase*> entityMap;
	EntityBase* entBase = readEntityFromEntityID(this, _coordinate2DEntityID, entityMap);
	if (entBase == nullptr) { throw std::exception("EntityBlock failed to load coordinate entity."); }
	if (entBase->getObserver() != nullptr) { entBase->setObserver(nullptr); }
	std::unique_ptr<EntityCoordinates2D> entCoordinate(dynamic_cast<EntityCoordinates2D*>(entBase));
	assert(entCoordinate != nullptr);

	ot::GraphicsItemCfg* blockCfg = CreateBlockCfg();
	blockCfg->setUid(getBlockID());
	blockCfg->setPosition(entCoordinate->getCoordinates());

	ot::GraphicsScenePackage pckg(_graphicsScenePackage);
	pckg.addItem(blockCfg);

	ot::JsonDocument reqDoc;
	reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, reqDoc.GetAllocator()), reqDoc.GetAllocator());

	_info.addToJsonObject(reqDoc, reqDoc.GetAllocator());

	ot::JsonObject pckgObj;
	pckg.addToJsonObject(pckgObj, reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, reqDoc.GetAllocator());

	getObserver()->sendMessageToViewer(reqDoc);
}

void EntityBlock::CreateConnections()
{
	ot::GraphicsConnectionPackage connectionPckg(_graphicsScenePackage);

	// Store connection information
	for (auto& connection : _connections)
	{
		connection.setStyle(ot::GraphicsConnectionCfg::SmoothLine);

		connectionPckg.addConnection(connection);
	}

	ot::JsonDocument reqDoc;
	reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, reqDoc.GetAllocator()), reqDoc.GetAllocator());
	_info.addToJsonObject(reqDoc, reqDoc.GetAllocator());

	ot::JsonObject pckgObj;
	connectionPckg.addToJsonObject(pckgObj, reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, reqDoc.GetAllocator());

	getObserver()->sendMessageToViewer(reqDoc);
}

void EntityBlock::AddConnectors(ot::GraphicsFlowItemBuilder& flowBlockBuilder)
{
	for (auto connectorByName : _connectorsByName)
	{
		const ot::Connector& connector = connectorByName.second;
		ot::ConnectorType connectorType = connector.getConnectorType();
		if (connectorType == ot::ConnectorType::UNKNOWN) { OT_LOG_EAS(""); };
		const std::string connectorName = connector.getConnectorName();
		const std::string connectorTitle = connector.getConnectorTitle();
		if (connectorType == ot::ConnectorType::In)
		{
			flowBlockBuilder.addLeft(connectorName, connectorTitle, ot::GraphicsFlowItemConnector::Circle);
		}
		else if (connectorType == ot::ConnectorType::InOptional)
		{
			flowBlockBuilder.addLeft(connectorName, connectorTitle, ot::GraphicsFlowItemConnector::Circle);
		}
		else if (connectorType == ot::ConnectorType::Out)
		{
			flowBlockBuilder.addRight(connectorName, connectorTitle, ot::GraphicsFlowItemConnector::Square);
		}
	}
}
