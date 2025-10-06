#include "EntitySolverDataProcessing.h"



void EntitySolverDataProcessing::createProperties(const std::string& _graphicsSceneFolder, const ot::UID _graphicsSceneFolderID)
{
	EntityPropertiesEntityList::createProperty("Pipeline", "Pipeline to run", _graphicsSceneFolder, _graphicsSceneFolderID, "", -1, "default", getProperties());
}
