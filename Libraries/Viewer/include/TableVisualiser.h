#pragma once
#include "Visualiser.h"

class TableVisualiser : public Visualiser
{
public:
	TableVisualiser(ot::UID _entityID) : Visualiser(_entityID){}
	void visualise() override;
};
