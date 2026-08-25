// @otlicense

// OpenTwin header
#include "OTCore/RAII/RAIIBase.h"

ot::RAIIBase::RAIIBase() : m_isValid(true) {};

void ot::RAIIBase::dismiss()
{
	m_isValid = false;
	this->invalidate();
}

void ot::RAIIBase::finalize()
{
	if (this->isValid())
	{
		this->execute();
	}
	this->dismiss();
}