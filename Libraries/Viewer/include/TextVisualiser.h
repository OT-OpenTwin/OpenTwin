#pragma once
#include "IVisualiser.h"
#include "OTCore/CoreTypes.h"
#include <string>

class TextVisualiser : public IVisualiser
{
public:
	TextVisualiser(ot::UID _modelEntityID) :m_modelEntityID(_modelEntityID) {}
	void visualise() override;
private:
	ot::UID m_modelEntityID;
};
