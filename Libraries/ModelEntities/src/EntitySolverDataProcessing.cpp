// @otlicense

#include "EntitySolverDataProcessing.h"
#include "PropertyHelper.h"

static EntityFactoryRegistrar<EntitySolverDataProcessing> registrar("EntitySolverDataProcessing");

void EntitySolverDataProcessing::createProperties(const std::string& _graphicsSceneFolder, const ot::UID _graphicsSceneFolderID)
{
	EntityPropertiesEntityList::createProperty("Pipeline", "Pipeline to run", _graphicsSceneFolder, _graphicsSceneFolderID, "", -1, "default", getProperties());
}

const std::string EntitySolverDataProcessing::getSelectedPipeline() 
{
	const std::string selectedPipeline = PropertyHelper::getEntityListProperty(this, "Pipeline to run","Pipeline")->getValueName();
	return selectedPipeline;
}
