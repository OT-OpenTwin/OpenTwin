//! @file VisualisationTypes.cpp
//! @author Jan Wagner, Alexander Kuester (alexk95)
//! @date September 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/VisualisationTypes.h"

void ot::VisualisationTypes::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("VisualisationTypes", static_cast<uint64_t>(m_visualisations.data()), _allocator);
}

void ot::VisualisationTypes::setFromJsonObject(const ot::ConstJsonObject& _object) {
	if (_object.HasMember("VisualisationTypes")) {
		m_visualisations = static_cast<VisualisationType>(json::getUInt64(_object, "VisualisationTypes"));
	}
	else {
		m_visualisations = None;
	}
}
