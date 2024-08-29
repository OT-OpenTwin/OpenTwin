//! @file GraphicsElement.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsElement.h"

ot::GraphicsElement::GraphicsElement() 
	: m_state(NoState)
{

}

ot::GraphicsElement::~GraphicsElement() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::GraphicsElement::setGraphicsElementState(GraphicsElementState _state, bool _active) {
	m_state.setFlag(_state, _active);
	this->graphicsElementStateChanged(m_state);
}

void ot::GraphicsElement::setGraphicsElementStateFlags(const GraphicsElementStateFlags& _state) {
	m_state = _state;
	this->graphicsElementStateChanged(m_state);
}