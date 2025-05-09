#include "MicroServiceSolver.h"

#include "EntitySolverFITTD.h"
#include "EntityResult3D.h"
#include "EntityResult3DData.h"
#include "EntitySignalType.h"
#include "EntityResultText.h"

#include "FolderNames.h"
#include "PhysicalQuantities.h"

#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

MicroServiceSolver::MicroServiceSolver(std::string solverName, int serviceID, int sessionCount, ClassFactory& _classFactory) :
	solverName(solverName),
	modelComponent(nullptr), 
	meshDataEntityID(0),
	meshDataVersion(0),
	solverEntityID(0),
	classFactory(&_classFactory)
{

}

MicroServiceSolver::~MicroServiceSolver()
{
	delete _units;
	_units = nullptr;
}

/////////////////////////////////////////////////////////////////
// General setter functions
/////////////////////////////////////////////////////////////////


void MicroServiceSolver::setDataBaseURL(const std::string &dbURL)
{
	dataBaseURL = dbURL;
}

void MicroServiceSolver::setProjectName(const std::string &prjName)
{
	projectName = prjName;
}

void MicroServiceSolver::setUIComponent(ot::components::UiComponent *uiComp)
{
	uiComponent = uiComp;
}

void MicroServiceSolver::setModelComponent(ot::components::ModelComponent *mdlComp)
{
	modelComponent = mdlComp;
}

void MicroServiceSolver::setModelStateVersion(const std::string &modelVersion)
{
	modelStateVersion = modelVersion;
}

void MicroServiceSolver::setMeshDataEntityID(ot::UID entityID)
{
	meshDataEntityID = entityID;
}

void MicroServiceSolver::setMeshDataEntityVersionID(ot::UID entityID)
{
	meshDataVersion = entityID;
}

void MicroServiceSolver::setSolverTopologyBase(const std::string & base)
{
	solverName = base;
}

void MicroServiceSolver::setSolverEntityID(ot::UID entityID)
{
	solverEntityID = entityID;
}


/////////////////////////////////////////////////////////////////
// Helper functions
/////////////////////////////////////////////////////////////////

void MicroServiceSolver::displayMessage(const std::string &msg)
{
	assert(uiComponent != nullptr);

	if (uiComponent != nullptr)
	{
		uiComponent->displayMessage(msg);
	}
}

void MicroServiceSolver::setProgressInformation(std::string message, bool continuous)
{
	uiComponent->setProgressInformation(message, continuous);
}

void MicroServiceSolver::setProgress(int percentage)
{
	uiComponent->setProgress(percentage);
}

void MicroServiceSolver::closeProgressInformation(void)
{
	uiComponent->closeProgressInformation();
}

std::list<ot::UID> MicroServiceSolver::getResultTopologyIDList(void)
{
	return topologyEntityIDList;
}

std::list<ot::UID> MicroServiceSolver::getResultTopologyVersionList(void)
{
	return topologyEntityVersionList;
}

std::list<bool> MicroServiceSolver::getResultForceVisibleList(void)
{
	return topologyEntityForceVisibleList;
}

std::list<ot::UID> MicroServiceSolver::getResultDataIDList(void)
{
	return dataEntityIDList;
}

std::list<ot::UID> MicroServiceSolver::getResultDataVersionList(void)
{
	return dataEntityVersionList;
}

std::list<ot::UID> MicroServiceSolver::getResultDataParentList(void)
{
	return dataEntityParentList;
}

EntityBase * MicroServiceSolver::LoadEntityFromName(std::string name)
{
	ot::EntityInformation entityInformation;
	ot::ModelServiceAPI::getEntityInformation(name, entityInformation);
	return ot::EntityAPI::readEntityFromEntityIDandVersion(entityInformation.getEntityID(), entityInformation.getEntityVersion(), *classFactory);
}



void MicroServiceSolver::RemoveOldResults()
{
	std::string rowDataFolder = solverName + "/" + FolderNames::GetFolderNameRawResultBase();
	std::list<std::string> entityList = ot::ModelServiceAPI::getListOfFolderItems(rowDataFolder);

	ot::ModelServiceAPI::deleteEntitiesFromModel(entityList);
}


SolverSettings MicroServiceSolver::LoadSolverSettings(std::string solverName)
{
	
	if (_units == nullptr)
	{
		_units = dynamic_cast<EntityUnits*>(LoadEntityFromName("Units"));
		assert(_units != nullptr);
	}

	std::unique_ptr< EntitySolverFITTD> solverEntity (dynamic_cast<EntitySolverFITTD*>(LoadEntityFromName(solverName)));
	assert(solverEntity != nullptr);
	this->solverName = solverName;

	auto debug = dynamic_cast<EntityPropertiesBoolean*>( solverEntity->getProperties().getProperty(EntitySolverFITTD::GetPropertyNameDebug()));
	assert(debug != nullptr);
	auto dimension = dynamic_cast<EntityPropertiesSelection*>( solverEntity->getProperties().getProperty(EntitySolverFITTD::GetPropertyNameDimension()));
	assert(dimension != nullptr);
	auto mesh = dynamic_cast<EntityPropertiesEntityList*>(solverEntity->getProperties().getProperty(EntitySolverFITTD::GetPropertyNameMesh()));
	assert(mesh != nullptr);
	if (mesh->getValueName() == "")
	{
		throw std::invalid_argument("No mesh selected.");
	}
	auto timeSteps = dynamic_cast<EntityPropertiesInteger*>(  solverEntity->getProperties().getProperty(EntitySolverFITTD::GetPropertyNameTimesteps()));
	assert(timeSteps != nullptr);
	auto volumeSelection = dynamic_cast<EntityPropertiesSelection*>(  solverEntity->getProperties().getProperty(EntitySolverFITTD::GetPropertyNameVolumeSelection()));
	assert(volumeSelection != nullptr);

	return SolverSettings(debug->getValue(), dimension->getValue(), mesh->getValueName(), timeSteps->getValue(), volumeSelection->getValue());
}

EntityMeshCartesianData * MicroServiceSolver::LoadMeshData(std::string meshName)
{
	auto meshData = dynamic_cast<EntityMeshCartesianData*>(LoadEntityFromName(meshName + "/Mesh"));
	
	if (meshData == nullptr)
	{
		throw std::invalid_argument("The specified mesh is not a valid cartesian mesh : " + meshName + "\n");
	}
	setMeshDataEntityID(meshData->getEntityID());
	setMeshDataEntityVersionID(meshData->getEntityStorageVersion());
	return meshData;
}

Signal MicroServiceSolver::LoadSignal(std::string signalName)
{
	std::unique_ptr<EntitySignalType> signalEntity(dynamic_cast<EntitySignalType*>(LoadEntityFromName(signalName)));
	assert(signalEntity != nullptr);

	std::string propertyName = EntitySignalType::GetPropertyNameExcitationType();
	EntityPropertiesSelection* excitationType = dynamic_cast<EntityPropertiesSelection*>(signalEntity->getProperties().getProperty(propertyName));

	propertyName = EntitySignalType::GetPropertyNameMinFrequency();
	EntityPropertiesDouble* minFrequency = dynamic_cast<EntityPropertiesDouble*>(signalEntity->getProperties().getProperty(propertyName));

	propertyName = EntitySignalType::GetPropertyNameMaxFrequency();
	EntityPropertiesDouble * maxFrequency = dynamic_cast<EntityPropertiesDouble*>(signalEntity->getProperties().getProperty(propertyName));

	double enteredMaxFrequ = maxFrequency->getValue();
	double enteredMinFrequ = minFrequency->getValue();
	double internalMinFrequ = enteredMinFrequ;
	double internalMaxFrequ = enteredMaxFrequ;

	std::string formerUnitMin, formerUnitMax;
	ApplyFrequencyUnit(internalMinFrequ, formerUnitMin);
	ApplyFrequencyUnit(internalMaxFrequ, formerUnitMax);

	if (internalMaxFrequ == internalMinFrequ)
	{
		throw std::logic_error("Signal " + signalName + " uses a 0 bandwidth.");
	}

	Signal newSignal(signalName, excitationType->getValue(), PhysicalQuantities(enteredMinFrequ, internalMinFrequ, formerUnitMin), PhysicalQuantities(enteredMaxFrequ, internalMaxFrequ, formerUnitMax));

	return newSignal;
}

PortSettings MicroServiceSolver::LoadPortSetting(std::string portName, std::string solverName)
{
	auto portEntityBase = LoadEntityFromName(portName);
	std::unique_ptr<EntitySolverPort> portEntity(dynamic_cast<EntitySolverPort*>(portEntityBase));
	assert(portEntity != nullptr);

	//Get basic port properties
	std::string propertyName = EntitySolverPort::GetPropertyNameSignal();
	auto signal = dynamic_cast<EntityPropertiesEntityList*>(portEntity->getProperties().getProperty(propertyName));

	propertyName = EntitySolverPort::GetPropertyNameExcitationAxis();
	auto excitationAxis = dynamic_cast<EntityPropertiesSelection*>(portEntity->getProperties().getProperty(propertyName));

	propertyName = EntitySolverPort::GetPropertyNameExcitationTarget();
	auto excitationTarget = dynamic_cast<EntityPropertiesSelection*>(portEntity->getProperties().getProperty(propertyName));

	PortSettings newPort(portEntity->getName(), excitationTarget->getValue(), excitationAxis->getValue(), signal->getValueName());

	//Determine port location
	propertyName = EntitySolverPort::GetPropertyNameSelectionMethod();
	EntityPropertiesSelection * pointSelection = dynamic_cast<EntityPropertiesSelection *>(portEntity->getProperties().getProperty(propertyName));
	if (pointSelection->getValue() == EntitySolverPort::GetValueSelectionMethodFree())
	{
		portName = EntitySolverPort::GetPropertyNameXCoordinate();
		auto xCoordinate = dynamic_cast<EntityPropertiesInteger*>(portEntity->getProperties().getProperty(propertyName));
		portName = EntitySolverPort::GetPropertyNameYCoordinate();
		auto yCoordinate = dynamic_cast<EntityPropertiesInteger*>(portEntity->getProperties().getProperty(propertyName));
		portName = EntitySolverPort::GetPropertyNameZCoordinate();
		auto zCoordinate = dynamic_cast<EntityPropertiesInteger*>(portEntity->getProperties().getProperty(propertyName));

		newPort.SetExcitationPoint(xCoordinate->getValue(), yCoordinate->getValue(), zCoordinate->getValue());
	}
	return newPort;
}

MonitorSettings MicroServiceSolver::LoadMonitorSetting(std::string monitorName)
{
	auto monitorEntityBase = LoadEntityFromName(monitorName);
	auto monitorEntity = dynamic_cast<EntitySolverMonitor*>(monitorEntityBase);
	assert(monitorEntity != nullptr);

	std::string propertyName = EntitySolverMonitor::GetPropertyNameDoF();
	auto selectedDegreesOfFreedom = dynamic_cast<EntityPropertiesSelection*>(monitorEntity->getProperties().getProperty(propertyName));

	propertyName = EntitySolverMonitor::GetPropertyNameDomain();
	auto selectedDomain = dynamic_cast<EntityPropertiesSelection*>(monitorEntity->getProperties().getProperty(propertyName));

	propertyName = EntitySolverMonitor::GetPropertyNameFieldComp();
	auto selectedFieldComp = dynamic_cast<EntityPropertiesSelection*>(monitorEntity->getProperties().getProperty(propertyName));

	propertyName = EntitySolverMonitor::GetPropertyNameObservFreq();
	auto observationFrequency = dynamic_cast<EntityPropertiesDouble*>(monitorEntity->getProperties().getProperty(propertyName));

	propertyName = EntitySolverMonitor::GetPropertyNameQuantity();
	auto monitorQuantity = dynamic_cast<EntityPropertiesSelection*>(monitorEntity->getProperties().getProperty(propertyName));

	propertyName = EntitySolverMonitor::GetPropertyNameMonitorFrequency();
	auto samplingRate = dynamic_cast<EntityPropertiesInteger*>(monitorEntity->getProperties().getProperty(propertyName));

	propertyName = EntitySolverMonitor::GetPropertyNameVolume();
	auto volume = dynamic_cast<EntityPropertiesSelection*>(monitorEntity->getProperties().getProperty(propertyName));

	propertyName = EntitySolverMonitor::GetPropertyNameObservedTimeStep();
	auto timeStep = dynamic_cast<EntityPropertiesInteger *>(monitorEntity->getProperties().getProperty(propertyName));

	double obsFrequency = observationFrequency->getValue();
	double enteredFrequency = obsFrequency;
	std::string formerUnit;
	ApplyFrequencyUnit(obsFrequency, formerUnit);

	PhysicalQuantities observedFrequency(enteredFrequency, obsFrequency, formerUnit);
	return MonitorSettings(monitorEntity->getName(), selectedDomain->getValue(), selectedDegreesOfFreedom->getValue(), selectedFieldComp->getValue(), monitorQuantity->getValue(), volume->getValue(),samplingRate->getValue(), observedFrequency, timeStep->getValue());
}

void MicroServiceSolver::ApplyFrequencyUnit(double & value, std::string & formerUnit)
{
	_units->TurnToSIFrequency(value, formerUnit);
}

void MicroServiceSolver::UpdateModel(void)
{
	ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisibleList, dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added FIT-TD results");
}


void MicroServiceSolver::addResultFD(std::string name, std::string title, double * xComponent, double * yComponent, double * zComponent, size_t size)
{
	try
	{
		//Store vector components and meshData reference in data entity
		ot::UID xUID = modelComponent->createEntityUID();
		ot::UID yUID = modelComponent->createEntityUID();
		ot::UID zUID = modelComponent->createEntityUID();

		//EntityResult3DData memory managment is taken over by container parent EntityResult3D
		auto dataEntity = new EntityResult3DData(modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, "Model");
		std::unique_ptr< EntityResult3D> topoEntity(new EntityResult3D(modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, "VisualizationService"));
		std::string rawDataFolder = solverName + "/" + FolderNames::GetFolderNameRawResultBase() + "/";
		dataEntity->setName(rawDataFolder + "3DResultData");
		dataEntity->setParent(topoEntity.get());
		
		std::unique_ptr<EntityCompressedVector> xComponentDataEnt (new EntityCompressedVector(xUID, dataEntity, nullptr, nullptr, nullptr, "Model"));
		xComponentDataEnt->setName("DFTVectorResultXComponent");
		xComponentDataEnt->setValues(xComponent, size);
		xComponentDataEnt->StoreToDataBase();
		ot::UID xVersion = xComponentDataEnt->getEntityStorageVersion();

		std::unique_ptr<EntityCompressedVector> yComponentDataEnt (new EntityCompressedVector(yUID, dataEntity, nullptr, nullptr, nullptr, "Model"));
		yComponentDataEnt->setName("DFTVectorResultYComponent");
		yComponentDataEnt->setValues(yComponent, size);
		yComponentDataEnt->StoreToDataBase();
		ot::UID yVersion = yComponentDataEnt->getEntityStorageVersion();

		std::unique_ptr<EntityCompressedVector> zComponentDataEnt (new EntityCompressedVector(zUID, dataEntity, nullptr, nullptr, nullptr, "Model"));
		zComponentDataEnt->setName("DFTVectorResultZComponent");
		zComponentDataEnt->setValues(zComponent, size);
		zComponentDataEnt->StoreToDataBase();
		ot::UID zVersion = zComponentDataEnt->getEntityStorageVersion();

		dataEntity->SetVectorDataUIDs(xComponentDataEnt->getEntityID(), xComponentDataEnt->getEntityStorageVersion(), 
									  yComponentDataEnt->getEntityID(), yComponentDataEnt->getEntityStorageVersion(),
									  zComponentDataEnt->getEntityID(), zComponentDataEnt->getEntityStorageVersion());
		dataEntity->setMeshData(meshDataEntityID, meshDataVersion);
		dataEntity->setEditable(true);
		dataEntity->setResultType(EntityResultBase::CARTESIAN_NODE_VECTORS);
		dataEntity->StoreToDataBase();

		topoEntity->setName(name + "/Plot");
		topoEntity->setEditable(true);
		topoEntity->setInitiallyHidden(true);
		topoEntity->setResultType(EntityResultBase::CARTESIAN_NODE_VECTORS);
		topoEntity->createProperties();
		topoEntity->setSource(dataEntity->getEntityID(), dataEntity->getEntityStorageVersion());
		topoEntity->StoreToDataBase();

		topologyEntityIDList.push_back(topoEntity->getEntityID());
		topologyEntityVersionList.push_back(topoEntity->getEntityStorageVersion());
		topologyEntityForceVisibleList.push_back(false);

		dataEntityIDList.insert(dataEntityIDList.end(), { xUID, yUID, zUID, dataEntity->getEntityID() });
		dataEntityVersionList.insert(dataEntityVersionList.end(), { xVersion, yVersion, zVersion, dataEntity->getEntityStorageVersion() });
		dataEntityParentList.insert(dataEntityParentList.end(), { dataEntity->getEntityID(), dataEntity->getEntityID() ,dataEntity->getEntityID(), topoEntity->getEntityID() });

		ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisibleList, dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added new DFT vector result");
	}
	catch (std::exception & e)
	{
		displayMessage("Exception while storing DFT vector result data: " + std::string(e.what()));
	}
}

void MicroServiceSolver::StoreLogDataInResultItem(std::string logData)
{
	// Store the logData in a result item
	EntityResultText *output = modelComponent->addResultTextEntity(solverName + "/Output", logData);

	topologyEntityIDList.push_back(output->getEntityID()); 
	topologyEntityVersionList.push_back(output->getEntityStorageVersion());
	topologyEntityForceVisibleList.push_back(false);

	dataEntityIDList.push_back(output->getTextDataStorageId());
	dataEntityVersionList.push_back(output->getTextDataStorageVersion());
	dataEntityParentList.push_back(output->getEntityID());
}
