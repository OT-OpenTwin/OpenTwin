#include "EntityBlock.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsPackage.h"
#include "OTCommunication/ActionTypes.h"

#include "EntityBlock.h"
#include "EntityBlockConnection.h"
#include "ConfigurationHelper.h"

EntityBlock::EntityBlock(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBase(ID, parent, obs, ms, factory, owner)
{
	
}

EntityBlock::~EntityBlock()
{
	if (m_coordinateEntity != nullptr)
	{
		delete m_coordinateEntity;
		m_coordinateEntity = nullptr;
	}
}



void EntityBlock::addVisualizationNodes(void)
{
	CreateNavigationTreeEntry();
	CreateBlockItem();
}

void EntityBlock::AddConnector(const ot::Connector& connector)
{
	if (m_connectorsByName.find(connector.getConnectorName()) != m_connectorsByName.end())
	{
		m_connectorsByName[connector.getConnectorName()] = connector;
		setModified();
	}
	else
	{
		OT_LOG_D("Connector with name: " + connector.getConnectorName() + " already exists and is not added");
	}
}

void EntityBlock::RemoveConnector(const ot::Connector& connector)
{
	if (m_connectorsByName.find(connector.getConnectorName()) != m_connectorsByName.end())
	{
		m_connectorsByName.erase(connector.getConnectorName());
		setModified();
	}
	else
	{
		OT_LOG_D("Connector with name: " + connector.getConnectorName() + " does not exist and cannot be removed.");
	}
}

void EntityBlock::AddConnection(const ot::UID id)
{
	m_connectionIDs.push_back(id);
	setModified();
}

void EntityBlock::RemoveConnection(const ot::UID idForRemoval)
{
	m_connectionIDs.remove(idForRemoval);
	setModified();
}

std::string EntityBlock::CreateBlockHeadline()
{
	const std::string nameWithoutRootDirectory = getName().substr(getName().find_last_of("/") + 1, getName().size());
	//const std::string blockTitel = m_blockTitle + ": " + nameWithoutRootDirectory;
	if (nameWithoutRootDirectory.empty()) return m_blockTitle;
	else return nameWithoutRootDirectory;
}

std::string EntityBlock::serialiseAsJSON()
{
	auto docBlock = EntityBase::serialiseAsMongoDocument();
	const std::string jsonDocBlock = bsoncxx::to_json(docBlock);
	ot::JsonDocument entireDoc;
	entireDoc.fromJson(jsonDocBlock);

	std::map<ot::UID, EntityBase*> entityMap;
	EntityCoordinates2D* position = dynamic_cast<EntityCoordinates2D*>(readEntityFromEntityID(this, m_coordinate2DEntityID, entityMap));
	auto docPosition = position->serialiseAsJSON();
	ot::JsonDocument serialisedPosition;
	serialisedPosition.fromJson(docPosition);

	entireDoc.AddMember("SerialisationOfPosition", serialisedPosition, entireDoc.GetAllocator());

	return entireDoc.toJson();
}

bool EntityBlock::deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept
{
	try
	{
		const std::string serialisationString = ot::json::toJson(_serialisation);
		std::string_view serialisedEntityJSONView(serialisationString);
		auto serialisedEntityBSON = bsoncxx::from_json(serialisedEntityJSONView);
		auto serialisedEntityBSONView = serialisedEntityBSON.view();

		readSpecificDataFromDataBase(serialisedEntityBSONView, _entityMap);
		setEntityID(createEntityUID());

		ot::ConstJsonObject positionObjJson = ot::json::getObject(_serialisation, "SerialisationOfPosition");
		std::unique_ptr<EntityCoordinates2D> position(new EntityCoordinates2D(createEntityUID(), nullptr, nullptr, nullptr, nullptr, getOwningService()));
		position->deserialiseFromJSON(positionObjJson, _copyInformation, _entityMap);
		position->setParent(this);
		m_coordinateEntity = position.release();
		m_coordinate2DEntityID = m_coordinateEntity->getEntityID();
		m_connectionIDs.clear();

		_entityMap[getEntityID()] = this;
		_entityMap[m_coordinateEntity->getEntityID()] = m_coordinateEntity;
		return true;
	}
	catch (std::exception _e)
	{
		OT_LOG_E("Failed to deserialise "+  getClassName() +" because: " + std::string(_e.what()));
		return false;
	}
}


void EntityBlock::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::AddStorageData(storage);
	
	storage.append(
		bsoncxx::builder::basic::kvp("CoordinatesEntityID", static_cast<int64_t>(m_coordinate2DEntityID)),
		bsoncxx::builder::basic::kvp("ServiceName", m_info.serviceName()),
		bsoncxx::builder::basic::kvp("ServiceType", m_info.serviceType()),
		bsoncxx::builder::basic::kvp("GraphicPackageChildName", m_graphicsScenePackageChildName)
	);

	auto connectorsArray = bsoncxx::builder::basic::array();
	for (auto& connector : m_connectorsByName)
	{
		auto subDocument = connector.second.SerializeBSON();
		connectorsArray.append(subDocument);
	}
	storage.append(bsoncxx::builder::basic::kvp("Connectors", connectorsArray));

	auto connectionIDArray = bsoncxx::builder::basic::array();
	std::map<ot::UID, EntityBase*> entityMap;

	for (auto& connection : m_connectionIDs)
	{
		//Here i add the ConnectionIDs
		auto subDocument = connection;
		connectionIDArray.append(static_cast<int64_t>(subDocument));
	}
	storage.append(bsoncxx::builder::basic::kvp("ConnectionIDs", connectionIDArray));
}

void EntityBlock::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);
	
	m_coordinate2DEntityID = static_cast<ot::UID>(doc_view["CoordinatesEntityID"].get_int64());
	m_info.setServiceName(doc_view["ServiceName"].get_utf8().value.data());
	m_info.setServiceType(doc_view["ServiceType"].get_utf8().value.data());
	m_graphicsScenePackageChildName = doc_view["GraphicPackageChildName"].get_utf8().value.data();

	auto allConnectors = doc_view["Connectors"].get_array();
	for (auto& element : allConnectors.value)
	{
		auto subDocument = element.get_value().get_document();
		ot::Connector connector;
		connector.DeserializeBSON(subDocument);
		m_connectorsByName[connector.getConnectorName()]=(connector);
	}

	auto connections = doc_view["ConnectionIDs"].get_array();
	for (auto& element : connections.value)
	{
		auto subDocument = element.get_value().get_int64();
		m_connectionIDs.push_back(subDocument);
	}	
}

void EntityBlock::CreateNavigationTreeEntry()
{
	if (m_navigationOldTreeIconName != "" && m_navigationOldTreeIconNameHidden != "")
	{
		OldTreeIcon treeIcons;
		treeIcons.size = 32;

		treeIcons.visibleIcon = m_navigationOldTreeIconName;
		treeIcons.hiddenIcon = m_navigationOldTreeIconNameHidden;

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

		treeIcons.addToJsonDoc(doc);
		getObserver()->sendMessageToViewer(doc);
	}
}

void EntityBlock::CreateBlockItem()
{
	std::map<ot::UID, EntityBase*> entityMap;
	EntityBase* entBase = readEntityFromEntityID(this, m_coordinate2DEntityID, entityMap);
	if (entBase == nullptr) { throw std::exception("EntityBlock failed to load coordinate entity."); }
	if (entBase->getObserver() != nullptr) { entBase->setObserver(nullptr); }
	std::unique_ptr<EntityCoordinates2D> entCoordinate(dynamic_cast<EntityCoordinates2D*>(entBase));
	assert(entCoordinate != nullptr);

	ot::GraphicsItemCfg* blockCfg = CreateBlockCfg();
	blockCfg->setUid(getEntityID());
	blockCfg->setPosition(entCoordinate->getCoordinates());

	const std::string graphicsSceneName = ot::ConfigurationHelper::getGraphicSceneName(getName(), m_graphicsScenePackageChildName);

	ot::GraphicsScenePackage pckg(graphicsSceneName);
	pckg.addItem(blockCfg);

	ot::JsonDocument reqDoc;
	reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, reqDoc.GetAllocator()), reqDoc.GetAllocator());

	m_info.addToJsonObject(reqDoc, reqDoc.GetAllocator());

	ot::VisualisationCfg visualisationCfg;
	ot::JsonObject visualisationCfgJson;
	visualisationCfg.addToJsonObject(visualisationCfgJson, reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_Visualisation_Config, visualisationCfgJson, reqDoc.GetAllocator());

	ot::JsonObject pckgObj;
	pckg.addToJsonObject(pckgObj, reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, reqDoc.GetAllocator());

	getObserver()->sendMessageToViewer(reqDoc);
}


void EntityBlock::AddConnectors(ot::GraphicsFlowItemBuilder& flowBlockBuilder)
{
	for (auto connectorByName : m_connectorsByName)
	{
		const ot::Connector& connector = connectorByName.second;
		ot::ConnectorType connectorType = connector.getConnectorType();
		if (connectorType == ot::ConnectorType::UNKNOWN) { OT_LOG_EAS(""); };
		const std::string connectorName = connector.getConnectorName();
		const std::string connectorTitle = connector.getConnectorTitle();
		if (connectorType == ot::ConnectorType::In)
		{
			flowBlockBuilder.addLeft(connectorName, connectorTitle, ot::GraphicsFlowItemConnector::TriangleRight);
		}
		else if (connectorType == ot::ConnectorType::InOptional)
		{
			flowBlockBuilder.addLeft(connectorName, connectorTitle, ot::GraphicsFlowItemConnector::Circle);
		}
		else if (connectorType == ot::ConnectorType::Out)
		{
			flowBlockBuilder.addRight(connectorName, connectorTitle, ot::GraphicsFlowItemConnector::TriangleRight);
		}
	}
}
