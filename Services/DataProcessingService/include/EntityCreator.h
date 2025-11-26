#pragma once
#include "OTCore/CoreTypes.h"

class EntityCreator
{
public:
	void createManifests();
	void createPipeline();
	void createSolver();

private:
	ot::UID m_manifestFolderID = ot::invalidUID;
	ot::UID m_dataProcessingFolderID = ot::invalidUID;
};
