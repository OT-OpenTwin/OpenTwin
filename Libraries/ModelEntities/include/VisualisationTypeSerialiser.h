#pragma once
#include "OTCore/JSON.h"
#include "EntityBase.h"

//! @brief Serialises the visualiser depending on the implemented interfaces. This way the inheriting classes don't need to handle the visualiser.
class VisualisationTypeSerialiser
{
public:
	void operator() (EntityBase* _baseEntity, ot::JsonValue& _object, ot::JsonAllocator& _allocator );
};
