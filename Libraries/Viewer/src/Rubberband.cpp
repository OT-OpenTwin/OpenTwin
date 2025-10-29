// @otlicense

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