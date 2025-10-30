// @otlicense
// File: CircleConnection.h
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

// RBE header
#include <rbeCore/AbstractConnection.h>

namespace rbeCalc {
	class AbstractCalculationItem;
}

namespace rbeCore {

	class AbstractPoint;

	class RBE_API_EXPORT CircleConnection : public AbstractConnection {
	public:
		CircleConnection();
		virtual ~CircleConnection();

		// #######################################################################################################
		
		// Base class functions

		virtual std::string debugInformation(const std::string& _prefix) override;

		virtual void addToJsonArray(RubberbandEngine * _engine, std::stringstream& _array, bool& _first) override;

		// #######################################################################################################

		// Setter

		void setCenterpoint(AbstractPoint * _point) { m_centerPoint = _point; }

		void setRadius(rbeCalc::AbstractCalculationItem * _radius);

		void setOrientation(CircleOrientation _orientation) { m_orientation = _orientation; }

		// #######################################################################################################

		// Getter

		AbstractPoint * centerPoint(void) { return m_centerPoint; }

		coordinate_t radius(void) const;

		CircleOrientation orientation(void) const { return m_orientation; }

		std::string orientationToString(void) const;

	private:

		AbstractPoint *							m_centerPoint;
		rbeCalc::AbstractCalculationItem *		m_radius;
		CircleOrientation						m_orientation;

		CircleConnection(CircleConnection&) = delete;
		CircleConnection& operator = (CircleConnection&) = delete;
	};

}