#pragma once
#include "OTCore/CoreTypes.h"

class Visualiser
{
public:
	Visualiser(ot::UID _entityID)
		:m_entityID(_entityID){}

	virtual void visualise() = 0;
	bool isVisible() { return m_visible; }
	void setVisible(bool _visible) { m_visible = _visible;}
protected:
	bool m_visible = true;
	ot::UID m_entityID = 0;
};
