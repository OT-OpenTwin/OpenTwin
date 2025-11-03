// @otlicense
// File: Application.cpp
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
#include "Application.h"
#include "ModelNotifier.h"
#include "UiNotifier.h"

// Open twin header
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionTypes.h"		// action member and types definition
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"

// Application specific includes
#include "EntitySolverKriging.h"
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "Kriging.h"

#include <thread>

#include <map>

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_KRIGING

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_KRIGING

Application::Application()
	: ot::ApplicationBase(MY_SERVICE_NAME, MY_SERVICE_TYPE, new UiNotifier(), new ModelNotifier()), krig("run")
{
}

Application::~Application()
{

}

// ##################################################################################################################################

// Required functions

std::string Application::processAction(const std::string & _action,  ot::JsonDocument& _doc)
{
	/* if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
	{
		std::string action = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);

		if      (action == "Post Processing:Kriging:Load")			loadDataset();
		else if (action == "Post Processing:Kriging:Train")			train();
		else if (action == "Post Processing:Kriging:Predict")		predict();
		else if (action == "Post Processing:Kriging:Save")			saveKrigingModel();
		else if (action == "Post Processing:Kriging:Add Params")	addParameters();
		else if (action == "Post Processing:Data:Import")			importData();

		else {
			assert(0); // Unhandled button action
			getUiComponent()->displayMessage("Handler not Implemented.\n");

		}
	}
	else {
		return OT_ACTION_RETURN_UnknownAction;
	}
	*/
	return "";
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	OT_LOG_D("Called..."); // todo: remove debug code

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	//_ui->registerForModelEvents();

	_ui->addMenuPage("Post Processing");

	_ui->addMenuGroup("Post Processing", "Data");
	_ui->addMenuGroup("Post Processing", "Kriging");


	ot::Flags<ot::LockTypes> modelWrite;
	modelWrite.set(ot::LockType::LockModelWrite);

	_ui->addMenuButton("Post Processing", "Data", "Import", "Import", modelWrite, "Kriging", "Default");
	_ui->addMenuButton("Post Processing", "Data", "Export", "Export", modelWrite, "Kriging", "Default");

	_ui->addMenuButton("Post Processing", "Kriging", "Add Params", "Add Params", modelWrite, "Kriging", "Default");
	_ui->addMenuButton("Post Processing", "Kriging", "Load", "Load Dataset", modelWrite, "Kriging", "Default");
	_ui->addMenuButton("Post Processing", "Kriging", "Train", "Train", modelWrite, "Kriging", "Default");
	_ui->addMenuButton("Post Processing", "Kriging", "Predict", "Predict", modelWrite, "Kriging", "Default");
	_ui->addMenuButton("Post Processing", "Kriging", "Save", "Save", modelWrite, "Kriging", "Default");

	modelSelectionChanged();

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

// ##################################################################################################################################

void Application::modelSelectionChanged(void)
{
	OT_LOG_D("Called..."); // todo: remove debug code

	if (isUiConnected()) {
		std::list<std::string> enabled;
		std::list<std::string> disabled;

		/*
		Configure your ui disabled / enabled state
		if (selectedEntities.size() > 0)
		{
			enabled.push_back("PHREEC:Solver:Run Solver");
		}
		else
		{
			disabled.push_back("PHREEC:Solver:Run Solver");
		}
		*/

		getUiComponent()->setControlsEnabledState(enabled, disabled);
	}
}

KrigingParams* Application::getParams(void) 
{
	OT_LOG_D("Called..."); // todo: remove debug code

	std::list<std::string> selectedSolvers = getListOfSelectedKrigingSolvers();

	if (selectedSolvers.size() != 1) return nullptr; // Only one solver can be selected at a time

	// Now we retrieve information about the solver items
	std::list<ot::EntityInformation> solverInfo;
	getModelComponent()->getEntityInformation(selectedSolvers, solverInfo);

	// Prefetch the solver information
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIdsSolver;

	for (auto info : solverInfo)
	{
		prefetchIdsSolver.push_back(std::pair<unsigned long long, unsigned long long>(info.getID(), info.getVersion()));
	}

	DataBase::instance().prefetchDocumentsFromStorage(prefetchIdsSolver);

	// Now read the solver objects for each solver
	std::map<std::string, EntityBase *> solverMap;
	for (auto info : solverInfo)
	{
		EntityBase *entity = getModelComponent()->readEntityFromEntityIDandVersion(info.getID(), info.getVersion());
		solverMap[info.getName()] = entity;

		if (entity == nullptr)
		{
			getUiComponent()->displayMessage("ERROR: Unable to read solver information.\n");
			return NULL;
		}

		EntityPropertiesDouble *theta = dynamic_cast<EntityPropertiesDouble*>(entity->getProperties().getProperty("Theta"));
		assert(theta != nullptr);
		EntityPropertiesDouble *nugget = dynamic_cast<EntityPropertiesDouble*>(entity->getProperties().getProperty("Nugget"));
		assert(nugget != nullptr);
		EntityPropertiesSelection *corr = dynamic_cast<EntityPropertiesSelection*>(entity->getProperties().getProperty("Corr"));
		assert(corr != nullptr);
		EntityPropertiesSelection *poly = dynamic_cast<EntityPropertiesSelection*>(entity->getProperties().getProperty("Poly"));
		assert(poly != nullptr);

		KrigingParams params;
		params.theta = theta->getValue();
		params.nugget = nugget->getValue();
		params.corr.assign(corr->getValue());
		params.poly.assign(poly->getValue());

		cout << "get params: ---------------------------------------" << endl;
		params.printIt();
		cout << "get params end: -----------------------------------" << endl;

		return new KrigingParams(params);
	}

	getUiComponent()->displayMessage("ERROR: Unable to read solver information.\n");
	return NULL;

	//KrigingParams params;
	//params.corr = "squar_exp";
	//params.poly = "linear";
	//return &params;
}

void Application::loadDataset(void) 
{
	OT_LOG_D("Called..."); // todo: remove debug code

	m_uiComponent->displayMessage("loading dataset ...\n");
	krig.setDataSet(DataSet::prepareDataset());
	krig.getDataSet().printDataSet();
	m_uiComponent->displayMessage("loading dataset complete\n");
}

void Application::train(void) 
{
	OT_LOG_D("Called..."); // todo: remove debug code

	KrigingParams* params = getParams();
	if (params == NULL)
	{
		m_uiComponent->displayMessage("params not selected.\n");
		return;
	}

	//krig.setParameters(params);

	std::array<long, 2> dsSize = krig.getDataSet().getDataSize();
	m_uiComponent->displayMessage("Training : (" + std::to_string(dsSize[0]) + ", " + std::to_string(dsSize[1]) + ")\n");
	m_uiComponent->displayMessage(krig.getParameters().toString());

	krig.train();

	m_uiComponent->displayMessage("Training Complete : " + krig.getModelStr().substr(0, 100) + "\n");
}

void Application::predict(void) 
{
	OT_LOG_D("Called..."); // todo: remove debug code

	std::list<std::vector<double>> data = DataSet::prepareDataset().getxDataPoints();
	const std::vector<double> yrow = *data.begin();
	m_uiComponent->displayMessage("Predicting ...\n(" + std::to_string(data.size()) + ", " + std::to_string(yrow.size()) + ")\n");
	m_uiComponent->displayMessage(DataSet::toString(data) + "\n");

	krig.predict(data);
	//krig.predict();

	m_uiComponent->displayMessage("Predicted value size: " + std::to_string(krig.getDataSet().getyPredictionPoints().size()) + "\n");
	m_uiComponent->displayMessage("Predicted data: \n" + krig.outputs + "\n");
	//m_uiComponent->displayMessage("Predicted dataset: \n" + DataSet::toString(krig->getDataSet().getyPredictionPoints()) + "\n");
}

void Application::saveKrigingModel(void) 
{
	// todo: add save functionallity
	assert(0);
	m_uiComponent->displayMessage("Selected Kriging Model saved.");
}

void Application::addParameters(void)
{
	OT_LOG_D("Called..."); // todo: remove debug code

	if (m_modelComponent == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}

	// First get a list of all folder items of the Solvers folder
	std::list<std::string> solverItems = m_modelComponent->getListOfFolderItems("Solvers");

	// Now get a new entity ID for creating the new item
	ot::UID entityID = m_modelComponent->createEntityUID();

	// Create a unique name for the new solver item
	int count = 1;
	std::string solverName;
	do
	{
		solverName = "Solvers/kriging" + std::to_string(count);
		count++;
	} while (std::find(solverItems.begin(), solverItems.end(), solverName) != solverItems.end());

	// Get information about the available meshes (and the first mesh in particular)
	//std::string meshFolderName, meshName;
	//UID meshFolderID{ 0 }, meshID{ 0 };

	//m_modelComponent->getAvailableMeshes(meshFolderName, meshFolderID, meshName, meshID);

	// Create the new solver item and store it in the data base
	EntitySolverKriging *solverEntity = new EntitySolverKriging(entityID, nullptr, nullptr, nullptr, nullptr, serviceName());
	solverEntity->setName(solverName);
	solverEntity->setEditable(true);

	EntityPropertiesDouble::createProperty("Parameters", "Theta", 0.01, "Kriging", solverEntity->getProperties());
	EntityPropertiesDouble::createProperty("Parameters", "Nugget", 2.220446049250313e-14, "Kriging", solverEntity->getProperties());

	std::list<std::string> corrFunctionType;
	corrFunctionType.push_back("abs_exp");
	corrFunctionType.push_back("square_exp");
	corrFunctionType.push_back("matern52");
	corrFunctionType.push_back("matern32");
	EntityPropertiesSelection::createProperty("Parameters", "Corr", corrFunctionType, "square_exp", "Kriging", solverEntity->getProperties());

	std::list<std::string> regressionFunctionType;
	regressionFunctionType.push_back("constant");
	regressionFunctionType.push_back("linear");
	regressionFunctionType.push_back("quadratic");
	EntityPropertiesSelection::createProperty("Parameters", "Poly", regressionFunctionType, "constant", "Kriging", solverEntity->getProperties());

	solverEntity->storeToDataBase();

	// Register the new solver item in the model

	std::list<ot::UID> topologyEntityIDList = { solverEntity->getEntityID() };
	std::list<ot::UID> topologyEntityVersionList = { solverEntity->getEntityStorageVersion() };
	std::list<bool> topologyEntityForceVisible = { false };
	std::list<ot::UID> dataEntityIDList;
	std::list<ot::UID> dataEntityVersionList;
	std::list<ot::UID> dataEntityParentList;

	m_modelComponent->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible,
										 dataEntityIDList, dataEntityVersionList, dataEntityParentList, "add kriging param");
}

std::list<std::string> Application::getListOfSelectedKrigingSolvers(void)
{
	OT_LOG_D("Called..."); // todo: remove debug code

	std::list<std::string> selectedSolverList;

	if (selectedEntities.empty())
	{
		if (m_uiComponent == nullptr) { assert(0); throw std::exception("UI is not connected"); }
		return selectedSolverList;
	}

	// We first get a list of all selected entities
	std::list<ot::EntityInformation> selectedEntityInfo;
	if (m_modelComponent == nullptr) { assert(0); throw std::exception("Model is not connected"); }
	m_modelComponent->getEntityInformation(selectedEntities, selectedEntityInfo);

	// Here we first need to check which solvers are selected and then run them one by one.
	std::map<std::string, bool> solverItemMap;
	for (auto entity : selectedEntityInfo)
	{
		if (entity.getName().substr(0, 8) == "Solvers/" && entity.getType() == "EntitySolverKriging")
		{
			size_t index = entity.getName().find('/', 8);
			if (index != std::string::npos)
			{
				solverItemMap[entity.getName().substr(0, index - 1)] = true;
			}
			else
			{
				solverItemMap[entity.getName()] = true;
			}
		}
	}

	for (auto solver : solverItemMap)
	{
		selectedSolverList.push_back(solver.first);
	}

	return selectedSolverList;
}

void Application::importData(void)
{
	OT_LOG_D("Called..."); // todo: remove debug code

	std::list<std::string> selectedSolvers = getListOfSelectedKrigingSolvers();

	if (selectedSolvers.size() == 1)
	{
		std::string itemName = selectedSolvers.front() + "/input";

		m_modelComponent->importTableFile(itemName);
	}
}
