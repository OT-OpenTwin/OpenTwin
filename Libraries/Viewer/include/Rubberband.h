// @otlicense
// File: Rubberband.h
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

#pragma once

#include "OTCore/CoreTypes.h"

// C++ header
#include <string>

#include <rbeCore/dataTypes.h>
#include <osg/Array>

namespace rbeWrapper {
	class RubberbandOsgWrapper;
}
namespace osg {
	class Switch;
}

class Rubberband {
public:

	Rubberband(osg::Switch *_group, ot::serviceID_t _creator, const std::string& _note, const std::string& _configurationJson);

	virtual ~Rubberband();

	// ##########################################################################################

	// Setter

	bool switchToNextStep(void);

	void applyCurrentStep(void);

	// ##########################################################################################

	// Getter

	ot::serviceID_t creator(void) const { return m_creatorId; }

	const std::string& note(void) const { return m_note; }

	rbeWrapper::RubberbandOsgWrapper * engine(void) { return m_engine; }

	void updateCurrentPosition(coordinate_t _u, coordinate_t _v, coordinate_t _w);

	std::string createPointDataJson(void);

private:
	ot::serviceID_t						m_creatorId;
	std::string							m_note;
	rbeWrapper::RubberbandOsgWrapper *	m_engine;

	Rubberband() = delete;
	Rubberband(Rubberband&) = delete;
	Rubberband& operator = (Rubberband&) = delete;
};