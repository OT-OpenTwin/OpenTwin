#include "EntityBlock.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OTGui/GraphicsPackage.h"
#include "BlockConnectionBSON.h"

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
	_connectionsByKey[connection.buildKey()] = connection;
	setModified();
}

void EntityBlock::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::AddStorageData(storage);
	
	storage.append(
		bsoncxx::builder::basic::kvp("BlockID", _blockID),
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
	for (auto& connection : _connectionsByKey)
	{
		ot::BlockConnectionBSON serializeableConnection(connection.second);
		auto subDocument = serializeableConnection.SerializeBSON();
		connectionArray.append(subDocument);
	}
	storage.append(bsoncxx::builder::basic::kvp("Connections", connectionArray));
}

void EntityBlock::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);
	
	_blockID = doc_view["BlockID"].get_utf8().value.data();
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
		_connectionsByKey[graphicsConnection.buildKey()] = graphicsConnection;
	}
}

std::string EntityBlock::CreateBlockHeadline()
{
	const std::string nameWithoutRootDirectory = getName().substr(getName().find_first_of("/") + 1, getName().size());
	const std::string blockTitel = _blockTitle + ": " + nameWithoutRootDirectory;
	return blockTitel;
}

void EntityBlock::CreateNavigationTreeEntry()
{
	if (_navigationTreeIconName != "" && _navigationTreeIconNameHidden != "")
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;

		treeIcons.visibleIcon = _navigationTreeIconName;
		treeIcons.hiddenIcon = _navigationTreeIconNameHidden;

		OT_rJSON_createDOC(doc);
		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_AddContainerNode);
		ot::rJSON::add(doc, OT_ACTION_PARAM_UI_TREE_Name, getName());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, getEntityID());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsEditable, getEditable());

		treeIcons.addToJsonDoc(&doc);
		getObserver()->sendMessageToViewer(doc);
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
	blockCfg->setUid(_blockID);
	blockCfg->setPosition(entCoordinate->getCoordinates());

	ot::GraphicsScenePackage pckg(_graphicsScenePackage);
	pckg.addItem(blockCfg);

	OT_rJSON_createDOC(reqDoc);
	ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem);
	_info.addToJsonObject(reqDoc, reqDoc);

	OT_rJSON_createValueObject(pckgDoc);
	pckg.addToJsonObject(reqDoc, pckgDoc);
	ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgDoc);

	getObserver()->sendMessageToViewer(reqDoc);
}

void EntityBlock::CreateConnections()
{
	ot::GraphicsConnectionPackage connectionPckg(_graphicsScenePackage);

	// Store connection information
	for (auto& connection : _connectionsByKey)
	{
		connectionPckg.addConnection(connection.second);
	}

	// Request UI to add connections
	OT_rJSON_createDOC(connectionReqDoc);
	ot::rJSON::add(connectionReqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection);
	_info.addToJsonObject(connectionReqDoc, connectionReqDoc);
	OT_rJSON_createValueObject(reqConnectionPckgObj);
	connectionPckg.addToJsonObject(connectionReqDoc, reqConnectionPckgObj);
	ot::rJSON::add(connectionReqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, reqConnectionPckgObj);

	getObserver()->sendMessageToViewer(connectionReqDoc);
}

void EntityBlock::AddConnectors(ot::GraphicsFlowItemCfg* flowBlockConfig)
{
	if (flowBlockConfig != nullptr)
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
				flowBlockConfig->addLeft(connectorName, connectorTitle, ot::GraphicsFlowConnectorCfg::Circle, ot::Color::Black);
			}
			else if (connectorType == ot::ConnectorType::InOptional)
			{
				flowBlockConfig->addLeft(connectorName, connectorTitle, ot::GraphicsFlowConnectorCfg::Circle, ot::Color::Blue);
			}
			else if (connectorType == ot::ConnectorType::Out)
			{
				flowBlockConfig->addRight(connectorName, connectorTitle, ot::GraphicsFlowConnectorCfg::Square, ot::Color::Black);
			}
		}
	}

}
