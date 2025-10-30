// @otlicense
// File: BlockEntityHandler.cpp
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
#include "OTCore/EntityName.h"

// Third Party Header

//C++
#include <algorithm>
#include <queue>

std::shared_ptr<EntityBlock> BlockEntityHandler::CreateBlockEntity(const std::string& editorName, const std::string& blockName, const ot::Point2DD& position) {
	EntityBase* baseEntity = EntityFactory::instance().create(blockName);
	assert(baseEntity != nullptr);
	std::shared_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(baseEntity));

	
	blockEntity->setEditable(true);
	blockEntity->setServiceInformation(Application::instance()->getBasicServiceInformation());
	blockEntity->setOwningService(OT_INFO_SERVICE_TYPE_CircuitSimulatorService);
	blockEntity->setEntityID(_modelComponent->createEntityUID());
	// Here i want to add the items to the corresponding editor	

	std::unique_ptr<EntityCoordinates2D> blockCoordinates(new EntityCoordinates2D(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_CircuitSimulatorService));
	blockCoordinates->setCoordinates(position);
	blockCoordinates->storeToDataBase();

	blockEntity->setCoordinateEntityID(blockCoordinates->getEntityID());
	
	

	//Von Blockentity in CircuitEntity casten und createProperties aufrufen

	std::string elementName = InitSpecialisedCircuitElementEntity(blockEntity);
	if (elementName != "") {
	//Create block Titles
	//First get a list of all folder items of the Circuit folder
		std::list<std::string> circuitItems = ot::ModelServiceAPI::getListOfFolderItems(editorName);
		// Create a unique name for the new circuit item
		int count = 1;
		std::string circuitItemName;
		std::string circuitAbbraviationName;
		do {
			circuitAbbraviationName = editorName +"/"+ elementName + std::to_string(count);
			count++;
		} while (std::find(circuitItems.begin(), circuitItems.end(), circuitAbbraviationName) != circuitItems.end());
		blockEntity->setName(circuitAbbraviationName);
	}
	else {
		std::string entName = CreateNewUniqueTopologyName(editorName, blockEntity->getBlockTitle());
		blockEntity->setName(entName);
	}



	blockEntity->storeToDataBase();
	ot::ModelServiceAPI::addEntitiesToModel({ blockEntity->getEntityID() }, { blockEntity->getEntityStorageVersion() }, { false }, { blockCoordinates->getEntityID() }, { blockCoordinates->getEntityStorageVersion() }, { blockEntity->getEntityID() }, "Added Block: " + blockName);

	return blockEntity;
}

void BlockEntityHandler::OrderUIToCreateBlockPicker() {
	ot::JsonDocument doc;
	
	auto pckg = BuildUpBlockPicker();
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_FillItemPicker, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, ot::JsonObject(pckg, doc.GetAllocator()), doc.GetAllocator());
	
	Application::instance()->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	// Message is queued, no response here
	std::string tmp;
	_uiComponent->sendMessage(true, doc, tmp);
}

std::map<ot::UID, std::shared_ptr<EntityBlock>> BlockEntityHandler::findAllBlockEntitiesByBlockID(const std::string& _folderName) {
	std::list<std::string> blockItemNames = ot::ModelServiceAPI::getListOfFolderItems(_folderName, true);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(blockItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	
	std::map<ot::UID, std::shared_ptr<EntityBlock>> blockEntitiesByBlockID;
	for (auto& entityInfo : entityInfos) {
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion());
		if (baseEntity != nullptr && baseEntity->getClassName() != "EntityBlockConnection") { //Otherwise not a BlockEntity, since ClassFactoryBlock does not handle others 
			std::shared_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(baseEntity));
			if (blockEntity != nullptr) {
				blockEntitiesByBlockID[blockEntity->getEntityID()] = blockEntity;
			}
		}

	}
	return blockEntitiesByBlockID;
}

std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> BlockEntityHandler::findAllEntityBlockConnections(const std::string& _folderName) {
	const std::string fullFolderName = _folderName + "/" + m_connectionsFolder;
	std::list<std::string> connectionItemNames = ot::ModelServiceAPI::getListOfFolderItems(fullFolderName);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(connectionItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);

	std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> entityBlockConnectionsByBlockID;
	EntityBlockConnection temp;
	for (auto& entityInfo : entityInfos) {
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion());
		if (baseEntity != nullptr && baseEntity->getClassName() == temp.getClassName()) {
			std::shared_ptr<EntityBlockConnection> blockEntityConnection(dynamic_cast<EntityBlockConnection*>(baseEntity));
			if (blockEntityConnection != nullptr) {
				entityBlockConnectionsByBlockID[blockEntityConnection->getEntityID()] = blockEntityConnection;
			}
		}
	}

	return entityBlockConnectionsByBlockID;	
}

std::shared_ptr<EntityFileText> BlockEntityHandler::getCircuitModel(const std::string& _folderName,std::string _modelName) {
	ot::EntityInformation circuitModelInfo;
	ot::ModelServiceAPI::getEntityInformation("Circuit Models/" + _folderName + "/" + _modelName, circuitModelInfo);

	auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(circuitModelInfo.getEntityID(), circuitModelInfo.getEntityVersion());
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

void BlockEntityHandler::addBlockConnection(const std::list<ot::GraphicsConnectionCfg>& _connections,std::string _baseFolderName) {
	auto blockEntitiesByBlockID = findAllBlockEntitiesByBlockID(_baseFolderName);

	std::list<ot::UID> topologyEntityIDList;
	std::list<ot::UID> topologyEntityVersionList;
	std::list<bool> topologyEntityForceVisible = { false };
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataEntityVersionList;
	std::list<ot::UID> dataEntityParentList;

	std::string blockName = "EntityBlockConnection";
	int count = 1;
	std::list< EntityBlockConnection> entitiesForUpdate;
	const std::string connectionFolderName = _baseFolderName + "/" + m_connectionsFolder;
	for (auto& connection : _connections) {

		std::list<std::string> connectionItems = ot::ModelServiceAPI::getListOfFolderItems(connectionFolderName);

		//Now I first create the needed parameters for entName
		ot::UID entityID = _modelComponent->createEntityUID();
		
		// Use a manual counter because all connections are added to the model at the end.
		// CreateNewUniqueEntityName only considers connections already in the model,
        // which could otherwise result in duplicate names.
		std::string connectionName;
		do {
			connectionName = connectionFolderName + "/Connection" + std::to_string(count);
			count++;
		} while (std::find(connectionItems.begin(), connectionItems.end(), connectionName) != connectionItems.end());



		//std::string connectionName = ot::EntityName::createUniqueEntityName(connectionFolderName, "Connection", connectionItems);
	
		//Here i create the connectionEntity
		EntityBlockConnection connectionEntity(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_CircuitSimulatorService);
		connectionEntity.createProperties();
		
		
		//Now i create the GraphicsConnectionCfg and set it with the information
		ot::GraphicsConnectionCfg newConnection(connection);
		newConnection.setUid(connectionEntity.getEntityID());
		newConnection.setLineShape(ot::GraphicsConnectionCfg::ConnectionShape::AutoXYLine);
		newConnection.setLinePainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemConnection));
		newConnection.setOriginPos(connection.getOriginPos());
		newConnection.setDestPos(connection.getDestPos());

		//Now i set the attirbutes of connectionEntity
		connectionEntity.setConnectionCfg(newConnection);
		connectionEntity.setName(connectionName);
		connectionEntity.setGraphicsScenePackageChildName(m_connectionsFolder);
		connectionEntity.setServiceInformation(Application::instance()->getBasicServiceInformation());
		connectionEntity.setOwningService(OT_INFO_SERVICE_TYPE_CircuitSimulatorService);

		
		if (blockEntitiesByBlockID.find(newConnection.getOriginUid()) != blockEntitiesByBlockID.end()) {
			auto& blockEntity = blockEntitiesByBlockID[newConnection.getOriginUid()];
		}
		else {
			OT_LOG_EAS("Could not create connection since block " + std::to_string(newConnection.getOriginUid()) + " was not found");
			continue;
		}

		if (blockEntitiesByBlockID.find(newConnection.getDestinationUid()) != blockEntitiesByBlockID.end()) {
			auto& blockEntity = blockEntitiesByBlockID[newConnection.getDestinationUid()];
		}
		else {
			OT_LOG_EAS("Could not create connection since block " + std::to_string(newConnection.getDestinationUid()) + " was not found.");
			continue;
		}
		
		entitiesForUpdate.push_back(connectionEntity);
	}

	for (auto& entityForUpdate : entitiesForUpdate) {
		entityForUpdate.storeToDataBase();
		topologyEntityIDList.push_back(entityForUpdate.getEntityID());
		topologyEntityVersionList.push_back(entityForUpdate.getEntityStorageVersion());
	}
		
	ot::ModelServiceAPI::updateTopologyEntities(topologyEntityIDList, topologyEntityVersionList, "Added new connection to BlockEntities");
		
	

}

void BlockEntityHandler::addConnectionToConnection(const std::list<ot::GraphicsConnectionCfg>& _connections, std::string _editorName, ot::Point2DD _pos)
{
	
	auto blockEntitiesByBlockID = findAllBlockEntitiesByBlockID(_editorName);

	auto connectionEntitiesByID = findAllEntityBlockConnections(_editorName);
	std::list<std::string> entitiesToDelete;
	std::list<ot::UID> topologyEntityIDList;
	std::list<ot::UID> topologyEntityVersionList;

	std::queue<std::pair<std::string,std::shared_ptr<EntityBlock>>> connectedElements;
	std::list<ot::GraphicsConnectionCfg> connectionsNew;
	
	for (auto connection : _connections)
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

		entitiesToDelete.push_back(connectionToDelete->getName());

		// Now i remove the connections from model
		ot::ModelServiceAPI::deleteEntitiesFromModel(entitiesToDelete);

		// As next step i need to add the intersection item 
		std::shared_ptr<EntityBlock> connector = CreateBlockEntity(_editorName, "EntityBlockCircuitConnector", _pos);

		//Now i create a GraphicsConnectionCfg for all elements
		while (!connectedElements.empty()) {
			ot::GraphicsConnectionCfg temp(connection);
			temp.setDestUid(connector->getEntityID());
			temp.setDestConnectable(connector->getName());
			temp.setOriginUid(connectedElements.front().second->getEntityID());
			temp.setOriginConnectable(connectedElements.front().first);
			connectionsNew.push_back(temp);
			connectedElements.pop();
		}

		addBlockConnection(connectionsNew, _editorName);
	}
}

std::string BlockEntityHandler::InitSpecialisedCircuitElementEntity(std::shared_ptr<EntityBlock> blockEntity) {
	EntityBlockCircuitElement* element = dynamic_cast<EntityBlockCircuitElement*>(blockEntity.get());

	if (element != nullptr) {
		// Get the CircuitModel folder infos
		const std::string circuitModelFolderName = "Circuit Models/" + element->getFolderName();
		ot::EntityInformation entityInfo;
		ot::ModelServiceAPI::getEntityInformation(circuitModelFolderName, entityInfo);

	
		element->createProperties(circuitModelFolderName, entityInfo.getEntityID());
		return element->getTypeAbbreviation();
		
	} else if(blockEntity->getClassName() != "EntityBlockCircuitGND" && blockEntity->getClassName() != "EntityBlockCircuitConnector") {
		OT_LOG_E("EntityBlockCircuitElement is null");
	}
	
	return "";
}



ot::GraphicsPickerCollectionPackage BlockEntityHandler::BuildUpBlockPicker() {
	ot::GraphicsPickerCollectionPackage graphicsPicker;
	ot::GraphicsPickerCollectionCfg a("CircuitElements", "Circuit Elements");
	ot::GraphicsPickerCollectionCfg a1("PassiveElements", "Passive Elements");
	ot::GraphicsPickerCollectionCfg a2("Meter Elements", "Meter Elements");
	ot::GraphicsPickerCollectionCfg a3("Sources", "Sources");
	ot::GraphicsPickerCollectionCfg a4("ActiveElements", "Active Elements");

	EntityBlockCircuitVoltageSource element;
	EntityBlockCircuitResistor resistor;
	EntityBlockCircuitDiode diode;
	EntityBlockCircuitVoltageMeter voltMeter;
	EntityBlockCircuitCurrentMeter currentMeter;
	EntityBlockCircuitCapacitor capacitor;
	EntityBlockCircuitInductor inductor;
	EntityBlockCircuitGND gndElement;
	EntityBlockCircuitTransmissionLine transmissionLine;
	
	a1.addItem(resistor.getClassName(), resistor.createBlockHeadline(), "CircuitElementImages/ResistorBG.png");
	a1.addItem(capacitor.getClassName(), capacitor.createBlockHeadline(), "CircuitElementImages/Capacitor.png");
	a1.addItem(inductor.getClassName(), inductor.createBlockHeadline(), "CircuitElementImages/Inductor.png");
	a1.addItem(gndElement.getClassName(), gndElement.createBlockHeadline(), "CircuitElementImages/GND.png");
	a1.addItem(transmissionLine.getClassName(), transmissionLine.createBlockHeadline(), "CircuitElementImages/TranLine.png");

	a2.addItem(voltMeter.getClassName(), voltMeter.createBlockHeadline(), "CircuitElementImages/VoltMeter.png");
	a2.addItem(currentMeter.getClassName(), currentMeter.createBlockHeadline(), "CircuitElementImages/CurrentMeter.png");

	a3.addItem(element.getClassName(), element.createBlockHeadline(), "CircuitElementImages/VoltageSource.png");
	

	a4.addItem(diode.getClassName(), diode.createBlockHeadline(), "CircuitElementImages/Diod2.png");

	a.addChildCollection(std::move(a1));
	a.addChildCollection(std::move(a2));
	a.addChildCollection(std::move(a3));
	a.addChildCollection(std::move(a4));

	graphicsPicker.addCollection(std::move(a));
	return graphicsPicker;
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

	// Here i create the plot for all the curves of voltage
	const std::string _plotNameVoltage = "/" + simulationType + "-Voltage";
	const std::string plotFolderVoltage = solverName + "/" + "Results";
	const std::string fullPlotNameVoltage = plotFolderVoltage + _plotNameVoltage;

	// Here i create the plit for all the curves of current
	const std::string _plotNameCurrent = "/" + simulationType + "-Current";
	const std::string plotFolderCurrent = solverName + "/" + "Results";
	const std::string fullPlotNameCurrent = plotFolderCurrent + _plotNameCurrent;

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
		curveCfg.setLinePenPainter(stylePainter.release());

		std::string yLabel = it.first;
		if (yLabel.find("V(") != std::string::npos || yLabel.find("vd_") != std::string::npos)
		{
			curveCfg.setEntityName(fullPlotNameVoltage + "/" + curveName);
			yLabel = "Voltage";
			quantity->setName(yLabel);
			quantity->addValueDescription("", ot::TypeNames::getDoubleTypeName(), yUnit);
			dataset.setQuantityDescription(quantity.release());
			plotBuilderVoltage.addCurve(std::move(dataset), curveCfg, curveName);				
		}
		else
		{
			curveCfg.setEntityName(fullPlotNameCurrent + "/" + curveName);
			yLabel = "Current";
			yUnit = "I";
			quantity->setName(yLabel);
			quantity->addValueDescription("", ot::TypeNames::getDoubleTypeName(), yUnit);
			dataset.setQuantityDescription(quantity.release());

			plotBuilderCurrent.addCurve(std::move(dataset), curveCfg, curveName);
		}
	}
	
	

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
