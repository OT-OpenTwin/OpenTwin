// @otlicense
// File: VisualisationTypes.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTGui/VisualisationTypes.h"

bool ot::VisualisationTypes::operator==(const VisualisationTypes& _other) const {
	if (m_visualisations != _other.m_visualisations) {
		return false;
	}
	if (m_customViewFlags.size() != _other.m_customViewFlags.size()) {
		return false;
	}
	for (const auto& it : m_customViewFlags) {
		auto itOther = _other.m_customViewFlags.find(it.first);
		if (itOther == _other.m_customViewFlags.end()) {
			return false;
		}
		if (it.second != itOther->second) {
			return false;
		}
	}

	return true;
}

bool ot::VisualisationTypes::operator!=(const VisualisationTypes& _other) const {
	return !((*this) == _other);
}

void ot::VisualisationTypes::setVisualisations(const VisTypes& _types) {
	if (_types != m_visualisations) {
		m_visualisations = _types;
		m_visualisationsModified = true;
	}
}

void ot::VisualisationTypes::addTextVisualisation() {
	if (m_visualisations.has(VisualisationType::Text) == false) {
		m_visualisations |= VisualisationType::Text;
		m_visualisationsModified = true;
	}
}

void ot::VisualisationTypes::addTableVisualisation() {
	if (m_visualisations.has(VisualisationType::Table) == false) {
		m_visualisations |= VisualisationType::Table;
		m_visualisationsModified = true;
	}
}

void ot::VisualisationTypes::addPlot1DVisualisation() {
	if (m_visualisations.has(VisualisationType::Plot1D) == false) {
		m_visualisations |= VisualisationType::Plot1D;
		m_visualisationsModified = true;
	}
}

void ot::VisualisationTypes::addCurveVisualisation() {
	if (m_visualisations.has(VisualisationType::Curve) == false) {
		m_visualisations |= VisualisationType::Curve;
		m_visualisationsModified = true;
	}
}

void ot::VisualisationTypes::addRangeVisualisation() {
	if (m_visualisations.has(VisualisationType::Range) == false) {
		m_visualisations |= VisualisationType::Range;
		m_visualisationsModified = true;
	}
}

void ot::VisualisationTypes::addGraphicsViewVisualisation() {
	if (m_visualisations.has(VisualisationType::GraphicsView) == false) {
		m_visualisations |= VisualisationType::GraphicsView;
		m_visualisationsModified = true;
	}
}

void ot::VisualisationTypes::addGraphicsItemVisualisation() {
	if (m_visualisations.has(VisualisationType::GraphicsItem) == false) {
		m_visualisations |= VisualisationType::GraphicsItem;
		m_visualisationsModified = true;
	}
}

void ot::VisualisationTypes::addGraphicsConnectionVisualisation() {
	if (m_visualisations.has(VisualisationType::GraphicsConnection) == false) {
		m_visualisations |= VisualisationType::GraphicsConnection;
		m_visualisationsModified = true;
	}
}

void ot::VisualisationTypes::addImageVisualisation() {
	if (m_visualisations.has(VisualisationType::Image) == false) {
		m_visualisations |= VisualisationType::Image;
		m_visualisationsModified = true;
	}
}

void ot::VisualisationTypes::addPDFVisualisation() {
	if (m_visualisations.has(VisualisationType::PDF) == false) {
		m_visualisations |= VisualisationType::PDF;
		m_visualisationsModified = true;
	}
}

void ot::VisualisationTypes::setCustomViewFlags(VisualisationType _visType, ot::WidgetViewBase::ViewFlags _flags) {
	auto it = m_customViewFlags.find(_visType);
	if (it == m_customViewFlags.end()) {
		m_customViewFlags.emplace(_visType, _flags);
		m_customViewFlagsModified = true;
	}
	else if (it->second != _flags) {
		it->second = _flags;
		m_customViewFlagsModified = true;
	}
}

std::optional<ot::WidgetViewBase::ViewFlags> ot::VisualisationTypes::getCustomViewFlags(VisualisationType _visType) const {
	auto it = m_customViewFlags.find(_visType);
	if (it != m_customViewFlags.end()) {
		return it->second;
	}
	else {
		return std::nullopt;
	}
}

void ot::VisualisationTypes::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("VisualisationTypes", static_cast<uint64_t>(m_visualisations), _allocator);

	JsonArray customFlagsArr;
	for (const auto& it : m_customViewFlags) {
		JsonObject flagObj;
		flagObj.AddMember("Vis", static_cast<uint64_t>(it.first), _allocator);
		flagObj.AddMember("Flags", JsonArray(WidgetViewBase::toStringList(it.second), _allocator), _allocator);
		customFlagsArr.PushBack(flagObj, _allocator);
	}
	_object.AddMember("CustomViewFlags", customFlagsArr, _allocator);
}

void ot::VisualisationTypes::setFromJsonObject(const ot::ConstJsonObject& _object) {
	if (_object.HasMember("VisualisationTypes")) {
		m_visualisations = VisTypes(json::getUInt64(_object, "VisualisationTypes"));
	}
	else {
		m_visualisations = None;
	}

	m_customViewFlags.clear();
	if (_object.HasMember("CustomViewFlags")) {
		for (const auto& flagObj : json::getObjectList(_object, "CustomViewFlags")) {
			auto visType = static_cast<VisualisationType>(json::getUInt64(flagObj, "Vis"));
			auto flags = WidgetViewBase::stringListToViewFlags(json::getStringList(flagObj, "Flags"));
			m_customViewFlags.insert_or_assign(visType, flags);
		}
	}
}

