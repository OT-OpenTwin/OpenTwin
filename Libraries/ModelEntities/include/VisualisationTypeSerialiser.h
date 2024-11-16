#pragma once
#include "OTCore/JSON.h"
#include "EntityBase.h"
class VisualisationTypeSerialiser
{
public:
	void operator() (EntityBase* _baseEntity, ot::JsonValue& _object, ot::JsonAllocator& _allocator );
};
