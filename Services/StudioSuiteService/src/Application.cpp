/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

// Service header
#include "Application.h"
#include "ModelNotifier.h"
#include "UiNotifier.h"

// Open twin header
#include "OTCommunication/actionTypes.h"		// action member and types definition
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

// Application specific includes
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "ClassFactory.h"
#include "EntityResultText.h"
#include "EntityUnits.h"
#include "EntityMaterial.h"
#include "EntityProperties.h"
#include "EntityGeometry.h"

#include "boost/algorithm/string.hpp"

#include <thread>
#include <map>
#include <sstream>

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_STUDIOSUITE

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_STUDIOSUITE

Application::Application()
	: ot::ApplicationBase(MY_SERVICE_NAME, MY_SERVICE_TYPE, new UiNotifier(), new ModelNotifier())
{

}

Application::~Application()
{

}

// ##################################################################################################################################

// Required functions

void Application::run(void)
{
	// This method is called once the service can start its operation
	if (EnsureDataBaseConnection())
	{
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
	}
	// Add code that should be executed when the service is started and may start its work
}

std::string Application::processAction(const std::string & _action, ot::JsonDocument& _doc)
{
	if (_action == OT_ACTION_CMD_UI_SS_UPLOAD_NEEDED)
	{
		uploadNeeded(_doc);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_DOWNLOAD_NEEDED)
	{
		downloadNeeded(_doc);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_FILES_UPLOADED)
	{
		filesUploaded(_doc);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_UNITS)
	{
		std::string content = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Content);
		changeUnits(content);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_MATERIALS)
	{
		std::string content = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Content);
		changeMaterials(content);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_SHAPEINFO)
	{
		std::string content = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Content);
		shapeInformation(content);
		return "";
	}
	else if (_action == OT_ACTION_CMD_UI_SS_TRIANGLES)
	{
		std::list<std::string> names = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_Name);
		std::list<std::string> triangles = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_Content);
		shapeTriangles(names, triangles);
		return "";
	}

	return OT_ACTION_RETURN_UnknownAction;
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument& _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	//_ui->registerForModelEvents();
	_ui->addMenuPage("Project");
	
	_ui->addMenuGroup("Project", "Import");
	_ui->addMenuGroup("Project", "Versions");

	ot::Flags<ot::ui::lockType> modelWrite;
	modelWrite.setFlag(ot::ui::lockType::tlModelWrite);

	_ui->addMenuButton("Project", "Import", "CST File", "CST File", modelWrite, "Import", "Default");
	_ui->addMenuButton("Project", "Versions", "Commit", "Commit", modelWrite, "AddSolver", "Default");
	_ui->addMenuButton("Project", "Versions", "Get", "Get", modelWrite, "ArrowGreenDown", "Default");

	modelSelectionChangedNotification();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{

}

void Application::modelDisconnected(const ot::components::ModelComponent * _model)
{

}

void Application::serviceConnected(ot::ServiceBase * _service)
{

}

void Application::serviceDisconnected(const ot::ServiceBase * _service)
{

}

void Application::preShutdown(void) {

}

void Application::shuttingDown(void)
{

}

bool Application::startAsRelayService(void) const
{
	return false;	// Do not want the service to start a relay service. Otherwise change to true
}

// ##################################################################################################################################

std::string Application::handleExecuteModelAction(ot::JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	if (     action == "Project:Import:CST File")			  importProject();
	else if (action == "Project:Versions:Commit")			  commitChanges();
	else if (action == "Project:Versions:Get")			      getChanges();
	//else if (action == "Model:Sources:Add Terminal")	      addTerminal();
	//else if (action == "ElmerFEM:Sources:Define Electrostatic Potential")  definePotential();
	else assert(0); // Unhandled button action
	return std::string();
}

std::string Application::handleModelSelectionChanged(ot::JsonDocument& _document) {
	selectedEntities = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_SelectedEntityIDs);
	modelSelectionChangedNotification();
	return std::string();
}

void Application::modelSelectionChangedNotification(void)
{
	//if (isUiConnected()) {
	//	std::list<std::string> enabled;
	//	std::list<std::string> disabled;

	//	if (selectedEntities.size() > 0)
	//	{
	//		enabled.push_back("ElmerFEM:Solver:Run Solver");
	//	}
	//	else
	//	{
	//		disabled.push_back("ElmerFEM:Solver:Run Solver");
	//	}

	//	m_uiComponent->setControlsEnabledState(enabled, disabled);
	//}
}

void Application::EnsureVisualizationModelIDKnown(void)
{
	if (visualizationModelID > 0) return;
	if (m_modelComponent == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	// The visualization model isnot known yet -> get it from the model
	visualizationModelID = m_modelComponent->getCurrentVisualizationModelID();
}


void Application::importProject(void)
{
	modelComponent()->clearNewEntityList();

	// TODO: Check whether the project has already been initialized
	 
	
	// Send the import message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_IMPORT, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());

	uiComponent()->sendMessage(true, doc);
}

void Application::commitChanges(void)
{
	modelComponent()->clearNewEntityList();

	// TODO: Check whether the project has already been initialized


	// Send the commit message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_COMMIT, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());

	uiComponent()->sendMessage(true, doc);
}

void Application::getChanges(void)
{
	modelComponent()->clearNewEntityList();

	// TODO: Check whether the project has already been initialized


	// Get the current model version
	std::string version = modelComponent()->getCurrentModelVersion();

	// Send the commit message to the UI
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_GET, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(version, doc.GetAllocator()), doc.GetAllocator());

	uiComponent()->sendMessage(true, doc);
}

void Application::uploadNeeded(ot::JsonDocument& _doc)
{
	size_t count = ot::json::getInt64(_doc, OT_ACTION_PARAM_COUNT);

	ot::UIDList entityID, versionID;

	for (size_t i = 0; i < count; i++)
	{
		entityID.push_back(m_modelComponent->createEntityUID());
		versionID.push_back(m_modelComponent->createEntityUID());
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_UPLOAD, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(versionID, doc.GetAllocator()), doc.GetAllocator());
	
	uiComponent()->sendMessage(true, doc);
}

void Application::downloadNeeded(ot::JsonDocument& _doc)
{
	// Determine all files in the Files folder
	std::list<std::string> fileNames = modelComponent()->getListOfFolderItems("Files", true);

	std::list<ot::EntityInformation> fileInfo;
	modelComponent()->getEntityInformation(fileNames, fileInfo);

	ot::UIDList entityID, versionID;

	for (auto file : fileInfo)
	{
		entityID.push_back(file.getID());
		versionID.push_back(file.getVersion());
	}

	// Get the current model version
	std::string version = modelComponent()->getCurrentModelVersion();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_DOWNLOAD, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(entityID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersionList, ot::JsonArray(versionID, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(version, doc.GetAllocator()), doc.GetAllocator());

	uiComponent()->sendMessage(true, doc);
}

void Application::filesUploaded(ot::JsonDocument& _doc)
{
	std::string changeMessage = ot::json::getString(_doc, OT_ACTION_PARAM_MESSAGE);

	std::list<std::string> modifiedNameList = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_Name);
	std::list<ot::UID> fileEntityIDList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityID);
	std::list<ot::UID> fileVersionList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityVersion);
	std::list<ot::UID> dataEntityIDList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_DataID);
	std::list<ot::UID> dataVersionList = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_DataVersion);

	std::list<std::string> deletedNameList = ot::json::getStringList(_doc, OT_ACTION_CMD_MODEL_DeleteEntity);

	// Now we need to send the model change to the model service 

	for (auto item : modifiedNameList)
	{
		ot::UID fileEntityID = fileEntityIDList.front(); fileEntityIDList.pop_front();
		ot::UID fileVersion  = fileVersionList.front();  fileVersionList.pop_front();
		ot::UID dataEntityID = dataEntityIDList.front(); dataEntityIDList.pop_front();
		ot::UID dataVersion  = dataVersionList.front();  dataVersionList.pop_front();

		modelComponent()->addNewTopologyEntity(fileEntityID, fileVersion, false);
		modelComponent()->addNewDataEntity(dataEntityID, dataVersion, fileEntityID);
	}

	modelComponent()->deleteEntitiesFromModel(deletedNameList, false);

	modelComponent()->storeNewEntities(changeMessage, false);

	// Determine the new version
	std::string newVersion = modelComponent()->getCurrentModelVersion();

	// Finally we send the new version to the frontend 
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_COPY, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(newVersion, doc.GetAllocator()), doc.GetAllocator());

	uiComponent()->sendMessage(true, doc);
}

void Application::changeUnits(const std::string &content)
{
	// Load the current units entity
	ot::EntityInformation entityInformation;
	modelComponent()->getEntityInformation("Units", entityInformation);
	EntityUnits* units = dynamic_cast<EntityUnits*> (modelComponent()->readEntityFromEntityIDandVersion(entityInformation.getID(), entityInformation.getVersion(), getClassFactory()));
	assert(units != nullptr);
	if (units == nullptr) return;

	// Loop through the various units and check whether an update is necessary
	std::stringstream buffer(content);
	bool changed = false;

	processSingleUnit("Dimension", buffer, units, changed);
	processSingleUnit("Temperature", buffer, units, changed);
	processSingleUnit("Voltage", buffer, units, changed);
	processSingleUnit("Current", buffer, units, changed);
	processSingleUnit("Resistance", buffer, units, changed);
	processSingleUnit("Conductance", buffer, units, changed);
	processSingleUnit("Capacitance", buffer, units, changed);
	processSingleUnit("Inductance", buffer, units, changed);
	processSingleUnit("Frequency", buffer, units, changed);
	processSingleUnit("Time", buffer, units, changed);

	// If a change is necessary, store the new entity
	if (changed)
	{
		units->StoreToDataBase();
		modelComponent()->addNewTopologyEntity(units->getEntityID(), units->getEntityStorageVersion(), false);
	}

	delete units; units = nullptr;
}

void Application::processSingleUnit(const std::string& unitName, std::stringstream& buffer, EntityUnits* units, bool& changed)
{
	std::string currentValue;
	std::getline(buffer, currentValue);

	EntityPropertiesSelection* scale = dynamic_cast<EntityPropertiesSelection*>(units->getProperties().getProperty(unitName));
	assert(scale != nullptr);
	if (scale == nullptr) return;

	if (scale->getValue() != currentValue)
	{
		changed = true;
		scale->setValue(currentValue);
	}
}

void Application::changeMaterials(const std::string &content)
{
	std::stringstream buffer(content);

	ot::UIDList currentMaterials = modelComponent()->getIDsOfFolderItemsOfType("Materials", "EntityMaterial", true);
	std::list<ot::EntityInformation> currentMaterialInfo;
	modelComponent()->getEntityInformation(currentMaterials, currentMaterialInfo);
	std::map<std::string, bool> materialProcessed;

	while (processSingleMaterial(buffer, materialProcessed));

	// Now we need to check if an existing material was not processed, which means that it no longer exists.
	// In this case, we need to delete the material

	std::list<std::string> obsoleteMaterials;

	for (auto material : currentMaterialInfo)
	{
		if (materialProcessed.count(material.getName()) == 0)
		{
			obsoleteMaterials.push_back(material.getName());
		}
	}

	if (!obsoleteMaterials.empty())
	{
		modelComponent()->deleteEntitiesFromModel(obsoleteMaterials, false);
	}
}

bool Application::processSingleMaterial(std::stringstream& buffer, std::map<std::string, bool> &materialProcessed)
{
	std::string materialName, materialColor, materialType, materialEps, materialMu, materialSigma;
	double materialEpsValue(1.0), materialMuValue(1.0), materialSigmaValue(0.0);

	std::getline(buffer, materialName);
	if (materialName.empty()) return false;

	std::getline(buffer, materialColor);
	double r(0.0), g(0.0), b(0.0);
	readDoubleTriple(materialColor, r, g, b);

	materialColors[materialName] = std::tuple<double, double, double>(r, g, b);

	std::getline(buffer, materialType);

	if (materialType == "Normal")
	{
		std::getline(buffer, materialEps);
		double epsx(0.0), epsy(0.0), epsz(0.0);
		readDoubleTriple(materialEps, epsx, epsy, epsz);
		materialEpsValue = sqrt((epsx * epsx + epsy * epsy + epsz * epsz)/3.0);

		std::getline(buffer, materialMu);
		double mux(0.0), muy(0.0), muz(0.0);
		readDoubleTriple(materialMu, mux, muy, muz);
		materialMuValue = sqrt((mux * mux + muy * muy + muz * muz) / 3.0);

		std::getline(buffer, materialSigma);
		double sigmax(0.0), sigmay(0.0), sigmaz(0.0);
		readDoubleTriple(materialSigma, sigmax, sigmay, sigmaz);
		materialSigmaValue = sqrt((sigmax * sigmax + sigmay * sigmay + sigmaz * sigmaz) / 3.0);
	}

	if (materialName[0] == '$')
	{
		// This is an internal material which we should not consider
		return true;
	}

	// Try loading the material
	EntityMaterial* material = nullptr;

	ot::EntityInformation entityInformation;
	if (modelComponent()->getEntityInformation("Materials/" + materialName, entityInformation))
	{
		material = dynamic_cast<EntityMaterial*> (modelComponent()->readEntityFromEntityIDandVersion(entityInformation.getID(), entityInformation.getVersion(), getClassFactory()));
	}

	bool changed = false;

	if (material == nullptr)
	{
		// We have a new material to create
		material = new EntityMaterial(modelComponent()->createEntityUID(), nullptr, nullptr, nullptr, nullptr, serviceName());
		material->setName("Materials/" + materialName);
		material->createProperties();

		EntityPropertiesSelection* type = dynamic_cast<EntityPropertiesSelection*>(material->getProperties().getProperty("Material type"));
		EntityPropertiesDouble* permittivity = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Permittivity (relative)"));
		EntityPropertiesDouble* permeability = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Permeability (relative)"));
		EntityPropertiesDouble* conductivity = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Conductivity"));

		type->setValue(materialType == "Normal" ? "Volumetric" : "PEC");
		type->setNeedsUpdate();

		permittivity->setValue(materialEpsValue);
		permeability->setValue(materialMuValue);
		conductivity->setValue(materialSigmaValue);

		changed = true;
	}
	else
	{
		// We need to modify an existing material
		EntityPropertiesSelection* type = dynamic_cast<EntityPropertiesSelection*>(material->getProperties().getProperty("Material type"));
		EntityPropertiesDouble* permittivity = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Permittivity (relative)"));
		EntityPropertiesDouble* permeability = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Permeability (relative)"));
		EntityPropertiesDouble* conductivity = dynamic_cast<EntityPropertiesDouble*>(material->getProperties().getProperty("Conductivity"));

		type->setValue(materialType == "Normal" ? "Volumetric" : "PEC");

		permittivity->setValue(materialEpsValue);
		permeability->setValue(materialMuValue);
		conductivity->setValue(materialSigmaValue);

		if (material->getProperties().anyPropertyNeedsUpdate())
		{
			changed = true;
		}

		materialProcessed[material->getName()] = true;
	}

	// If a change is necessary, store the new entity
	if (changed)
	{
		material->updateFromProperties();
		material->StoreToDataBase();
		modelComponent()->addNewTopologyEntity(material->getEntityID(), material->getEntityStorageVersion(), false);
	}

	delete material; material = nullptr;

	return true;
}

void Application::readDoubleTriple(const std::string& line, double& a, double& b, double& c)
{
	std::stringstream buffer(line);

	std::string sA, sB, sC;
	buffer >> sA;
	buffer >> sB;
	buffer >> sC;

	std::replace(sA.begin(), sA.end(), ',', '.');
	std::replace(sB.begin(), sB.end(), ',', '.');
	std::replace(sC.begin(), sC.end(), ',', '.');

	a = std::atof(sA.c_str());
	b = std::atof(sB.c_str());
	c = std::atof(sC.c_str());
}

void Application::shapeInformation(const std::string &content)
{
	std::stringstream data(content);

	while (!data.eof())
	{
		std::string name, material;

		data >> name;
		data >> material;

		if (!name.empty())
		{
			shapeMaterials[name] = material;
		}
	}
}

void Application::shapeTriangles(std::list<std::string>& shapeNames, std::list<std::string>& shapeTriangles)
{
	std::string materialsFolder = "Materials";

	std::list<std::string> entityList{ materialsFolder };
	std::list<ot::EntityInformation> entityInfo;

	modelComponent()->getEntityInformation(entityList, entityInfo);

	assert(entityInfo.size() == 1);
	assert(entityInfo.front().getName() == materialsFolder);

	ot::UID materialsFolderID = entityInfo.front().getID();

	auto triangles = shapeTriangles.begin();

	for (auto name : shapeNames)
	{
		storeShape(name, *triangles, materialsFolder, materialsFolderID);
		triangles++;
	}
}

void Application::storeShape(const std::string& name, const std::string& triangles, 
							 const std::string &materialsFolder, ot::UID materialsFolderID)
{
	std::string otName = "Geometry\\" + name;
	std::replace(otName.begin(), otName.end(), '\\', '/');

	std::string material = shapeMaterials[name];
	std::tuple<double, double, double> rgb = materialColors[material];

	int colorR = (int) (255 * std::get<0>(rgb));
	int colorG = (int) (255 * std::get<1>(rgb));
	int colorB = (int) (255 * std::get<2>(rgb));

	ot::UID entityID = modelComponent()->createEntityUID();
	ot::UID facetsID = modelComponent()->createEntityUID();

	EntityGeometry* entityGeom = new EntityGeometry(entityID, nullptr, nullptr, nullptr, nullptr, serviceName());
	entityGeom->setName(otName);
	entityGeom->setEditable(false);

	entityGeom->createProperties(colorR, colorG, colorB, materialsFolder, materialsFolderID);

	EntityPropertiesEntityList* materialProp = dynamic_cast<EntityPropertiesEntityList*> (entityGeom->getProperties().getProperty("Material"));
	assert(materialProp != nullptr);
	if (materialProp != nullptr)
	{
		materialProp->setValueName(material);
	}

	entityGeom->getFacets()->setEntityID(facetsID);

	createFacets(triangles, entityGeom->getFacets()->getNodeVector(), entityGeom->getFacets()->getTriangleList(), entityGeom->getFacets()->getEdgeList());

	entityGeom->getFacets()->StoreToDataBase();
	entityGeom->StoreToDataBase();

	modelComponent()->addNewDataEntity(entityGeom->getFacets()->getEntityID(), entityGeom->getFacets()->getEntityStorageVersion(), entityGeom->getEntityID());
	modelComponent()->addNewDataEntity(entityGeom->getBrepEntity()->getEntityID(), entityGeom->getBrepEntity()->getEntityStorageVersion(), entityGeom->getEntityID());
	modelComponent()->addNewTopologyEntity(entityGeom->getEntityID(), entityGeom->getEntityStorageVersion(), false);

	delete entityGeom;
	entityGeom = nullptr;
}

void Application::createFacets(const std::string& data, std::vector<Geometry::Node>& nodes, std::list<Geometry::Triangle>& triangles, std::list<Geometry::Edge>& edges)
{
	// Process the STL data
	std::stringstream stlData(data);

	ot::UID vertex[3];
	Geometry::Node node[3];
	int vertexID = 0;
	size_t vertexCount = 0;

	std::list<Geometry::Node> nodesList;

	while (!stlData.eof())
	{
		std::string line;
		std::getline(stlData, line);

		boost::trim_left(line);
		boost::to_lower(line);

		std::stringstream lineBuffer(line);

		std::string keyword;
		lineBuffer >> keyword;

		if (keyword == "vertex")
		{
			double pX(0.0), pY(0.0), pZ(0.0);

			lineBuffer >> pX >> pY >> pZ;

			node[vertexID].setCoords(pX, pY, pZ);

			vertex[vertexID] = vertexCount;
			vertexCount++;
			vertexID++;

			if (vertexID == 3)
			{
				// We need to calculate the facet normal, since this information can be incorrect in Studio Suite generated STL files

				double vector1[] = { node[1].getCoord(0) - node[0].getCoord(0), node[1].getCoord(1) - node[0].getCoord(1), node[1].getCoord(2) - node[0].getCoord(2) };
				double vector2[] = { node[2].getCoord(0) - node[0].getCoord(0), node[2].getCoord(1) - node[0].getCoord(1), node[2].getCoord(2) - node[0].getCoord(2) };

				double normalX = vector1[1] * vector2[2] - vector1[2] * vector2[1];
				double normalY = vector1[2] * vector2[0] - vector1[0] * vector2[2];
				double normalZ = vector1[0] * vector2[1] - vector1[1] * vector2[0];

				double abs = sqrt(normalX * normalX + normalY * normalY + normalZ * normalZ);

				normalX /= abs;
				normalY /= abs;
				normalZ /= abs;

				node[0].setNormals(normalX, normalY, normalZ);
				node[1].setNormals(normalX, normalY, normalZ);
				node[2].setNormals(normalX, normalY, normalZ);

				nodesList.push_back(node[0]);
				nodesList.push_back(node[1]);
				nodesList.push_back(node[2]);

				// Three vertices were added, so we have a new facet
				triangles.push_back(Geometry::Triangle(vertex[0], vertex[1], vertex[2], 0));
				vertexID = 0;
			}
		}
	}

	// Finally we need to store the list of nodes in the nodes array
	nodes.reserve(nodesList.size());
	for (auto n : nodesList)
	{
		nodes.push_back(n);
	}
}

