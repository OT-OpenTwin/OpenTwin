// @otlicense

#pragma once
#include "OTCore/CoreTypes.h"
#include <string>
#include "EntityBase.h"
#include "OTGui/VisualisationCfg.h"

class ViewVisualisationHandler
{
public:
	void handleVisualisationRequest(ot::UID _entityID, ot::VisualisationCfg& _visualisationCfg);
	void handleRenaming(ot::UID _entityID);
private:
	void setupPlot(EntityBase* _plotEntityBase, bool _setAsActiveView);
	void setupGraphicsScene(EntityBase* _container);
};
