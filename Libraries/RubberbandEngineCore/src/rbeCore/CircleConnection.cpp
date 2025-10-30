// @otlicense
// File: CircleConnection.cpp
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

// RBE header
#include <rbeCore/CircleConnection.h>
#include <rbeCore/jsonMember.h>
#include <rbeCore/AbstractPoint.h>
#include <rbeCore/rbeAssert.h>

#include <rbeCalc/AbstractCalculationItem.h>

// C++ header
#include <cassert>

rbeCore::CircleConnection::CircleConnection()
	: AbstractConnection(ctCircle), m_centerPoint(nullptr), m_orientation(coUV), m_radius(nullptr)
{

}

rbeCore::CircleConnection::~CircleConnection() {
	if (m_radius) {
		delete m_radius;
	}
}

// ####################################################################################################################

// Base class functions

std::string rbeCore::CircleConnection::debugInformation(const std::string& _prefix) {
	std::string ret{ "{\n" };
	ret.append(_prefix).append("\t\"" RBE_JSON_CONNECTION_Type "\": \"" RBE_JSON_VALUE_ConnectionType_Circle "\",\n");
	ret.append(_prefix).append("\t\"" RBE_JSON_CONNECTION_CIRCLE_Midpoint "\": ");
	if (m_centerPoint) {
		ret.append(m_centerPoint->debugInformation(_prefix + "\t\t"));
	}
	else {
		ret.append("NULL");
	}
	ret.append(",\n").append(_prefix).append("\t\"" RBE_JSON_CONNECTION_CIRCLE_Orientation "\": \"");
	switch (m_orientation)
	{
	case rbeCore::coUV: ret.append(RBE_JSON_VALUE_CircleOrientation_UV "\""); break;
	case rbeCore::coUW: ret.append(RBE_JSON_VALUE_CircleOrientation_UW "\""); break;
	case rbeCore::coVW: ret.append(RBE_JSON_VALUE_CircleOrientation_VW "\""); break;
	default:
		rbeAssert(0, "Not implemented orientation type");
		break;
	}
	ret.append(",\n").append(_prefix).append("\t\"" RBE_JSON_CONNECTION_CIRCLE_Radius "\": ");
	ret.append(std::to_string(radius()));

	ret.append("\n").append(_prefix).append("}");
	return ret;
}

void rbeCore::CircleConnection::addToJsonArray(RubberbandEngine * _engine, std::stringstream& _array, bool& _first) {
	if (_first) { _first = false; }
	else { _array << ","; }
	_array << "{\"" RBE_JSON_CONNECTION_Type "\":\"" RBE_JSON_VALUE_ConnectionType_Circle "\",\"" RBE_JSON_CONNECTION_CIRCLE_Midpoint "\":";
	
	if (m_centerPoint) { m_centerPoint->addAsJsonObject(_array); }
	else { _array << "NULL"; }

	_array << ",\"" RBE_JSON_CONNECTION_CIRCLE_Orientation "\": \"" << orientationToString() << "\",\"" RBE_JSON_CONNECTION_CIRCLE_Radius "\": ";
	_array << radius() << "}";
}

// ####################################################################################################################

// Setter

void rbeCore::CircleConnection::setRadius(rbeCalc::AbstractCalculationItem * _radius) {
	if (m_radius) { delete m_radius; }
	m_radius = _radius;
}

// ####################################################################################################################

// Getter

coordinate_t rbeCore::CircleConnection::radius(void) const {
	if (m_radius) {
		return m_radius->value();
	}
	else {
		return 0.;
	}
}

std::string rbeCore::CircleConnection::orientationToString(void) const {
	switch (m_orientation)
	{
	case rbeCore::coUV: return RBE_JSON_VALUE_CircleOrientation_UV;
	case rbeCore::coUW: return RBE_JSON_VALUE_CircleOrientation_UW;
	case rbeCore::coVW: return RBE_JSON_VALUE_CircleOrientation_VW;
	default:
		rbeAssert(0, "Not implemented orientation type");
		return "";
		break;
	}
}
