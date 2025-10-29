// @otlicense

#pragma once
#include "EntityBase.h"

class SelectionChangedObserver
{
public:
	virtual void updatedSelection(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons) = 0;
};
