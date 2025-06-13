// Service header
#include "BlockEntityHandler.h"

// Open twin header
#include "OTCommunication/ActionTypes.h"
#include "CircuitElement.h"
#include "Connection.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTGui/StyleRefPainter2D.h"

//#include "ExternalDependencies.h"
#include "Application.h"
#include "ClassFactoryBlock.h"
#include "ClassFactory.h"
#include "EntityBlockCircuitVoltageSource.h"
#include "EntityBlockCircuitResistor.h"
#include "EntityBlockConnection.h"
#include "SimulationResults.h"
#include "EntityBlockCircuitDiode.h"
#include "EntityBlockCircuitVoltageMeter.h"
#include "EntityBlockCircuitCurrentMeter.h"
#include "EntityBlockCircuitCapacitor.h"
#include "EntityBlockCircuitInductor.h"
#include "EntityBlockCircuitElement.h"
#include "EntityBlockCircuitGND.h"
#include "EntityBlockCircuitTransmissionLine.h"

#include "PlotBuilder.h"
#include "ResultCollectionExtender.h"
#include "QuantityDescriptionCurve.h"
#include "OTGui/PainterRainbowIterator.h"

// Third Party Header

//C++
#include <algorithm>
#include <queue>

namespace NodeNumbers {
	static unsigned long long nodeNumber = 0;
	static unsigned long long connectionNumber = 1;	
}

std::shared_ptr<EntityBlock> BlockEntityHandler::CreateBlockEntity(const std::string& editorName, const std::string& blockName, ot::Point2DD& position) {
	ClassFactoryBlock factory;
	EntityBase* baseEntity = factory.CreateEntity(blockName);
	assert(baseEntity != nullptr);
	std::shared_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(baseEntity));

	
	blockEntity->setEditable(true);
	blockEntity->SetServiceInformation(Application::instance()->getBasicServiceInformation());
	blockEntity->setOwningService(OT_INFO_SERVICE_TYPE_CircuitSimulatorService);
	blockEntity->setEntityID(_modelComponent->createEntityUID());
	// Here i want to add the items to the corresponding editor

	
	blockEntity->SetGraphicsScenePackageName(_packageName);
	

	std::unique_ptr<EntityCoordinates2D> blockCoordinates(new EntityCoordinates2D(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_CircuitSimulatorService));
	blockCoordinates->setCoordinates(position);
	blockCoordinates->StoreToDataBase();

	blockEntity->setCoordinateEntityID(blockCoordinates->getEntityID());
	
	// Get the CircuitModel folder infos
	std::string circuitModelFolderName = "CircuitModels";
	ot::EntityInformation entityInfo;
	ot::ModelServiceAPI::getEntityInformation(circuitModelFolderName, entityInfo);

	//Von Blockentity in CircuitEntity casten und createProperties aufrufen

	std::string elementName = InitSpecialisedCircuitElementEntity(circuitModelFolderName, entityInfo.getEntityID(), blockEntity);
	if (elementName != "") {
	//Create block Titles
	//First get a list of all folder items of the Circuit folder
		std::list<std::string> circuitItems = ot::ModelServiceAPI::getListOfFolderItems("Circuits/" + editorName);
		// Create a unique name for the new circuit item
		int count = 1;
		std::string circuitItemName;
		std::string circuitAbbraviationName;
		do {
			circuitAbbraviationName = _blockFolder + "/" + editorName +"/"+ elementName + std::to_string(count);
			count++;
		} while (std::find(circuitItems.begin(), circuitItems.end(), circuitAbbraviationName) != circuitItems.end());
		blockEntity->setName(circuitAbbraviationName);
	}
	else {
		std::string entName = CreateNewUniqueTopologyName(_blockFolder + "/" + editorName, blockEntity->getBlockTitle());
		blockEntity->setName(entName);
	}



	blockEntity->StoreToDataBase();
	ot::ModelServiceAPI::addEntitiesToModel({ blockEntity->getEntityID() }, { blockEntity->getEntityStorageVersion() }, { false }, { blockCoordinates->getEntityID() }, { blockCoordinates->getEntityStorageVersion() }, { blockEntity->getEntityID() }, "Added Block: " + blockName);

	return blockEntity;
}

void BlockEntityHandler::UpdateBlockPosition(const ot::UID& blockID, const ot::Point2DD& position, const ot::Transform transform, ClassFactory* classFactory) {
	
	std::list<ot::EntityInformation> entityInfos;
	ot::UIDList entityIDList{ blockID };
	ot::ModelServiceAPI::getEntityInformation(entityIDList, entityInfos);
	auto entBase = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfos.begin()->getEntityID(), entityInfos.begin()->getEntityVersion(), *classFactory);
	std::unique_ptr<EntityBlock> blockEnt(dynamic_cast<EntityBlock*>(entBase));
	

	bool _rotationChanged = false;
	bool _flipChanged = false;

	//Here I will update the rotation
	auto propertyBase = blockEnt->getProperties().getProperty("Rotation");
	if (propertyBase != nullptr) {
		auto propertyRotation = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
		if (propertyRotation->getValue() != transform.getRotation()) {
			propertyRotation->setValue(transform.getRotation());
			_rotationChanged = true;
		}
	}

	//Here I update the Flip
	std::map<ot::Transform::FlipState, std::string > stringFlipMap;
	stringFlipMap.insert_or_assign(ot::Transform::NoFlip, "NoFlip");
	stringFlipMap.insert_or_assign(ot::Transform::FlipVertically, "FlipVertically");
	stringFlipMap.insert_or_assign(ot::Transform::FlipHorizontally, "FlipHorizontally");

	auto propertyBaseFlip = blockEnt->getProperties().getProperty("Flip");
	if (propertyBaseFlip != nullptr) {
		auto propertyFlip = dynamic_cast<EntityPropertiesSelection*>(propertyBaseFlip);
		if (propertyFlip->getValue() != stringFlipMap[transform.getFlipStateFlags()]) {
			propertyFlip->setValue(stringFlipMap[transform.getFlipStateFlags()]);
			_flipChanged = true;
		}

	}
	


	// If the rotation or flip is changed then we update the block but do not add new coordinate entity
	// because dragging and flipping/rotating at same time is not possible

	if (_rotationChanged || _flipChanged) {
		blockEnt->StoreToDataBase();
		const std::string comment = "Property Updated";
		ot::UIDList topoList{blockEnt->getEntityID()};
		ot::UIDList versionList{blockEnt->getEntityStorageVersion()};
		ot::ModelServiceAPI::updateTopologyEntities(topoList, versionList, comment);
	}
	else {  // block was dragged

		ot::UID positionID = blockEnt->getCoordinateEntityID();
		entityInfos.clear();
		entityIDList = { positionID };
		ot::ModelServiceAPI::getEntityInformation(entityIDList, entityInfos);
		entBase = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfos.begin()->getEntityID(), entityInfos.begin()->getEntityVersion(), *classFactory);
		std::unique_ptr<EntityCoordinates2D> coordinateEnt(dynamic_cast<EntityCoordinates2D*>(entBase));
		coordinateEnt->setCoordinates(position);
		coordinateEnt->StoreToDataBase();
		ot::ModelServiceAPI::addEntitiesToModel({}, {}, {}, { coordinateEnt->getEntityID() }, { coordinateEnt->getEntityStorageVersion() }, { blockID }, "Update BlockItem position");
	}
	
}

void BlockEntityHandler::OrderUIToCreateBlockPicker() {
	auto graphicsEditorPackage = BuildUpBlockPicker();
	ot::JsonDocument doc;
	ot::JsonObject pckgObj;
	graphicsEditorPackage->addToJsonObject(pckgObj, doc.GetAllocator());

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, doc.GetAllocator());

	Application::instance()->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	// Message is queued, no response here
	std::string tmp;
	_uiComponent->sendMessage(true, doc, tmp);
}

std::map<ot::UID, std::shared_ptr<EntityBlock>> BlockEntityHandler::findAllBlockEntitiesByBlockID() {
	std::list<std::string> blockItemNames = ot::ModelServiceAPI::getListOfFolderItems(_blockFolder + "/" + _packageName, true);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(blockItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactoryBlock classFactory;

	std::map<ot::UID, std::shared_ptr<EntityBlock>> blockEntitiesByBlockID;
	for (auto& entityInfo : entityInfos) {
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), classFactory);
		if (baseEntity != nullptr && baseEntity->getClassName() != "EntityBlockConnection") { //Otherwise not a BlockEntity, since ClassFactoryBlock does not handle others 
			std::shared_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(baseEntity));
			assert(blockEntity != nullptr);
			blockEntitiesByBlockID[blockEntity->getEntityID()] = blockEntity;
		}

	}
	return blockEntitiesByBlockID;
}

std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> BlockEntityHandler::findAllEntityBlockConnections() {
	std::list<std::string> connectionItemNames = ot::ModelServiceAPI::getListOfFolderItems("Circuits/" + _packageName + "/Connections");
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(connectionItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactoryBlock classFactory;

	std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> entityBlockConnectionsByBlockID;
	for (auto& entityInfo : entityInfos) {
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), classFactory);
		if (baseEntity != nullptr && baseEntity->getClassName() == "EntityBlockConnection") {
			std::shared_ptr<EntityBlockConnection> blockEntityConnection(dynamic_cast<EntityBlockConnection*>(baseEntity));
			assert(blockEntityConnection != nullptr);
			entityBlockConnectionsByBlockID[blockEntityConnection->getEntityID()] = blockEntityConnection;
		}
	}

	return entityBlockConnectionsByBlockID;	
}

std::shared_ptr<EntityFileText> BlockEntityHandler::getCircuitModel(std::string _modelName) {
	ot::EntityInformation circuitModelInfo;
	ot::ModelServiceAPI::getEntityInformation("CircuitModels/" + _modelName, circuitModelInfo);
	ClassFactory classFactory;
	
	auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(circuitModelInfo.getEntityID(), circuitModelInfo.getEntityVersion(), Application::instance()->getClassFactory());
	if (baseEntity != nullptr) {
		std::shared_ptr<EntityFileText> circuitModelEntity(dynamic_cast<EntityFileText*>(baseEntity));
		assert(circuitModelEntity != nullptr);
		return circuitModelEntity;
	}
	else {
		OT_LOG_E("No CircuitModelEntity found: " + _modelName);
		return nullptr;
	}

}

bool BlockEntityHandler::connectorHasTypeOut(std::shared_ptr<EntityBlock> blockEntity, const std::string& connectorName) {
	auto allConnectors = blockEntity->getAllConnectorsByName();
	const ot::ConnectorType connectorType = allConnectors[connectorName].getConnectorType();
	if (connectorType == ot::ConnectorType::UNKNOWN) { OT_LOG_EAS("Unset connectortype of connector: " + allConnectors[connectorName].getConnectorName()); }
	if (connectorType == ot::ConnectorType::In || connectorType == ot::ConnectorType::InOptional) {
		return false;
	}
	else {
		return true;
	}
}



void BlockEntityHandler::AddBlockConnection(const std::list<ot::GraphicsConnectionCfg>& connections,std::string name) {
	
	auto blockEntitiesByBlockID = findAllBlockEntitiesByBlockID();
	//auto entityBlockConnectionsByBlockID = findAllEntityBlockConnections();

	std::list<ot::UID> topologyEntityIDList;
	std::list<ot::UID> topologyEntityVersionList;
	std::list<bool> topologyEntityForceVisible = { false };
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataEntityVersionList;
	std::list<ot::UID> dataEntityParentList;

	std::string blockName = "EntityBlockConnection";
	int count = 1;
	std::list< std::shared_ptr<EntityBlock>> entitiesForUpdate;
	for (auto& connection : connections) {
		bool originConnectorIsTypeOut(true), destConnectorIsTypeOut(true);

		std::list<std::string> connectionItems = ot::ModelServiceAPI::getListOfFolderItems("Circuits/" + name + "/Connections");

		//Now I first create the needed parameters for entName
		ot::UID entityID = _modelComponent->createEntityUID();
		
		std::string connectionName;
		do {
			connectionName = "Circuits/" + name + "/Connections/" + "Connection" + std::to_string(count);
			count++;
		} while (std::find(connectionItems.begin(), connectionItems.end(), connectionName) != connectionItems.end());


		//Here i create the connectionEntity
		EntityBlockConnection* connectionEntity = new EntityBlockConnection(entityID, nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_CircuitSimulatorService);
		connectionEntity->createProperties();
		
		
	
		//Now i create the GraphicsConnectionCfg and set it with the information

		ot::GraphicsConnectionCfg connectionCfg;
		connectionCfg.setOriginUid(connection.getOriginUid());
		connectionCfg.setDestUid(connection.getDestinationUid());
		connectionCfg.setOriginConnectable(connection.getOriginConnectable());
		connectionCfg.setDestConnectable(connection.getDestConnectable());
		connectionCfg.setLineShape(ot::GraphicsConnectionCfg::ConnectionShape::AutoXYLine);
		connectionCfg.setLinePainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemConnection));

		//Now i set the attirbutes of connectionEntity
		connectionEntity->setConnectionCfg(connectionCfg);
		connectionEntity->setName(connectionName);
		connectionEntity->SetGraphicsScenePackageName(name);
		connectionEntity->SetServiceInformation(Application::instance()->getBasicServiceInformation());
		connectionEntity->setOwningService(OT_INFO_SERVICE_TYPE_CircuitSimulatorService);
		connectionEntity->StoreToDataBase();

		topologyEntityIDList.push_back(connectionEntity->getEntityID());
		topologyEntityVersionList.push_back(connectionEntity->getEntityStorageVersion());


		if (blockEntitiesByBlockID.find(connection.getOriginUid()) != blockEntitiesByBlockID.end()) {
			blockEntitiesByBlockID[connection.getOriginUid()]->AddConnection(connectionEntity->getEntityID());
			entitiesForUpdate.push_back(blockEntitiesByBlockID[connection.getOriginUid()]);
		}
		else {
			OT_LOG_EAS("Could not create connection since block " + std::to_string(connection.getOriginUid()) + " was not found");
			continue;
		}

		if (blockEntitiesByBlockID.find(connection.getDestinationUid()) != blockEntitiesByBlockID.end()) {
			blockEntitiesByBlockID[connection.getDestinationUid()]->AddConnection(connectionEntity->getEntityID());
			entitiesForUpdate.push_back(blockEntitiesByBlockID[connection.getDestinationUid()]);
		}
		else {
			OT_LOG_EAS("Could not create connection since block " + std::to_string(connection.getDestinationUid()) + " was not found.");
			continue;
		}


	}

	if (entitiesForUpdate.size() != 0) {

		for (auto entityForUpdate : entitiesForUpdate) {
			entityForUpdate->StoreToDataBase();
			topologyEntityIDList.push_back(entityForUpdate->getEntityID());
			topologyEntityVersionList.push_back(entityForUpdate->getEntityStorageVersion());
		}
		
		ot::ModelServiceAPI::updateTopologyEntities(topologyEntityIDList, topologyEntityVersionList, "Added new connection to BlockEntities");
		
	}

}

void BlockEntityHandler::AddConnectionToConnection(const std::list<ot::GraphicsConnectionCfg>& connections, std::string editorName, ot::Point2DD pos)
{
	
	auto blockEntitiesByBlockID = findAllBlockEntitiesByBlockID();
	auto connectionEntitiesByID = findAllEntityBlockConnections();
	std::list< std::shared_ptr<EntityBlock>> entitiesForUpdate;
	std::list<std::string> entitiesToDelete;
	std::list<ot::UID> topologyEntityIDList;
	std::list<ot::UID> topologyEntityVersionList;

	std::queue<std::pair<std::string,std::shared_ptr<EntityBlock>>> connectedElements;
	std::list<ot::GraphicsConnectionCfg> connectionsNew;
	
	for (auto connection : connections)
	{
		//First i get the connection which i want to delete by the connection to be added
		if (connectionEntitiesByID.find(connection.getDestinationUid()) == connectionEntitiesByID.end()) {
			OT_LOG_EAS("Connection not found: " + connection.getDestinationUid());
			continue;
		}

		//Now i got the connection and want to delete it at the blocks
		std::shared_ptr<EntityBlockConnection> connectionToDelete = connectionEntitiesByID[connection.getDestinationUid()];
		ot::GraphicsConnectionCfg connectionCfg = connectionToDelete->getConnectionCfg();
		//Saving connected Element and connector
		connectedElements.push(std::make_pair(connection.getOriginConnectable(), blockEntitiesByBlockID[connection.getOriginUid()]));

		// Here I check if the the blocks which are connected to the connection exist
		if (blockEntitiesByBlockID.find(connectionCfg.getDestinationUid()) == blockEntitiesByBlockID.end() ||
			blockEntitiesByBlockID.find(connectionCfg.getOriginUid()) == blockEntitiesByBlockID.end()) {
			OT_LOG_EA("BlockEntity not found");
			continue;
		}

		//Saving  connected Elements and connectors
		connectedElements.push(std::make_pair(connectionCfg.getDestConnectable(),blockEntitiesByBlockID[connectionCfg.getDestinationUid()]));
		
		connectedElements.push(std::make_pair(connectionCfg.getOriginConnectable(),blockEntitiesByBlockID[connectionCfg.getOriginUid()]));
	
		//Now I delete the connection at the blocks
		blockEntitiesByBlockID[(connectionCfg.getDestinationUid())]->RemoveConnection(connectionToDelete->getEntityID());
		blockEntitiesByBlockID[(connectionCfg.getOriginUid())]->RemoveConnection(connectionToDelete->getEntityID());

		entitiesForUpdate.push_back(blockEntitiesByBlockID[(connectionCfg.getDestinationUid())]);
		entitiesForUpdate.push_back(blockEntitiesByBlockID[(connectionCfg.getOriginUid())]);

		entitiesToDelete.push_back(connectionToDelete->getName());

		for (auto entityForUpdate : entitiesForUpdate) {
			entityForUpdate->StoreToDataBase();
			topologyEntityIDList.push_back(entityForUpdate->getEntityID());
			topologyEntityVersionList.push_back(entityForUpdate->getEntityStorageVersion());
		}

		// Now i remove the connections from model
		ot::ModelServiceAPI::deleteEntitiesFromModel(entitiesToDelete);
		ot::ModelServiceAPI::updateTopologyEntities(topologyEntityIDList, topologyEntityVersionList, "Removed Connection from Blocks");

		// As next step i need to add the intersection item 
		std::shared_ptr<EntityBlock> connector = CreateBlockEntity(editorName, "EntityBlockCircuitConnector", pos);

		//Now i create a GraphicsConnectionCfg for all elements
		while (!connectedElements.empty()) {
			ot::GraphicsConnectionCfg temp(connection);
			temp.setDestUid(connector->getEntityID());
			temp.setDestConnectable(connector->getName());
			temp.setOriginUid(connectedElements.front().second->getEntityID());
			temp.setOriginConnectable(connectedElements.front().first);
			/*	ot::GraphicsConnectionCfg(connectedElements.front()->getEntityID(), connectors.front(), connector->getEntityID(), connector->getAllConnectorsByName().begin()->first);*/
			connectionsNew.push_back(temp);
			connectedElements.pop();
		}

		AddBlockConnection(connectionsNew, editorName);	
	}
}

std::string BlockEntityHandler::InitSpecialisedCircuitElementEntity(const std::string& _circuitModelFolderName, const ot::UID& _circuitModelFolderID, std::shared_ptr<EntityBlock> blockEntity) {
	EntityBlockCircuitElement* element = dynamic_cast<EntityBlockCircuitElement*>(blockEntity.get());
	if (element != nullptr) {
		element->createProperties(_circuitModelFolderID);
		return element->getTypeAbbreviation();
	}
	else if(blockEntity->getClassName() != "EntityBlockCircuitGND" && blockEntity->getClassName() != "EntityBlockCircuitConnector") {
		OT_LOG_E("EntityBlockCircuitElement is null");
	}
	
	return "";
}



ot::GraphicsNewEditorPackage* BlockEntityHandler::BuildUpBlockPicker() {
	ot::GraphicsNewEditorPackage* pckg = new ot::GraphicsNewEditorPackage(this->getInitialCircuitName(), this->getInitialCircuitName());
	ot::GraphicsPickerCollectionCfg* a = new ot::GraphicsPickerCollectionCfg("CircuitElements", "Circuit Elements");
	ot::GraphicsPickerCollectionCfg* a1 = new ot::GraphicsPickerCollectionCfg("PassiveElements", "Passive Elements");
	ot::GraphicsPickerCollectionCfg* a2 = new ot::GraphicsPickerCollectionCfg("Meter Elements", "Meter Elements");
	ot::GraphicsPickerCollectionCfg* a3 = new ot::GraphicsPickerCollectionCfg("Sources", "Sources");
	ot::GraphicsPickerCollectionCfg* a4 = new ot::GraphicsPickerCollectionCfg("ActiveElements", "Active Elements");

	a->addChildCollection(a1);
	a->addChildCollection(a2);
	a->addChildCollection(a3);
	a->addChildCollection(a4);

	EntityBlockCircuitVoltageSource element(0, nullptr, nullptr, nullptr,nullptr, "");
	EntityBlockCircuitResistor resistor(0, nullptr, nullptr, nullptr, nullptr, "");
	EntityBlockCircuitDiode diode(0, nullptr, nullptr, nullptr, nullptr, "");
	EntityBlockCircuitVoltageMeter voltMeter(0, nullptr, nullptr, nullptr, nullptr, "");
	EntityBlockCircuitCurrentMeter currentMeter(0, nullptr, nullptr, nullptr, nullptr, "");
	EntityBlockCircuitCapacitor capacitor(0, nullptr, nullptr, nullptr, nullptr, "");
	EntityBlockCircuitInductor inductor(0, nullptr, nullptr, nullptr, nullptr, "");
	EntityBlockCircuitGND gndElement(0, nullptr, nullptr, nullptr, nullptr, "");
	EntityBlockCircuitTransmissionLine transmissionLine (0, nullptr, nullptr, nullptr, nullptr, "");
	
	
	a1->addItem(resistor.getClassName(), resistor.CreateBlockHeadline(), "CircuitElementImages/ResistorBG.png");
	a1->addItem(capacitor.getClassName(), capacitor.CreateBlockHeadline(), "CircuitElementImages/Capacitor.png");
	a1->addItem(inductor.getClassName(), inductor.CreateBlockHeadline(), "CircuitElementImages/Inductor.png");
	a1->addItem(gndElement.getClassName(), gndElement.CreateBlockHeadline(), "CircuitElementImages/GND.png");
	a1->addItem(transmissionLine.getClassName(), transmissionLine.CreateBlockHeadline(), "CircuitElementImages/TranLine.png");

	a2->addItem(voltMeter.getClassName(), voltMeter.CreateBlockHeadline(), "CircuitElementImages/VoltMeter.png");
	a2->addItem(currentMeter.getClassName(), currentMeter.CreateBlockHeadline(), "CircuitElementImages/CurrentMeter.png");

	a3->addItem(element.getClassName(), element.CreateBlockHeadline(), "CircuitElementImages/VoltageSource.png");
	

	a4->addItem(diode.getClassName(), diode.CreateBlockHeadline(), "CircuitElementImages/Diod2.png");
	pckg->addCollection(a);
	return pckg;
}





void BlockEntityHandler::createResultCurves(std::string solverName,std::string simulationType,std::string circuitName) 
{
		
	std::map<std::string, std::vector<double>> resultVectors = SimulationResults::getInstance()->getResultMap();
	ResultCollectionExtender extender(Application::instance());

	PlotBuilder plotBuilderCurrent(extender);
	PlotBuilder plotBuilderVoltage(extender);

	//First we parse the result values, depending on the simulation type 
	std::vector<double> xValues;	
	if (simulationType == ".dc") {
		auto it = resultVectors.find("v-sweep");
		if (it != resultVectors.end()) {
			xValues = resultVectors.at("v-sweep");
			resultVectors.erase(it);
		}
		else
		{
			OT_LOG_E("No v-sweep vector found");
			//ngSpice_Command(const_cast<char*>("quit"));
			return;
		}

		simulationType = "DC";
	}
	else if (simulationType == ".TRAN") {
		auto it = resultVectors.find("time");
		if (it != resultVectors.end()) {
			xValues = resultVectors.at("time");
			resultVectors.erase(it);
		}
		else {
			OT_LOG_E("No time vector found");
			//ngSpice_Command(const_cast<char*>("quit"));
			return;
		}
			
		simulationType = "TRAN";
	}
	else {
		auto it = resultVectors.find("frequency");
		if (it != resultVectors.end()) {
			xValues = resultVectors.at("frequency");
			resultVectors.erase(it);
		}
		else {
			OT_LOG_E("No frequency vector found");
			//ngSpice_Command(const_cast<char*>("quit"));
			return;
		}

		simulationType = "AC";
	}
	
	MetadataParameter parameter;
	for (double& value : xValues)
	{
		parameter.values.push_back(ot::Variable(value));
	}
	xValues.clear();

	// Now i try to find the branch and erase it too
	auto it = resultVectors.find("v1#branch");
	if (it != resultVectors.end()) {
		resultVectors.erase(it);
	}
	else {
		OT_LOG_E("No v1#branch vector found");
	}


	// Here i want to delete the ediff vector from my Result beacuse i dont need it
	while (true) {
			
		auto it2 = std::find_if(resultVectors.begin(), resultVectors.end(),
			[&](const std::pair<const std::string, std::vector<double>>& element) {
				return element.first.find("ediff") != std::string::npos;
			});
		if (it2 != resultVectors.end()) {
				
			resultVectors.erase(it2);
		}
		else {
				
			break;
		}
	}

	std::string _curveFolderPath = solverName + "/" + "Results" + "/" + "1D/Curves";

	ot::PainterRainbowIterator rainbowPainterIt;
	// No i want to get the node vectors of voltage and for each of them i create a curve
	for (auto& it : resultVectors) {
		std::string curveName;
		std::string xLabel;
		std::string xUnit;
		std::string yUnit;
		auto& map = Application::instance()->getNGSpice().netlistNameToCustomNameMap;

		std::string name;
		std::string delimiter = "#branch";
		std::size_t pos = it.first.find(delimiter);

		std::string key = (pos != std::string::npos) ? it.first.substr(0, pos) : it.first;
		if (map.find(key) != map.end()) {
			name = map[key];
		}
		else {
			name = it.first; 
		}

		if (simulationType == "DC") {
			curveName = name + "-DC";
			xLabel = "sweep";
			xUnit = "V";
			yUnit = "V";

				
		}
		else if (simulationType == "TRAN") {
			curveName = name + "-TRAN";
			xLabel = "time";
			xUnit = "ms";
			yUnit = "V";
		}
		else {
			curveName = name + "-AC";
			xLabel = "frequency";
			xUnit = "hz";
			yUnit = "V";
		}

		parameter.unit = xUnit;
		parameter.parameterName = xLabel;
		parameter.typeName = ot::TypeNames::getDoubleTypeName();
		std::shared_ptr<ParameterDescription> parameterDescr(new ParameterDescription(parameter, false));
		

		std::unique_ptr<QuantityDescriptionCurve> quantity(new QuantityDescriptionCurve());
		const std::vector<double>& yValues = it.second;
		for (const double& value : yValues)
		{
			quantity->addDatapoint(ot::Variable(value));
		}
		it.second.clear();

		DatasetDescription dataset;
		dataset.addParameterDescription(parameterDescr);

		ot::Plot1DCurveCfg curveCfg;
		curveCfg.setTitle(curveName);
				
		auto stylePainter = rainbowPainterIt.getNextPainter();
		curveCfg.setLinePen(stylePainter.release());

		std::string yLabel = it.first;
		if (yLabel.find("V(") != std::string::npos || yLabel.find("vd_") != std::string::npos)
		{
			yLabel = "Voltage";
			quantity->setName(yLabel);
			quantity->addValueDescription("", ot::TypeNames::getDoubleTypeName(), yUnit);
			dataset.setQuantityDescription(quantity.release());
			plotBuilderVoltage.addCurve(std::move(dataset), curveCfg, curveName);				
		}
		else
		{
			yLabel = "Current";
			yUnit = "I";
			quantity->setName(yLabel);
			quantity->addValueDescription("", ot::TypeNames::getDoubleTypeName(), yUnit);
			dataset.setQuantityDescription(quantity.release());

			plotBuilderCurrent.addCurve(std::move(dataset), curveCfg, curveName);
		}
	}
	
	// Here i create the plot for all the curves of voltage
	const std::string _plotNameVoltage = "/" + simulationType+"-Voltage";
	const std::string plotFolderVoltage = solverName + "/" + "Results";
	const std::string fullPlotNameVoltage = plotFolderVoltage + _plotNameVoltage;

	// Here i create the plit for all the curves of current
	const std::string _plotNameCurrent = "/" + simulationType+ "-Current";
	const std::string plotFolderCurrent = solverName + "/" + "Results";
	const std::string fullPlotNameCurrent = plotFolderCurrent + _plotNameCurrent;

	// Creating the Plot Entity for Voltage
	if (plotBuilderVoltage.getNumberOfCurves() != 0)
	{
		ot::Plot1DCfg plotCfg;
		plotCfg.setEntityName(fullPlotNameVoltage);
		plotBuilderVoltage.buildPlot(plotCfg, false);			
	}
	// Creating the Plot Entity for Current
	if (plotBuilderCurrent.getNumberOfCurves() != 0)
	{
		ot::Plot1DCfg plotCfg;
		plotCfg.setEntityName(fullPlotNameCurrent);
		plotBuilderCurrent.buildPlot(plotCfg, false);
	}
}

// Setter
void BlockEntityHandler::setPackageName(std::string name) {
	this->_packageName = name;
}

// Getter
const std::string BlockEntityHandler::getPackageName() const {
	return this->_packageName;
}

const std::string BlockEntityHandler::getInitialCircuitName() const {
	return this->m_initialCircuitName;
}
