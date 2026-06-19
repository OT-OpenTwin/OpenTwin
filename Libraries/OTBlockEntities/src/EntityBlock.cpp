// @otlicense
// File: EntityBlock.cpp
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
#include "OTCore/Logging/Logger.h"
#include "OTGui/Graphics/GraphicsPackage.h"
#include "OTCommunication/ActionTypes.h"
#include "OTBlockEntities/EntityBlock.h"
#include "OTBlockEntities/EntityBlockConnection.h"
#include "OTBlockEntities/BlockConfigurationHelper.h"

ot::EntityBlock::EntityBlock(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	:EntityBase(ID, parent, obs, ms)
{
	ot::VisualisationTypes visTypes = getVisualizationTypes();
	visTypes.addGraphicsItemVisualisation();
	setDefaultVisualizationTypes(visTypes);
}

ot::EntityBlock::~EntityBlock()
{
	if (m_coordinateEntity != nullptr)
	{
		delete m_coordinateEntity;
		m_coordinateEntity = nullptr;
	}
}

void ot::EntityBlock::addVisualizationNodes(void)
{
	createNavigationTreeEntry();
	createBlockItem();
}

void ot::EntityBlock::addConnector(const ot::Connector& connector)
{
	if (m_connectorsByName.find(connector.getConnectorName()) == m_connectorsByName.end())
	{
		m_connectorsByName[connector.getConnectorName()] = connector;
		setModified();
	}
	else
	{
		OT_LOG_D("Connector with name: " + connector.getConnectorName() + " already exists and is not added");
	}
}

void ot::EntityBlock::addConnector(ot::Connector&& connector) {
	if (m_connectorsByName.find(connector.getConnectorName()) == m_connectorsByName.end())
	{
		m_connectorsByName[connector.getConnectorName()] = std::move(connector);
		setModified();
	}
	else
	{
		OT_LOG_D("Connector with name: " + connector.getConnectorName() + " already exists and is not added");
	}
}

void ot::EntityBlock::removeConnector(const ot::Connector& connector)
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

void ot::EntityBlock::clearConnectors() {
	m_connectorsByName.clear();
	setModified();
}

ot::Connector ot::EntityBlock::getConnectorByName(const std::string& _connectorName) const {
	auto it = m_connectorsByName.find(_connectorName);
	if (it != m_connectorsByName.end()) {
		return it->second;
	}
	else {
		OT_LOG_E("Connector with name \"" + _connectorName + "\" does not exist.");
		return ot::Connector();
	}
}

void ot::EntityBlock::setGraphicsPickerKey(const std::string& _key) {
	if (_key != m_graphicsPickerKey) {
		m_graphicsPickerKey = _key;
		setModified();
	}
}

void ot::EntityBlock::createProperties() {

}

ot::GraphicsConnectionCfg::ConnectionShape ot::EntityBlock::getDefaultConnectionShape() const {
	return ot::GraphicsConnectionCfg::ConnectionShape::DirectLine;
}

std::string ot::EntityBlock::createBlockHeadline() const
{
	const std::string nameWithoutRootDirectory = getName().substr(getName().find_last_of("/") + 1, getName().size());
	//const std::string blockTitel = m_blockTitle + ": " + nameWithoutRootDirectory;
	if (nameWithoutRootDirectory.empty()) return m_blockTitle;
	else return nameWithoutRootDirectory;
}

std::string ot::EntityBlock::serialiseAsJSON()
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

bool ot::EntityBlock::deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, const ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept
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
		std::unique_ptr<EntityCoordinates2D> position(new EntityCoordinates2D(createEntityUID(), nullptr, nullptr, nullptr));
		position->deserialiseFromJSON(positionObjJson, _copyInformation, _entityMap);
		position->setParent(this);
		m_coordinateEntity = position.release();
		m_coordinate2DEntityID = m_coordinateEntity->getEntityID();

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

void ot::EntityBlock::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBase::addStorageData(storage);
	
	storage.append(
		bsoncxx::builder::basic::kvp("CoordinatesEntityID", static_cast<int64_t>(m_coordinate2DEntityID)),
		bsoncxx::builder::basic::kvp("GraphicPackageChildName", m_graphicsScenePackageChildName),
		bsoncxx::builder::basic::kvp("GraphicsPickerKey", m_graphicsPickerKey),
		bsoncxx::builder::basic::kvp("IsHidden", m_hidden)
	);

	auto connectorsArray = bsoncxx::builder::basic::array();
	for (auto& connector : m_connectorsByName)
	{
		auto subDocument = connector.second.SerializeBSON();
		connectorsArray.append(subDocument);
	}
	storage.append(bsoncxx::builder::basic::kvp("Connectors", connectorsArray));
}

void ot::EntityBlock::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);
	
	m_coordinate2DEntityID = static_cast<ot::UID>(doc_view["CoordinatesEntityID"].get_int64());
	m_graphicsScenePackageChildName = doc_view["GraphicPackageChildName"].get_string().value.data();

	auto allConnectors = doc_view["Connectors"].get_array();
	for (auto& element : allConnectors.value)
	{
		auto subDocument = element.get_value().get_document();
		ot::Connector connector;
		connector.DeserializeBSON(subDocument);
		m_connectorsByName[connector.getConnectorName()]=(connector);
	}

	auto docIt = doc_view.find("GraphicsPickerKey");
	if (docIt != doc_view.end())
	{
		m_graphicsPickerKey = docIt->get_string().value.data();
	}
	else {
		// Legacy support
		auto lst = getServicesForCallback(Callback::DataNotify);
		if (!lst.empty()) {
			m_graphicsPickerKey = lst.front();
		}
		else {
			OT_LOG_W("Block entity has no GraphicsPickerKey and no callback service set { \"ID\": " + std::to_string(getEntityID()) + ", \"Name\": \"" + getName() + "\" }");
			m_graphicsPickerKey = OT_INFO_SERVICE_TYPE_MODEL;
		}
	}

	docIt = doc_view.find("IsHidden");
	if (docIt != doc_view.end())
	{
		m_hidden = docIt->get_bool().value;
	}
}

void ot::EntityBlock::createNavigationTreeEntry()
{
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());

		doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

		getObserver()->sendMessageToViewer(doc);
}

void ot::EntityBlock::createBlockItem()
{
	OTAssertNullptr(getObserver());
	if (m_hidden)
	{
		ot::JsonDocument doc = createGraphicsHideRequest();
		getObserver()->sendMessageToViewer(doc);
	}
	else
	{
		ot::JsonDocument doc = createGraphicsShowRequest();
		getObserver()->sendMessageToViewer(doc);
	}
}

ot::JsonDocument ot::EntityBlock::createGraphicsShowRequest() {
	OTAssertNullptr(getModelState());
	std::map<ot::UID, EntityBase*> entityMap;
	EntityBase* entBase = readEntityFromEntityID(this, m_coordinate2DEntityID, entityMap);
	if (entBase == nullptr) {
		throw ot::Exception::General("EntityBlock failed to load coordinate entity.");
	}
	if (entBase->getObserver() != nullptr) {
		entBase->setObserver(nullptr);
	}

	std::unique_ptr<EntityCoordinates2D> entCoordinate(dynamic_cast<EntityCoordinates2D*>(entBase));
	assert(entCoordinate != nullptr);

	return createGraphicsShowRequest(entCoordinate->getCoordinates());
}

ot::JsonDocument ot::EntityBlock::createGraphicsShowRequest(const ot::Point2DD& _position) {
	ot::GraphicsItemCfg* blockCfg = createBlockCfg();

	if (!blockCfg) {
		OT_LOG_E("Failed to create block configuration for block entity  { \"EntityID\": " + std::to_string(getEntityID()) + " }");
		throw ot::Exception::General("Failed to create block configuration for block entity { \"EntityID\": " + std::to_string(getEntityID()) + " }");
	}

	blockCfg->setUid(getEntityID());
	blockCfg->setPosition(_position);

	const std::string graphicsSceneName = ot::BlockConfigurationHelper::getGraphicSceneName(getName(), m_graphicsScenePackageChildName);

	ot::GraphicsScenePackage pckg(graphicsSceneName);
	pckg.addItem(blockCfg);
	pckg.setPickerKey(m_graphicsPickerKey);

	ot::JsonDocument reqDoc;
	reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, reqDoc.GetAllocator()), reqDoc.GetAllocator());

	ot::VisualisationCfg visualisationCfg;
	ot::JsonObject visualisationCfgJson;
	visualisationCfg.addToJsonObject(visualisationCfgJson, reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_VisualisationConfig, visualisationCfgJson, reqDoc.GetAllocator());

	ot::JsonObject pckgObj;
	pckg.addToJsonObject(pckgObj, reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, reqDoc.GetAllocator());

	return reqDoc;
}

ot::JsonDocument ot::EntityBlock::createGraphicsHideRequest()
{
	ot::JsonDocument reqDoc;
	reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveItem, reqDoc.GetAllocator()), reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_ItemId, getEntityID(), reqDoc.GetAllocator());

	const std::string graphicsSceneName = ot::BlockConfigurationHelper::getGraphicSceneName(getName(), m_graphicsScenePackageChildName);
	reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName, ot::JsonString(graphicsSceneName, reqDoc.GetAllocator()), reqDoc.GetAllocator());

	return reqDoc;
}
