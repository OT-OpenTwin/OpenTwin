//! @file GraphicsElement.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsElement.h"

ot::GraphicsElement::GraphicsElement() 
	: m_state(NoState), m_scene(nullptr)
{
}

ot::GraphicsElement::~GraphicsElement() {
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::GraphicsElement::setGraphicsElementState(GraphicsElementState _state, bool _active) {
	if (m_state.flagIsSet(_state) == _active) return;
	m_state.setFlag(_state, _active);
	this->graphicsElementStateChanged(m_state);
}

void ot::GraphicsElement::setGraphicsElementStateFlags(const GraphicsElementStateFlags& _state) {
	if (m_state == _state) return;
	m_state = _state;
	this->graphicsElementStateChanged(m_state);
}

std::list<ot::GraphicsElement*> ot::GraphicsElement::getAllGraphicsElements(void) {
	return std::list<GraphicsElement*>({ this });
}