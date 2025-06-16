#pragma once
#include "OTCore/CoreTypes.h"
#include <string>
#include "EntityBase.h"

class ViewVisualisationHandler
{
public:
	void handleVisualisationRequest(ot::UID _entityID, const std::string& _visualisationType, bool _setAsActiveView, bool _overrideContent = true);
	void handleRenaming(ot::UID _entityID);
private:
	void setupPlot(EntityBase* _plotEntityBase, bool _setAsActiveView);
};
