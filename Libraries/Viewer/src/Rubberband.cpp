// @otlicense
// File: Rubberband.cpp
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

// Project header
#include "stdafx.h"
#include "Rubberband.h"

// RBE header
#include <rbeWrapper/RubberbandOsgWrapper.h>

Rubberband::Rubberband(osg::Switch *_group, ot::serviceID_t _creator, const std::string& _note, const std::string& _configurationJson)
	: m_engine(nullptr), m_creatorId(_creator), m_note(_note)
{
	m_engine = new rbeWrapper::RubberbandOsgWrapper(_group, 0., 0., 0.);
	m_engine->setupFromJson(_configurationJson.c_str());
	if (m_engine->hasNextStep()) {
		m_engine->activateStepOne();
	}
}

Rubberband::~Rubberband() {
	delete m_engine;
}

// ##########################################################################################

// Setter

bool Rubberband::switchToNextStep(void) {
	if (m_engine->hasNextStep()) {
		m_engine->activateNextStep();
		return true;
	}
	else {
		return false;
	}
}

void Rubberband::applyCurrentStep(void) {
	m_engine->applyCurrentStep();
}

// ##########################################################################################

// Getter

void Rubberband::updateCurrentPosition(coordinate_t _u, coordinate_t _v, coordinate_t _w) {
	m_engine->updateCurrent(_u, _v, _w);
}

std::string Rubberband::createPointDataJson(void) {
	return m_engine->pointsJsonArray();
}