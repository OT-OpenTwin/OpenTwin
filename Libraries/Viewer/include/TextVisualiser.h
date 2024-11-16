#pragma once
#include "Visualiser.h"
#include "OTCore/CoreTypes.h"
#include <string>

class TextVisualiser : public Visualiser
{
public:
	TextVisualiser(ot::UID _modelEntityID) :Visualiser(_modelEntityID) {}
	void visualise() override;
};
