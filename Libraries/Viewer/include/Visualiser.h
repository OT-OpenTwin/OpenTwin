#pragma once
#include "OTCore/CoreTypes.h"

class Visualiser
{
public:
	Visualiser(ot::UID _entityID)
		:m_entityID(_entityID){}

	virtual void visualise() = 0;
	bool isVisible() { return m_visible; }
	//! @brief Switch to turn a visualisation type off
	void setVisible(bool _visible) { m_visible = _visible;}
	//! @brief Switch for deciding if a data pull is necessary or if a lookup is necessary, if the displayed data may still be up-to date
	void setViewIsOpen(bool _viewIsOpen) { m_viewIsOpen = _viewIsOpen; }
	bool viewIsCurrentlyOpen() { return m_viewIsOpen; }

protected:
	bool m_visible = true;
	bool m_viewIsOpen = false;
	ot::UID m_entityID = 0;
};
