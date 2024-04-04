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
// Third Party Header

namespace NodeNumbers
{
	static unsigned long long nodeNumber = 0;
	static unsigned long long connectionNumber = 1;
	
}

void BlockEntityHandler::CreateBlockEntity(const std::string& editorName, const std::string& blockName, ot::Point2DD& position)
{
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



void BlockEntityHandler::OrderUIToCreateBlockPicker()
{
	auto graphicsEditorPackage = BuildUpBlockPicker();
	ot::JsonDocument doc;
	ot::JsonObject pckgObj;
	graphicsEditorPackage->addToJsonObject(pckgObj, doc.GetAllocator());

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, doc.GetAllocator());

	Application::instance()->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	// Message is queued, no response here
	_uiComponent->sendMessage(true, doc);
}

std::map<ot::UID, std::shared_ptr<EntityBlock>> BlockEntityHandler::findAllBlockEntitiesByBlockID()
{
	std::list<std::string> blockItemNames = _modelComponent->getListOfFolderItems(_blockFolder + "/" + _packageName, true);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(blockItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactoryBlock classFactory;

	std::map<ot::UID, std::shared_ptr<EntityBlock>> blockEntitiesByBlockID;
	for (auto& entityInfo : entityInfos)
	{
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
		if (baseEntity != nullptr && baseEntity->getClassName() != "EntityBlockConnection") //Otherwise not a BlockEntity, since ClassFactoryBlock does not handle others
		{
			std::shared_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(baseEntity));
			assert(blockEntity != nullptr);
			blockEntitiesByBlockID[blockEntity->getEntityID()] = blockEntity;
		}
	}
	return blockEntitiesByBlockID;
}

std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> BlockEntityHandler::findAllEntityBlockConnections()
{
	std::list<std::string> connectionItemNames = _modelComponent->getListOfFolderItems("Circuits/" + _packageName + "/Connections");
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(connectionItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactoryBlock classFactory;

	std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> entityBlockConnectionsByBlockID;
	for (auto& entityInfo : entityInfos)
	{
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
		if (baseEntity != nullptr && baseEntity->getClassName() == "EntityBlockConnection")
		{
			std::shared_ptr<EntityBlockConnection> blockEntityConnection(dynamic_cast<EntityBlockConnection*>(baseEntity));
			assert(blockEntityConnection != nullptr);
			entityBlockConnectionsByBlockID[blockEntityConnection->getEntityID()] = blockEntityConnection;
		}
	}

	return entityBlockConnectionsByBlockID;

	
}

bool BlockEntityHandler::connectorHasTypeOut(std::shared_ptr<EntityBlock> blockEntity, const std::string& connectorName)
{
	auto allConnectors = blockEntity->getAllConnectorsByName();
	const ot::ConnectorType connectorType = allConnectors[connectorName].getConnectorType();
	if (connectorType == ot::ConnectorType::UNKNOWN) { OT_LOG_EAS("Unset connectortype of connector: " + allConnectors[connectorName].getConnectorName()); }
	if (connectorType == ot::ConnectorType::In || connectorType == ot::ConnectorType::InOptional)
	{
		return false;
	}
	else
	{
		return true;
	}
}



void BlockEntityHandler::AddBlockConnection(const std::list<ot::GraphicsConnectionCfg>& connections,std::string name)
{
	
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
	for (auto& connection : connections)
	{
		bool originConnectorIsTypeOut(true), destConnectorIsTypeOut(true);

		std::list<std::string> connectionItems = _modelComponent->getListOfFolderItems("Circuits/" + name + "/Connections");


		//Now I first create the needed parameters for entName
		ot::UID entityID = _modelComponent->createEntityUID();
		
		
		int count = 1;
		std::string connectionName;
		do
		{
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
		connectionCfg.setOriginConnectable(connection.originConnectable());
		connectionCfg.setDestConnectable(connection.destConnectable());

		
		//Now i set the attirbutes of connectionEntity
		connectionEntity->setConnectionCfg(connectionCfg);
		connectionEntity->setName(connectionName);
		connectionEntity->SetGraphicsScenePackageName(name);
		connectionEntity->SetServiceInformation(Application::instance()->getBasicServiceInformation());
		connectionEntity->setOwningService(OT_INFO_SERVICE_TYPE_CircuitSimulatorService);
		connectionEntity->StoreToDataBase();

		topologyEntityIDList.push_back(connectionEntity->getEntityID());
		topologyEntityVersionList.push_back(connectionEntity->getEntityStorageVersion());


		if (blockEntitiesByBlockID.find(connection.getOriginUid()) != blockEntitiesByBlockID.end())
		{
			auto& blockEntity = blockEntitiesByBlockID[connection.getOriginUid()];

			originConnectorIsTypeOut = connectorHasTypeOut(blockEntity, connection.originConnectable());
		}
		else
		{
			OT_LOG_EAS("Could not create connection since block " + std::to_string(connection.getOriginUid()) + " was not found");
			continue;
		}

		if (blockEntitiesByBlockID.find(connection.getDestinationUid()) != blockEntitiesByBlockID.end())
		{
			auto& blockEntity = blockEntitiesByBlockID[connection.getDestinationUid()];
			destConnectorIsTypeOut = connectorHasTypeOut(blockEntity, connection.destConnectable());
		}
		else
		{
			OT_LOG_EAS("Could not create connection since block " + std::to_string(connection.getDestinationUid()) + " was not found.");
			continue;
		}

		if (originConnectorIsTypeOut != destConnectorIsTypeOut)
		{
			blockEntitiesByBlockID[connection.getOriginUid()]->AddConnection(connectionEntity->getEntityID());
			entitiesForUpdate.push_back(blockEntitiesByBlockID[connection.getOriginUid()]);
			blockEntitiesByBlockID[connection.getDestinationUid()]->AddConnection(connectionEntity->getEntityID());
			entitiesForUpdate.push_back(blockEntitiesByBlockID[connection.getDestinationUid()]);

			/*auto it = Application::instance()->getNGSpice().getMapOfCircuits().find(name);*/
			/*Connection conn(connection);
			
			conn.setNodeNumber(std::to_string(NodeNumbers::nodeNumber++));

			it->second.addConnection(connection.originUid(),conn);
			it->second.addConnection(connection.destUid(),conn);*/

		}
		else
		{
			_uiComponent->displayMessage("Cannot create connection. One port needs to be an ingoing port while the other is an outgoing port.\n");
		}
	}

	


	

	if (entitiesForUpdate.size() != 0)
	{

		for (auto entityForUpdate : entitiesForUpdate)
		{
			entityForUpdate->StoreToDataBase();
			topologyEntityIDList.push_back(entityForUpdate->getEntityID());
			topologyEntityVersionList.push_back(entityForUpdate->getEntityStorageVersion());
		}
		
		_modelComponent->updateTopologyEntities(topologyEntityIDList, topologyEntityVersionList, "Added new connection to BlockEntities");
		
	}

	
	
	


	

}

void BlockEntityHandler::InitSpecialisedCircuitElementEntity(std::shared_ptr<EntityBlock> blockEntity)
{
	EntityBlockCircuitVoltageSource* CircuitElement = dynamic_cast<EntityBlockCircuitVoltageSource*>(blockEntity.get());
	if (CircuitElement != nullptr)
	{
		CircuitElement->createProperties();
	}

	EntityBlockCircuitResistor* resistor = dynamic_cast<EntityBlockCircuitResistor*>(blockEntity.get());
	if (resistor != nullptr)
	{
		resistor->createProperties();
	}
	
}



ot::GraphicsNewEditorPackage* BlockEntityHandler::BuildUpBlockPicker()
{
	ot::GraphicsNewEditorPackage* pckg = new ot::GraphicsNewEditorPackage(_packageName, _packageName);
	ot::GraphicsPickerCollectionCfg* a = new ot::GraphicsPickerCollectionCfg("CircuitElements", "Circuit Elements");
	ot::GraphicsPickerCollectionCfg* a1 = new ot::GraphicsPickerCollectionCfg("PassiveElements", "Passive Elements");
	a->addChildCollection(a1);
	EntityBlockCircuitVoltageSource element(0, nullptr, nullptr, nullptr,nullptr, "");
	EntityBlockCircuitResistor resistor(0, nullptr, nullptr, nullptr, nullptr, "");
	a1->addItem(element.getClassName(), element.CreateBlockHeadline(), "CircuitElementImages/VoltageSource.png");
	a1->addItem(resistor.getClassName(), resistor.CreateBlockHeadline(), "CircuitElementImages/ResistorBG.png");

	pckg->addCollection(a);

	Circuit circuit;
	circuit.setEditorName(pckg->title());
	circuit.setId(pckg->name());
	Application::instance()->getNGSpice().getMapOfCircuits().insert_or_assign(pckg->name(), circuit);

	return pckg;
}

void BlockEntityHandler::createResultCurves()
{
	
		std::map<std::string, std::vector<double>> resultVectors = SimulationResults::getInstance()->getResultMap();
		std::list<std::pair<ot::UID, std::string>> curves;
		ot::UIDList topoEntID, topoEntVers, dataEntID, dataEntVers, dataEntParent;
		std::list<bool> forceVis;
		const int colorID(0);

		std::vector<double> xValues;
		std::vector<double> yValues_1;
		std::vector<double> yValues_2;

		//Filling the needed vectors with data from resultVectorsMap

		for (auto it : resultVectors)
		{
			if (it.first == "v-sweep")
			{
				for (int i = 0; i < it.second.size(); i++)
				{
					xValues.push_back(it.second.at(i));
				}
			}
			else if (it.first == "v1#branch")
			{
				continue;
			}
			else if (it.first == "V(1)")
			{
				for (int i = 0; i < it.second.size(); i++)
				{
					yValues_1.push_back(it.second.at(i));
				}
			}
			else
			{
				for (int i = 0; i < it.second.size(); i++)
				{
					yValues_2.push_back(it.second.at(i));
				}
			}
		}

		//Creating all needed Names for resultEntity creation
		const std::string curveName1 = "V(1)";
		const std::string curveName2 = "V(2)";
		const std::string _plotName = "DC-Simulation";

		const std::string fullCurveName1 = _curveFolderPath + "/" + curveName1;
		const std::string fullCurveName2 = _curveFolderPath + "/" + curveName2;

		const std::string plotFolder = _resultFolder + "1D/Plots/";
		const std::string fullPlotName = plotFolder + _plotName;

		//creating the resultCurves
		EntityResult1DCurve* curve1 = _modelComponent->addResult1DCurveEntity(fullCurveName1, xValues, yValues_1, {}, "sweep", "V", "V(1)", "V", colorID, true);
		EntityResult1DCurve* curve2 = _modelComponent->addResult1DCurveEntity(fullCurveName2, xValues, yValues_2, {}, "sweep", "V", "V(2)", "V", colorID, true);

		//adding the curves to list of curves
		curves.push_back(std::pair<ot::UID, std::string>(curve1->getEntityID(), curveName1));
		curves.push_back(std::pair<ot::UID, std::string>(curve2->getEntityID(), curveName2));

		topoEntID.push_back(curve1->getEntityID());
		topoEntVers.push_back(curve1->getEntityStorageVersion());
		dataEntID.push_back((ot::UID)curve1->getCurveDataStorageId());
		dataEntVers.push_back((ot::UID)curve1->getCurveDataStorageId());
		dataEntParent.push_back(curve1->getEntityID());
		forceVis.push_back(false);

		topoEntID.push_back(curve2->getEntityID());
		topoEntVers.push_back(curve2->getEntityStorageVersion());
		dataEntID.push_back((ot::UID)curve2->getCurveDataStorageId());
		dataEntVers.push_back((ot::UID)curve2->getCurveDataStorageId());
		dataEntParent.push_back(curve2->getEntityID());
		forceVis.push_back(false);

		//Creating the Plot Entity
		EntityResult1DPlot* plotID = _modelComponent->addResult1DPlotEntity(fullPlotName, "Result Plot", curves);
		topoEntID.push_back(plotID->getEntityID());
		topoEntVers.push_back(plotID->getEntityStorageVersion());
		forceVis.push_back(false);
		
		_modelComponent->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEntID, dataEntVers, dataEntParent, "Created plot");
}

//Setter
void BlockEntityHandler::setPackageName(std::string name)
{
	this->_packageName = name;
}

//Getter
std::string BlockEntityHandler::getPackageName()
{
	return this->_packageName;
}