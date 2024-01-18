#pragma once
#include "ClassFactory.h"

class EntityHandler
{
public:
	static ClassFactory& getClassFactory()
	{
		static ClassFactory instance;
		return instance;
	};
	EntityBase* readEntityFromEntityIDandVersion(ot::UID _entityID, ot::UID _version);

private:
};
