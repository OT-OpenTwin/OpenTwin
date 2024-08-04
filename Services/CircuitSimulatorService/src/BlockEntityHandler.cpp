// Service header
#include "BlockEntityHandler.h"
// Open twin header
#include "OTCommunication/ActionTypes.h"
#include "CircuitElement.h"
#include "Connection.h"

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
// Third Party Header

//C++
#include <algorithm>

namespace NodeNumbers {
	static unsigned long long nodeNumber = 0;
	static unsigned long long connectionNumber = 1;	
}

void BlockEntityHandler::CreateBlockEntity(const std::string& editorName, const std::string& blockName, ot::Point2DD& position) {
	ClassFactoryBlock factory;
	EntityBase* baseEntity = factory.CreateEntity(blockName);
	assert(baseEntity != nullptr);
	std::shared_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(baseEntity));

	std::string entName = CreateNewUniqueTopologyName(_blockFolder + "/" + editorName, blockEntity->getBlockTitle());
	blockEntity->setName(entName);
	blockEntity->SetServiceInformation(Application::instance()->getBasicServiceInformation());
	blockEntity->setOwningService(OT_INFO_SERVICE_TYPE_CircuitSimulatorService);
	blockEntity->setEntityID(_modelComponent->createEntityUID());

	// Here i want to add the items to the corresponding editor


	blockEntity->SetGraphicsScenePackageName(_packageName);
	

	std::unique_ptr<EntityCoordinates2D> blockCoordinates(new EntityCoordinates2D(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_CircuitSimulatorService));
	blockCoordinates->setCoordinates(position);
	blockCoordinates->StoreToDataBase();

	blockEntity->setCoordinateEntityID(blockCoordinates->getEntityID());

	//Von Blockentity in CircuitEntity casten und createProperties aufrufen
	
	InitSpecialisedCircuitElementEntity(blockEntity);

	blockEntity->StoreToDataBase();
	_modelComponent->addEntitiesToModel({ blockEntity->getEntityID() }, { blockEntity->getEntityStorageVersion() }, { false }, { blockCoordinates->getEntityID() }, { blockCoordinates->getEntityStorageVersion() }, { blockEntity->getEntityID() }, "Added Block: " + blockName);

	
}

void BlockEntityHandler::UpdateBlockPosition(const ot::UID& blockID, const ot::Point2DD& position, const ot::Transform transform, ClassFactory* classFactory) {
	
	std::list<ot::EntityInformation> entityInfos;
	ot::UIDList entityIDList{ blockID };
	_modelComponent->getEntityInformation(entityIDList, entityInfos);
	auto entBase = _modelComponent->readEntityFromEntityIDandVersion(entityInfos.begin()->getID(), entityInfos.begin()->getVersion(), *classFactory);
	std::unique_ptr<EntityBlock> blockEnt(dynamic_cast<EntityBlock*>(entBase));
	
	

	//Here I will update the rotation
	
	auto propertyBase = blockEnt->getProperties().getProperty("Rotation");
	auto propertyRotation = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	propertyRotation->setValue(transform.getRotation());
	

	//Here I update the Flip
	std::map<ot::Transform::FlipState, std::string > stringFlipMap;
	stringFlipMap.insert_or_assign(ot::Transform::NoFlip, "NoFlip");
	stringFlipMap.insert_or_assign(ot::Transform::FlipVertically,"FlipVertically" );
	stringFlipMap.insert_or_assign(ot::Transform::FlipHorizontally,"FlipHorizontally" );

	auto propertyBaseFlip = blockEnt->getProperties().getProperty("Flip");
	auto propertyFlip = dynamic_cast<EntityPropertiesSelection*>(propertyBaseFlip);
	propertyFlip->setValue(stringFlipMap[transform.getFlipStateFlags()]);
	


	
	ot::UID positionID = blockEnt->getCoordinateEntityID();
	entityInfos.clear();
	entityIDList = { positionID };
	_modelComponent->getEntityInformation(entityIDList, entityInfos);
	entBase = _modelComponent->readEntityFromEntityIDandVersion(entityInfos.begin()->getID(), entityInfos.begin()->getVersion(), *classFactory);
	std::unique_ptr<EntityCoordinates2D> coordinateEnt(dynamic_cast<EntityCoordinates2D*>(entBase));
	coordinateEnt->setCoordinates(position);
	coordinateEnt->StoreToDataBase();
	blockEnt->StoreToDataBase();
	
	_modelComponent->addEntitiesToModel({}, {}, {}, {coordinateEnt->getEntityID()}, {coordinateEnt->getEntityStorageVersion()}, {blockID}, "Update BlockItem position");
	const std::string comment = "Property Updated";
	ot::UIDList topoList{blockEnt->getEntityID()};
	ot::UIDList versionList{blockEnt->getEntityStorageVersion()};
	_modelComponent->updateTopologyEntities(topoList, versionList, comment);
}






void BlockEntityHandler::OrderUIToCreateBlockPicker() {
	auto graphicsEditorPackage = BuildUpBlockPicker();
	ot::JsonDocument doc;
	ot::JsonObject pckgObj;
	graphicsEditorPackage->addToJsonObject(pckgObj, doc.GetAllocator());

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_FillItemPicker, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, doc.GetAllocator());

	Application::instance()->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	// Message is queued, no response here
	std::string tmp;
	_uiComponent->sendMessage(true, doc, tmp);
}

std::map<ot::UID, std::shared_ptr<EntityBlock>> BlockEntityHandler::findAllBlockEntitiesByBlockID() {
	std::list<std::string> blockItemNames = _modelComponent->getListOfFolderItems(_blockFolder + "/" + _packageName, true);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(blockItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactoryBlock classFactory;

	std::map<ot::UID, std::shared_ptr<EntityBlock>> blockEntitiesByBlockID;
	for (auto& entityInfo : entityInfos) {
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
		if (baseEntity != nullptr && baseEntity->getClassName() != "EntityBlockConnection") { //Otherwise not a BlockEntity, since ClassFactoryBlock does not handle others 
			std::shared_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(baseEntity));
			assert(blockEntity != nullptr);
			blockEntitiesByBlockID[blockEntity->getEntityID()] = blockEntity;
		}

	}
	return blockEntitiesByBlockID;
}

std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> BlockEntityHandler::findAllEntityBlockConnections() {
	std::list<std::string> connectionItemNames = _modelComponent->getListOfFolderItems("Circuits/" + _packageName + "/Connections");
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(connectionItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactoryBlock classFactory;

	std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> entityBlockConnectionsByBlockID;
	for (auto& entityInfo : entityInfos) {
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
		if (baseEntity != nullptr && baseEntity->getClassName() == "EntityBlockConnection") {
			std::shared_ptr<EntityBlockConnection> blockEntityConnection(dynamic_cast<EntityBlockConnection*>(baseEntity));
			assert(blockEntityConnection != nullptr);
			entityBlockConnectionsByBlockID[blockEntityConnection->getEntityID()] = blockEntityConnection;
		}
	}

	return entityBlockConnectionsByBlockID;

	
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

	std::list< std::shared_ptr<EntityBlock>> entitiesForUpdate;
	for (auto& connection : connections) {
		bool originConnectorIsTypeOut(true), destConnectorIsTypeOut(true);

		std::list<std::string> connectionItems = _modelComponent->getListOfFolderItems("Circuits/" + name + "/Connections");


		//Now I first create the needed parameters for entName
		ot::UID entityID = _modelComponent->createEntityUID();
		
		
		int count = 1;
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
		
		_modelComponent->updateTopologyEntities(topologyEntityIDList, topologyEntityVersionList, "Added new connection to BlockEntities");
		
	}

	
	
	


	

}

void BlockEntityHandler::InitSpecialisedCircuitElementEntity(std::shared_ptr<EntityBlock> blockEntity) {
	EntityBlockCircuitVoltageSource* CircuitElement = dynamic_cast<EntityBlockCircuitVoltageSource*>(blockEntity.get());
	if (CircuitElement != nullptr) {
		CircuitElement->createProperties();
	}

	EntityBlockCircuitResistor* resistor = dynamic_cast<EntityBlockCircuitResistor*>(blockEntity.get());
	if (resistor != nullptr) {
		resistor->createProperties();
	}

	EntityBlockCircuitDiode* diode = dynamic_cast<EntityBlockCircuitDiode*>(blockEntity.get());
	if (diode != nullptr) {
		diode->createProperties();
	}

	EntityBlockCircuitCapacitor* capacitor = dynamic_cast<EntityBlockCircuitCapacitor*>(blockEntity.get());
	if (capacitor != nullptr) {
		capacitor->createProperties();
	}

	EntityBlockCircuitInductor* inductor = dynamic_cast<EntityBlockCircuitInductor*>(blockEntity.get());
	if (inductor != nullptr) {
		inductor->createProperties();
	}
	
}



ot::GraphicsPickerCollectionPackage* BlockEntityHandler::BuildUpBlockPicker() {
	//ot::GraphicsNewEditorPackage* pckg = new ot::GraphicsNewEditorPackage(_packageName, _packageName);
	ot::GraphicsPickerCollectionPackage* pckg = new ot::GraphicsPickerCollectionPackage();
	ot::GraphicsPickerCollectionCfg* a = new ot::GraphicsPickerCollectionCfg("CircuitElements", "Circuit Elements");
	ot::GraphicsPickerCollectionCfg* a1 = new ot::GraphicsPickerCollectionCfg("PassiveElements", "Passive Elements");
	ot::GraphicsPickerCollectionCfg* a2 = new ot::GraphicsPickerCollectionCfg("Meter Elements", "Meter Elements");
	a->addChildCollection(a1);
	a->addChildCollection(a2);

	EntityBlockCircuitVoltageSource element(0, nullptr, nullptr, nullptr,nullptr, "");
	EntityBlockCircuitResistor resistor(0, nullptr, nullptr, nullptr, nullptr, "");
	EntityBlockCircuitDiode diode(0, nullptr, nullptr, nullptr, nullptr, "");
	EntityBlockCircuitVoltageMeter voltMeter(0, nullptr, nullptr, nullptr, nullptr, "");
	EntityBlockCircuitCurrentMeter currentMeter(0, nullptr, nullptr, nullptr, nullptr, "");
	EntityBlockCircuitCapacitor capacitor(0, nullptr, nullptr, nullptr, nullptr, "");
	EntityBlockCircuitInductor inductor(0, nullptr, nullptr, nullptr, nullptr, "");

	
	a1->addItem(element.getClassName(), element.CreateBlockHeadline(), "CircuitElementImages/VoltageSource.png");
	a1->addItem(resistor.getClassName(), resistor.CreateBlockHeadline(), "CircuitElementImages/ResistorBG.png");
	a1->addItem(diode.getClassName(), diode.CreateBlockHeadline(), "CircuitElementImages/Diod2.png");
	a1->addItem(capacitor.getClassName(), capacitor.CreateBlockHeadline(), "CircuitElementImages/Capacitor.png");
	a1->addItem(inductor.getClassName(), inductor.CreateBlockHeadline(), "CircuitElementImages/Inductor.png");

	a2->addItem(voltMeter.getClassName(), voltMeter.CreateBlockHeadline(), "CircuitElementImages/VoltMeter.png");
	a2->addItem(currentMeter.getClassName(), currentMeter.CreateBlockHeadline(), "CircuitElementImages/CurrentMeter.png");
	pckg->addCollection(a);

	

	return pckg;
}

void BlockEntityHandler::createResultCurves(std::string solverName,std::string simulationType,std::string circuitName) {
	
		std::map<std::string, std::vector<double>> resultVectors = SimulationResults::getInstance()->getResultMap();
		std::list<std::pair<ot::UID, std::string>> curves;
		ot::UIDList topoEntID, topoEntVers, dataEntID, dataEntVers, dataEntParent;
		std::list<bool> forceVis;
		const int colorID(0);

		std::vector<double> xValues;
	
		if (simulationType == ".dc") {
			auto it = resultVectors.find("v-sweep");
			xValues = resultVectors.at("v-sweep");
			if (it != resultVectors.end()) {
				resultVectors.erase(it);
			}
		}
		else if (simulationType == ".TRAN") {
			auto it = resultVectors.find("time");
			xValues = resultVectors.at("time");
			if (it != resultVectors.end()) {
				resultVectors.erase(it);
				
			}
		}
		else {
			auto it = resultVectors.find("frequency");
			xValues = resultVectors.at("frequency");
			if (it != resultVectors.end()) {
				resultVectors.erase(it);
			}
			
		}
			
		

		//First i try to find the xValues of the Curve and that are the sweep 
		//Then i fill my vector with it and erase it out of the map

		

		//Now i try to find the branch and erase it too
		auto it = resultVectors.find("v1#branch");
		if (it != resultVectors.end()) {
			resultVectors.erase(it);
		}


		//Here i want to delete the ediff vector from my Result beacuse i dont need it
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

		// No i want to get the node vectors of voltage and for each of them i create a curve
		for (auto it : resultVectors) {
			std::string curveName;
			std::string fullCurveName;
			std::string xLabel;
			std::string xUnit;
			std::string yUnit;
	
			if (simulationType == ".dc") {
				curveName = it.first + "-DC";
				fullCurveName = _curveFolderPath + "/" + curveName;
				xLabel = "sweep";
				xUnit = "V";
				yUnit = "V";
				
			}
			else if (simulationType == ".TRAN") {
				curveName = it.first + "-TRAN";
				fullCurveName = _curveFolderPath +  "/" + curveName;
				xLabel = "time";
				xUnit = "ms";
				yUnit = "V";
			}
			else {
				curveName = it.first + "-AC";
				fullCurveName = _curveFolderPath +  "/" + curveName;
				xLabel = "frequency";
				xUnit = "hz";
				yUnit = "V";
			}

			std::string yLabel = it.first;
			EntityResult1DCurve* curve = _modelComponent->addResult1DCurveEntity(fullCurveName, xValues, it.second, {}, xLabel, xUnit, yLabel, yUnit, colorID, true);
			curves.push_back(std::pair<ot::UID, std::string>(curve->getEntityID(), curveName));

			topoEntID.push_back(curve->getEntityID());
			topoEntVers.push_back(curve->getEntityStorageVersion());
			dataEntID.push_back((ot::UID)curve->getCurveDataStorageId());
			dataEntVers.push_back((ot::UID)curve->getCurveDataStorageId());
			dataEntParent.push_back(curve->getEntityID());
			forceVis.push_back(false);
		}
	
		//Here i create the plot for all the curves
		const std::string _plotName = "/" + simulationType + "-Simulation";
		const std::string plotFolder = solverName + "/" + "Results";
		const std::string fullPlotName = plotFolder + _plotName;

		//Creating the Plot Entity
		EntityResult1DPlot* plotID = _modelComponent->addResult1DPlotEntity(fullPlotName, "Result Plot", curves);
		topoEntID.push_back(plotID->getEntityID());
		topoEntVers.push_back(plotID->getEntityStorageVersion());
		forceVis.push_back(false);
		
		_modelComponent->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEntID, dataEntVers, dataEntParent, "Created plot");
}

//Setter
void BlockEntityHandler::setPackageName(std::string name) {
	this->_packageName = name;
}

//Getter
const std::string BlockEntityHandler::getPackageName() const {
	return this->_packageName;
}