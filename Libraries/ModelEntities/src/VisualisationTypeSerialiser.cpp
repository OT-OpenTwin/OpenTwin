#include "VisualisationTypeSerialiser.h"
#include "IVisualisationTable.h"
#include "IVisualisationText.h"
#include "OTGui/VisualisationTypes.h"

void VisualisationTypeSerialiser::operator()(EntityBase* _baseEntity, ot::JsonValue& _object, ot::JsonAllocator& _allocator)
{
	ot::VisualisationTypes visTypes;
	IVisualisationTable* visAsTable = dynamic_cast<IVisualisationTable*>(_baseEntity);
	bool hasVisualisation = false;
	if (visAsTable)
	{
		visTypes.addTableVisualisation();
		hasVisualisation = true;
	}
	
	IVisualisationText* visAsText = dynamic_cast<IVisualisationText*>(_baseEntity);
	if (visAsText)
	{
		visTypes.addTextVisualisation();
		hasVisualisation = true;
	}

	if (hasVisualisation)
	{
		visTypes.addToJsonObject(_object, _allocator);
	}
}
