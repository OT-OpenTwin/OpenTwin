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
#include "OTViewer/Rubberband.h"
#include "OTViewer/ViewerObjectSelectionHandler.h"

// RBE header
#include "OTRubberbandOSG/RubberbandOsgWrapper.h"

Rubberband::Rubberband(osg::Switch *_group, ot::serviceID_t _creator, const std::string& _note, const std::string& _configurationJson)
	: m_engine(nullptr), m_creatorId(_creator), m_note(_note), m_currentlastHeight(0.0), m_selectionHandler(nullptr)
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

	m_selectedCoordinates.push_back(m_currentCoordinate);
	m_selectedLastHeight.push_back(m_currentlastHeight);
	m_lastPointInPlaneCoordinates.push_back(m_currentLastPointInPlane);

	if (m_engine->hasNextStep()) {
		m_engine->activateNextStep();
		return true;
	}
	else {
		return false;
	}
}

bool Rubberband::switchToPreviousStep(void) {

	// The rubberband mechanism does not have an undo operation for a point. Therefore, we clear it and
	// replay all points except for the last one

	if (m_selectedCoordinates.empty()) return false; // We are at the first operation, so we cannot drop points

	std::tuple< coordinate_t, coordinate_t, coordinate_t> lastCoordinate = m_selectedCoordinates.back();
	std::tuple< coordinate_t, coordinate_t, coordinate_t> lastPointInPlane = m_lastPointInPlaneCoordinates.back();
	double lastHeight = m_selectedLastHeight.back();
	m_selectedCoordinates.pop_back();
	m_selectedLastHeight.pop_back();
	m_lastPointInPlaneCoordinates.pop_back();

	m_engine->clear();
	m_engine->activateStepOne();

	for (auto coordinate : m_selectedCoordinates)
	{
		m_engine->updateCurrent(std::get<0>(coordinate), std::get<1>(coordinate), std::get<2>(coordinate));
		m_engine->activateNextStep();
	}

	if (m_selectionHandler != nullptr)
	{
		m_selectionHandler->setLastHeight(lastHeight);
		m_selectionHandler->setLastPointInPlane(osg::Vec3(std::get<0>(lastPointInPlane), std::get<1>(lastPointInPlane), std::get<2>(lastPointInPlane)));
	}

	m_engine->updateCurrent(std::get<0>(lastCoordinate), std::get<1>(lastCoordinate), std::get<2>(lastCoordinate));


	return true;
}

void Rubberband::applyCurrentStep(void) {
	m_engine->applyCurrentStep();
}

// ##########################################################################################

// Getter

void Rubberband::updateCurrentPosition(coordinate_t _u, coordinate_t _v, coordinate_t _w, double lastHeight, double lastPoint_u, double lastPoint_v, double lastPoint_w, ViewerObjectSelectionHandler* handler) {

	m_currentCoordinate = std::tuple< coordinate_t, coordinate_t, coordinate_t>(_u, _v, _w);
	m_currentlastHeight = lastHeight;
	m_currentLastPointInPlane = std::tuple< coordinate_t, coordinate_t, coordinate_t>(lastPoint_u, lastPoint_v, lastPoint_w);
	m_selectionHandler = handler;

	m_engine->updateCurrent(_u, _v, _w);
}

std::string Rubberband::createPointDataJson(void) {
	return m_engine->pointsJsonArray();
}