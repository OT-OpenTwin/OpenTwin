//! @file GraphicsItem.hpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/LogDispatcher.h"

template <class T>
T* ot::GraphicsItem::getItemConfiguration(void) {
	T* ret = dynamic_cast<T*>(m_config);
	OTAssertNullptr(ret);
	return ret;
}

template <class T>
const T* ot::GraphicsItem::getItemConfiguration(void) const {
	T* ret = dynamic_cast<T*>(m_config);
	OTAssertNullptr(ret);
	return ret;
}