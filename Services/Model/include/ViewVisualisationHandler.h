#pragma once
#include "OTCore/CoreTypes.h"
#include <string>
class ViewVisualisationHandler
{
public:
	void handleVisualisationRequest(ot::UID _entityID, const std::string& _visualisationType, bool _setAsActiveView, bool _overrideContent = true);

};
