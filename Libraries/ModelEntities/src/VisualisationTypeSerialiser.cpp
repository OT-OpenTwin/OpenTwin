// @otlicense

#include "VisualisationTypeSerialiser.h"
#include "IVisualisationTable.h"
#include "IVisualisationText.h"
#include "IVisualisationImage.h"
#include "OTGui/VisualisationTypes.h"

void VisualisationTypeSerialiser::operator()(EntityBase* _baseEntity, ot::JsonValue& _object, ot::JsonAllocator& _allocator)
{
	ot::VisualisationTypes visTypes;
	
	if (dynamic_cast<IVisualisationTable*>(_baseEntity)) {
		visTypes.addTableVisualisation();
	}
	if (dynamic_cast<IVisualisationText*>(_baseEntity)) {
		visTypes.addTextVisualisation();
	}
	if (dynamic_cast<IVisualisationImage*>(_baseEntity)) {
		visTypes.addImageVisualisation();
	}

	if (visTypes.hasVisualisations()) {
		visTypes.addToJsonObject(_object, _allocator);
	}
}
