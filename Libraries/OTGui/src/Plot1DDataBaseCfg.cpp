// @otlicense
// File: Plot1DDataBaseCfg.cpp
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
#include "OTGui/Plot1DDataBaseCfg.h"

ot::Plot1DDataBaseCfg::Plot1DDataBaseCfg() {}

ot::Plot1DDataBaseCfg::Plot1DDataBaseCfg(const Plot1DDataBaseCfg& _other) {
	*this = _other;
}

ot::Plot1DDataBaseCfg::Plot1DDataBaseCfg(const Plot1DCfg& _other)
	: Plot1DCfg(_other)
{}

ot::Plot1DDataBaseCfg::~Plot1DDataBaseCfg() {}

ot::Plot1DDataBaseCfg& ot::Plot1DDataBaseCfg::operator = (const Plot1DDataBaseCfg& _other) {
	if (this == &_other) return *this;
	Plot1DCfg::operator=(_other);

	m_curves = _other.m_curves;

	return *this;
}

ot::Plot1DDataBaseCfg& ot::Plot1DDataBaseCfg::operator=(const Plot1DCfg& _other) {
	Plot1DCfg::operator=(_other);
	
	return *this;
}

void ot::Plot1DDataBaseCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	Plot1DCfg::addToJsonObject(_object, _allocator);

	JsonArray curvesArray;
	for (const Plot1DCurveCfg& curve : m_curves) {
		JsonObject curveObject;
		curve.addToJsonObject(curveObject, _allocator);
		curvesArray.PushBack(curveObject, _allocator);
	}
	_object.AddMember("Curves", curvesArray, _allocator);
}

void ot::Plot1DDataBaseCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_curves.clear();
	Plot1DCfg::setFromJsonObject(_object);

	ConstJsonObjectList curvesArray = json::getObjectList(_object, "Curves");
	for (const ConstJsonObject& curveObject : curvesArray) {
		Plot1DCurveCfg newCurve;
		newCurve.setFromJsonObject(curveObject);
		m_curves.push_back(newCurve);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::Plot1DDataBaseCfg::addCurve(const Plot1DCurveCfg& _curve) {
	m_curves.push_back(_curve);
}

void ot::Plot1DDataBaseCfg::addCurve(Plot1DCurveCfg&& _curve) {
	m_curves.push_back(std::move(_curve));
}

bool ot::Plot1DDataBaseCfg::updateCurveVersion(ot::UID _curveEntityUID, ot::UID _newCurveEntityVersion) {
	bool changed = false;
	for (Plot1DCurveCfg& curve : m_curves) {
		if (curve.getEntityID() == _curveEntityUID && curve.getEntityVersion() != _newCurveEntityVersion) {
			curve.setEntityID(_newCurveEntityVersion);
			changed = true;
		}
	}
	return changed;
}
