// @otlicense

// Service header
#include "Application.h"
#include "Handler/ProjectComparisonManager.h"

// OpenTwin header
#include "OTServiceFoundation/ProgressUpdater.h"
#include "OTModelEntities/ModelState.h"
#include "OTModelEntities/EntityBase.h"
#include "OTModelEntities/ProjectCompare/EntityProjectCompareNewEntity.h"
#include "OTModelEntities/ProjectCompare/EntityProjectCompareMissingEntitiy.h"

ProjectComparisonManager::ProjectComparisonManager(ot::ParallelCollectionRAII&& _collectionSwitch, ModelState* _baseState, ModelState* _compareState)
	: m_collectionSwitch(std::move(_collectionSwitch)),
	m_baseState(_baseState), m_compareState(_compareState)
{

}

ProjectComparisonManager::~ProjectComparisonManager()
{

}

bool ProjectComparisonManager::exec(ot::ProjectCompareConfig&& _config)
{
	try
	{
		ComparisonData data;
		initializeComparisonData(data, _config);
		openOtherProject(data);
		compareEntityExistance(data);
		storeResults(data);

		return true;
	}
	catch (const std::exception& e)
	{
		OT_LOG_E(e.what());
		return false;
	}
	catch (...)
	{
		OT_LOG_E("[FATAL] Unknown exception during project compare");
		return false;
	}
}

void ProjectComparisonManager::initializeComparisonData(ComparisonData& _data, const ot::ProjectCompareConfig& _config)
{
	Application* app = Application::instance();
	Model* model = app->getModel();
	if (!model)
	{
		throw ot::Exception::General("No model created yet");
	}

	m_collectionSwitch.switchToInitial();

	_data.rootEntity.createProperties();
	_data.rootEntity.setPropertiesFromConfig(_config);
	_data.rootEntity.setBaseProject(app->getProjectName());
	_data.rootEntity.setBaseVersion(m_baseState->getModelStateVersion());

	std::list<std::string> folderContent = model->getListOfFolderItems(ot::FolderNames::ProjectCompareFolder, false);
	const std::string rootName = ot::EntityName::createUniqueEntityName(ot::FolderNames::ProjectCompareFolder, folderContent, "Comparison");
	_data.rootEntity.setName(rootName);
	_data.rootEntity.setEntityID(model->createEntityUID());
}

void ProjectComparisonManager::openOtherProject(ComparisonData& _data)
{
	updateProgress(ComparisonStep::StepOpenOtherProject);

	m_collectionSwitch.switchToOther();

	if (!m_compareState->loadModelState(_data.rootEntity.getCompareVersion()))
	{
		throw ot::Exception::General("Failed to load model state for project: " + _data.rootEntity.getCompareProject() + " with version: " + _data.rootEntity.getCompareVersion());
	}
}

void ProjectComparisonManager::compareEntityExistance(ComparisonData& _data)
{
	updateProgress(ComparisonStep::StepCompare);
	
	// Read base entities

	m_collectionSwitch.switchToInitial();

	std::map<std::string, EntityBase*> baseEntitiesBuffer;
	std::list<std::pair<ot::UID, ModelStateEntity>> baseEntityInfos;
	m_baseState->getListOfTopologyEntities(baseEntityInfos);
	
	// Read other entities

	m_collectionSwitch.switchToOther();

	std::map<std::string, EntityBase*> rightEntitiesBuffer;
	std::list<std::pair<ot::UID, ModelStateEntity>> rightEntityInfos;
	m_compareState->getListOfTopologyEntities(rightEntityInfos);
}

void ProjectComparisonManager::storeResults(ComparisonData& _data)
{
	Model* model = Application::instance()->getModel();
	if (!model)
	{
		OT_LOG_E("No model created yet");
		return;
	}

	m_collectionSwitch.switchToInitial();

	// Store root
	_data.rootEntity.storeToDataBase();
	_data.newModelStateInfo.addTopologyEntity(_data.rootEntity);

	// Store other
	for (EntityBase* entity : _data.entitiesToStore)
	{
		entity->storeToDataBase();
		_data.newModelStateInfo.addTopologyEntity(*entity);
	}

	// Save state
	if (_data.newModelStateInfo.hasEntities())
	{
		model->addEntitiesToModel(_data.newModelStateInfo, "Compared projects", true, true, true);
	}
}

void ProjectComparisonManager::initializeUpdater(ProgressUpdater* _updater)
{
	m_progressUpdater = _updater;
	if (m_progressUpdater)
	{
		m_progressUpdater->setTotalNumberOfSteps(static_cast<uint64_t>(ComparisonStep::StepDone));
	}
	updateProgress(m_step);
}

void ProjectComparisonManager::updateProgress(ComparisonStep _step)
{
	m_step = _step;
	if (m_progressUpdater)
	{
		m_progressUpdater->triggerUpdate(static_cast<uint64_t>(m_step));
	}
}