// @otlicense

/*
 *	File:		RubberbandConfiguration.cpp
 *	Package:	rbeCore
 *
 *  Created on: November 07, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <rbeCore/RubberbandConfiguration.h>
#include <rbeCore/rbeAssert.h>
#include <rbeCore/jsonMember.h>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

rbeCore::RubberbandConfiguration::RubberbandConfiguration() {

}

rbeCore::RubberbandConfiguration::~RubberbandConfiguration() {
	clear();
}

// #############################################################################################

void rbeCore::RubberbandConfiguration::setToDefaultCuboid() {
	clear();
	*this
		// Create step 1: The base
		//    4 Points on the UV axis (width and depth)
		//                               
		//      4          3
		//      #__________#
		//     /          /
		//    /          /
		//   #__________#
		//   1          2
		//
		<< (cfgStep(cfgStep::UV, false) << cfgPoint(1, ORIGIN, ORIGIN, ORIGIN) << cfgPoint(2, CURRENT, ORIGIN, ORIGIN) <<
			cfgPoint(3, CURRENT, CURRENT, ORIGIN) << cfgPoint(4, ORIGIN, CURRENT, ORIGIN) <<
			cfgLineConnection(1, 2) << cfgLineConnection(2, 3) << cfgLineConnection(3, 4) << cfgLineConnection(4, 1))

		// Create step 2: The brick
		//    4 Points on the W axis (height) connected by a line
		//       + also display the lines of step 1 (otherwise the bottom four lines would be missing
		//       + Set the step to "may end without = true" so we can stop after step 1 and create a 2D rectangle
		//      
		//       8          7
		//       #__________#
		//      /          /|      
		//     / |        / |      
		//  5 #__________# 6|
		//    |  #  -  - |- #
		//    | .        | /
		//    |'         |/
		//    #----------#
		//
		<< (cfgStep(cfgStep::W, true) << cfgPoint(5, "$1.U", "$1.V", CURRENT) << cfgPoint(6, "$2.U", "$2.V", CURRENT) <<
			cfgPoint(7, "$3.U", "$3.V", CURRENT) << cfgPoint(8, "$4.U", "$4.V", CURRENT) <<
			cfgHistoryConnection(1) <<
			cfgLineConnection(5, 6) << cfgLineConnection(6, 7) << cfgLineConnection(7, 8) << cfgLineConnection(8, 5) <<
			cfgLineConnection(1, 5) << cfgLineConnection(2, 6) << cfgLineConnection(3, 7) << cfgLineConnection(4, 8))
		/*
		<< (cfgStep(cfgStep::UV, true) << cfgPoint(9, "$5.U+$7.U>Current.U", "$5.V+$7.V>Current.V", "$5.W") << cfgPoint(10, "$6.U-$7.U>Current.U", "$6.V+$7.V>Current.V", "$6.W") <<
			cfgPoint(11, "$7.U-$7.U>Current.U", "$7.V-$7.V>Current.V", "$7.W") << cfgPoint(12, "$8.U+$7.U>Current.U", "$8.V-$7.V>Current.V", "$8.W") <<
			cfgLimit(Umin, 1) << cfgLimit(Umax, 2) << cfgLimit(Vmin, 3) << cfgLimit(Vmax, 2) <<
			cfgHistoryConnection(2) <<
			cfgLineConnection(9, 10) << cfgLineConnection(10, 11) << cfgLineConnection(11, 12) << cfgLineConnection(12, 9))
		*/
	;
}

void rbeCore::RubberbandConfiguration::setToDefaultCylinder() {
	clear();
	*this

		<< (cfgStep(cfgStep::UV, false) << cfgPoint(1, ORIGIN, ORIGIN, ORIGIN) << cfgPoint(2, "Origin.U-Current>Origin", ORIGIN, ORIGIN) <<
			cfgPoint(3, ORIGIN, "Origin.V-Current>Origin", ORIGIN) << cfgPoint(4, "Origin.U+Current>Origin", ORIGIN, ORIGIN) <<
			cfgPoint(5, ORIGIN, "Origin.V+Current>Origin", ORIGIN) << 
			cfgPoint(100, "Origin.U-Current>Origin/3", "Origin.V-Current>Origin/3", ORIGIN) << cfgPoint(101, "Origin.U+Current>Origin/3", "Origin.V+Current>Origin/3", ORIGIN) <<
			cfgPoint(102, "Origin.U+Current>Origin/3", "Origin.V-Current>Origin/3", ORIGIN) << cfgPoint(103, "Origin.U-Current>Origin/3", "Origin.V+Current>Origin/3", ORIGIN) <<
			cfgCircleConnection(1, "$1>$2", rbeCore::coUV, false) << cfgLineConnection(100, 101, true) << cfgLineConnection(102, 103, true) <<
			cfgLineConnection(2, 4, true) << cfgLineConnection(3, 5, true))

		<< (cfgStep(cfgStep::W, false) << cfgPoint(6, "$1.U", "$1.V", CURRENT) << cfgPoint(7, "$2.U", "$2.V", CURRENT) <<
			cfgPoint(8, "$3.U", "$3.V", CURRENT) << cfgPoint(9, "$4.U", "$4.V", CURRENT) << cfgPoint(10, "$5.U", "$5.V", CURRENT) <<
			cfgHistoryConnection(1) << cfgLineConnection(2, 7) << cfgLineConnection(3, 8) << cfgLineConnection(4, 9) << cfgLineConnection(5, 10) <<
			cfgCircleConnection(6, "$6>$7", rbeCore::coUV, false))
		;
}

void rbeCore::RubberbandConfiguration::setToDefaultSphere(void) {
	clear();

	*this
		<< (cfgStep(cfgStep::UV, false) <<
			cfgPoint(1, ORIGIN, ORIGIN, ORIGIN) <<
			cfgPoint(2, "Origin.U-Current>Origin", ORIGIN, ORIGIN) <<
			cfgPoint(3, ORIGIN, "Origin.V-Current>Origin", ORIGIN) <<
			cfgPoint(4, "Origin.U+Current>Origin", ORIGIN, ORIGIN) <<
			cfgPoint(5, ORIGIN, "Origin.V+Current>Origin", ORIGIN) <<
			cfgPoint(6, ORIGIN, ORIGIN, "Origin.W-Current>Origin") <<
			cfgPoint(7, ORIGIN, ORIGIN, "Origin.W+Current>Origin") <<

			cfgLineConnection(2, 4, true) <<
			cfgLineConnection(3, 5, true) <<
			cfgLineConnection(6, 7, true) <<

			cfgCircleConnection(1, "$2>$1", rbeCore::coUV, false) <<
			cfgCircleConnection(1, "$2>$1", rbeCore::coUW, false) <<
			cfgCircleConnection(1, "$2>$1", rbeCore::coVW, false))
		;
}

void rbeCore::RubberbandConfiguration::setToDefaultTorus(void){
	clear();
	
	*this
		<< (cfgStep(cfgStep::UV, false) <<
			// points for first circle
			cfgPoint(1, ORIGIN, ORIGIN, ORIGIN) <<
			cfgPoint(2, "Origin.U-Current>Origin", ORIGIN, ORIGIN) <<
			cfgPoint(3, ORIGIN, "Origin.V-Current>Origin", ORIGIN) <<
			cfgPoint(4, "Origin.U+Current>Origin", ORIGIN, ORIGIN) << 
			cfgPoint(5, ORIGIN, "Origin.V+Current>Origin", ORIGIN) <<

			// first circle on the UV level
			cfgCircleConnection(1, "$2>$1", rbeCore::coUV, false))

		<< (cfgStep(cfgStep::UV, true) <<
			// points for second circle
			cfgPoint(6, "Origin.U-Current>Origin", ORIGIN, ORIGIN) <<
			cfgPoint(7, ORIGIN, "Origin.V-Current>Origin", ORIGIN) <<
			cfgPoint(8, "Origin.U+Current>Origin", ORIGIN, ORIGIN)		   <<
			cfgPoint(9, ORIGIN, "Origin.V+Current>Origin", ORIGIN)		   <<

			// points for middle points of verticle points
			// the id of the point represents between which two points 
			//		the middle point for the vertical circle is e.g. 206 - between 2 and 6 
			cfgPoint(206, "Origin.U-$6>$1/2-$2>$1/2", ORIGIN, ORIGIN) <<
			cfgPoint(307, ORIGIN, "Origin.V-$7>$1/2-$3>$1/2", ORIGIN) <<
			cfgPoint(408, "Origin.U+$6>$1/2+$2>$1/2", ORIGIN, ORIGIN) <<
			cfgPoint(509, ORIGIN, "Origin.V+$9>$1/2+$5>$1/2", ORIGIN) <<

			cfgHistoryConnection(1) <<

			// second circle on the UV level
			cfgCircleConnection(1, "$6>$1", rbeCore::coUV, false) <<
			
			// vertical circles
			cfgCircleConnection(206, "$6>$206", rbeCore::coUW, false) << 
			cfgCircleConnection(307, "$7>$307", rbeCore::coVW, false) << 
			cfgCircleConnection(408, "$8>$408", rbeCore::coUW, false) <<
			cfgCircleConnection(509, "$9>$509", rbeCore::coVW, false))
		;
}

void rbeCore::RubberbandConfiguration::setToDefaultCone(void) {
	clear();

	*this
		// first step: create the bottom circle
		<< (cfgStep(cfgStep::UV, false) <<

			// points for the bottom circle
			cfgPoint(1, ORIGIN, ORIGIN, ORIGIN) <<
			cfgPoint(2, "Origin.U-Current>Origin", ORIGIN, ORIGIN) <<
			cfgPoint(3, ORIGIN, "Origin.V-Current>Origin", ORIGIN) <<
			cfgPoint(4, "Origin.U+Current>Origin", ORIGIN, ORIGIN) <<
			cfgPoint(5, ORIGIN, "Origin.V+Current>Origin", ORIGIN) <<

			// points for small cross in the bottom circle
			cfgPoint(100, "Origin.U-Current>Origin/3", "Origin.V-Current>Origin/3", ORIGIN) <<
			cfgPoint(101, "Origin.U+Current>Origin/3", "Origin.V-Current>Origin/3", ORIGIN) <<
			cfgPoint(102, "Origin.U+Current>Origin/3", "Origin.V+Current>Origin/3", ORIGIN) <<
			cfgPoint(103, "Origin.U-Current>Origin/3", "Origin.V+Current>Origin/3", ORIGIN) <<

			// lines for big cross in the bottom circle on the XY-axis
			cfgLineConnection(2, 4, true) << cfgLineConnection(3, 5, true) <<

			// lines for small cross in the bottom circle
			cfgLineConnection(100, 102, true) << cfgLineConnection(101, 103, true) <<

			cfgCircleConnection(1, "$2>$1", rbeCore::coUV, false))

		// second step: decide the hight of the cone
		<< (cfgStep(cfgStep::W, false) <<

			// middle point of the top circle
			cfgPoint(6, "$1.U", "$1.V", CURRENT) <<

			// points for the top circle
			cfgPoint(7, "$2.U", "$2.V", CURRENT)  <<
			cfgPoint(8, "$3.U", "$3.V", CURRENT)  <<
			cfgPoint(9, "$4.U", "$4.V", CURRENT)  <<
			cfgPoint(10, "$5.U", "$5.V", CURRENT) <<

			cfgHistoryConnection(1) <<

			// vertical lines
			cfgLineConnection(2, 7, true)  <<
			cfgLineConnection(3, 8, true)  <<
			cfgLineConnection(4, 9, true)  <<
			cfgLineConnection(5, 10, true) <<

			cfgCircleConnection(6, "$6>$7", rbeCore::coUV, true))

		// third step: create the top circle of the cone
		<< (cfgStep(cfgStep(cfgStep::UV, false)) <<

			//points for the top circle
			cfgPoint(11, "$6.U-Current>Origin", "$6.V", "$6.W") <<
			cfgPoint(12, "$6.U", "$6.V-Current>Origin", "$6.W") <<
			cfgPoint(13, "$6.U+Current>Origin", "$6.V", "$6.W") <<
			cfgPoint(14, "$6.U", "$6.V+Current>Origin", "$6.W") <<
			
			cfgHistoryConnection(2) << 

			//vertical lines
			cfgLineConnection(2, 11, false) <<
			cfgLineConnection(3, 12, false) <<
			cfgLineConnection(4, 13, false) <<
			cfgLineConnection(5, 14, false) <<
			
			cfgCircleConnection(6, "$6>$11", rbeCore::coUV, false))
		;
}

void rbeCore::RubberbandConfiguration::setToDefaultPyramid(void) {
	clear();
	 
	*this
		<< (cfgStep(cfgStep::UV, false) <<

			// points for the sqaure base of the pyramid
			cfgPoint(1, ORIGIN, ORIGIN, ORIGIN) <<
			cfgPoint(2, CURRENT, ORIGIN, ORIGIN) <<
			cfgPoint(3, CURRENT, CURRENT, ORIGIN) <<
			cfgPoint(4, ORIGIN, CURRENT, ORIGIN) <<

			// lines for the sqaure base
			cfgLineConnection(1, 2, false) <<
			cfgLineConnection(2, 3, false) <<
			cfgLineConnection(3, 4, false) <<
			cfgLineConnection(4, 1, false) <<

			// lines for the x in the middle
			cfgLineConnection(1, 3, false) <<
			cfgLineConnection(2, 4, false)
		)

		<< (cfgStep(cfgStep::W, true) <<
			
			// points in the middle of the square base
			cfgPoint(5, "$3.U/2+$1.U/2", "$3.V/2+$1.V/2", ORIGIN) <<
			cfgPoint(6, "$5.U", "$5.V", CURRENT) <<
			
			cfgHistoryConnection(1) <<

			cfgLineConnection(5, 6, false) <<
			cfgLineConnection(1, 6, false) <<
			cfgLineConnection(2, 6, false) <<
			cfgLineConnection(3, 6, false) <<
			cfgLineConnection(4, 6, false)
		)
		;
}

// #############################################################################################

// Getter

void rbeCore::RubberbandConfiguration::addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object) const {
	if (!_object.IsObject()) { rbeAssert(0, "The provided json value is not an object @RubberbandConfiguration"); return; }

	rapidjson::Value stepData(rapidjson::kArrayType);
	int stepId{ 1 };
	for (auto s : m_steps) {
		rapidjson::Value stepEntry(rapidjson::kObjectType);
		s.addToJsonObject(_doc, stepEntry, stepId++);
		stepData.PushBack(stepEntry, _doc.GetAllocator());
	}
	_object.AddMember(RBE_JSON_DOC_RubberbandSteps, stepData, _doc.GetAllocator());
}

// #############################################################################################

// Operators

void rbeCore::RubberbandConfiguration::clear(void) {
	m_steps.clear();
}

rbeCore::RubberbandConfiguration& rbeCore::RubberbandConfiguration::operator << (const cfgStep& _step) {
	m_steps.push_back(_step);
	return *this;
}

std::string rbeCore::RubberbandConfiguration::toJson(void) const {
	rapidjson::Document doc;
	rapidjson::Value obj(rapidjson::kObjectType);
	doc.SetObject();
	addToJsonObject(doc, obj);

	// Create String buffer
	rapidjson::StringBuffer buffer;
	buffer.Clear();

	// Setup the Writer
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	obj.Accept(writer);

	// Return string
	const char * temp = buffer.GetString();
	return std::string(temp);
}


// ##################################################################################################################################################

// ##################################################################################################################################################

// ##################################################################################################################################################

rbeCore::cfgStep::cfgStep(Projection _projection, bool _mayEndWithoutThisStep)
	: m_projection(_projection), m_mayEndWithoutThisStep(_mayEndWithoutThisStep)
{}

rbeCore::cfgStep::cfgStep(const cfgStep& _other)
	: m_projection(_other.m_projection), m_connectionData(_other.m_connectionData), m_mayEndWithoutThisStep(_other.m_mayEndWithoutThisStep)
{
	for (auto pt : _other.m_points) { m_points.push_back(pt); }
}

rbeCore::cfgStep::~cfgStep() {}

rbeCore::cfgStep& rbeCore::cfgStep::operator = (const cfgStep& _other) {
	m_projection = _other.m_projection;
	for (auto pt : _other.m_points) { m_points.push_back(pt); }
	m_connectionData = _other.m_connectionData;
	m_mayEndWithoutThisStep = _other.m_mayEndWithoutThisStep;
	return *this;
}

// #############################################################################################

void rbeCore::cfgStep::addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object, int _id) const {
	if (!_object.IsObject()) { rbeAssert(0, "The provided json value is not an object @cfgStep"); return; }

	// Projection, mayEndWithout and step id..
	_object.AddMember(RBE_JSON_STEP_Step, _id, _doc.GetAllocator());
	_object.AddMember(RBE_JSON_STEP_MayEndWithout, m_mayEndWithoutThisStep, _doc.GetAllocator());
	switch (m_projection)
	{
	case rbeCore::cfgStep::UV: _object.AddMember(RBE_JSON_STEP_Projection, RBE_JSON_VALUE_ProjectionUV, _doc.GetAllocator()); break;
	case rbeCore::cfgStep::W: _object.AddMember(RBE_JSON_STEP_Projection, RBE_JSON_VALUE_ProjectionW, _doc.GetAllocator()); break;
	default:
		rbeAssert(0, "Unknown projection");
		break;
	}
	
	// Points
	rapidjson::Value pointData(rapidjson::kArrayType);
	for (auto p : m_points) {
		rapidjson::Value pointEntry(rapidjson::kObjectType);
		p.addToJsonObject(_doc, pointEntry);
		pointData.PushBack(pointEntry, _doc.GetAllocator());
	}
	_object.AddMember(RBE_JSON_STEP_Points, pointData, _doc.GetAllocator());

	// Limits
	rapidjson::Value limitsData(rapidjson::kArrayType);
	for (auto l : m_limits) {
		rapidjson::Value limitEntry(rapidjson::kObjectType);
		l.addToJsonObject(_doc, limitEntry);
		limitsData.PushBack(limitEntry, _doc.GetAllocator());
	}
	_object.AddMember(RBE_JSON_STEP_Limits, limitsData, _doc.GetAllocator());

	// Connections
	rapidjson::Value connectionData(rapidjson::kArrayType);
	m_connectionData.addToJsonArray(_doc, connectionData);
	_object.AddMember(RBE_JSON_STEP_Connections, connectionData, _doc.GetAllocator());
}

// #############################################################################################

rbeCore::cfgStep& rbeCore::cfgStep::operator << (const cfgPoint& _point) {
	m_points.push_back(_point);
	return *this;
}

rbeCore::cfgStep& rbeCore::cfgStep::operator << (const cfgLineConnection& _connection) {
	m_connectionData.add(_connection);
	return *this;
}

rbeCore::cfgStep& rbeCore::cfgStep::operator << (const cfgCircleConnection& _connection) {
	m_connectionData.add(_connection);
	return *this;
}

rbeCore::cfgStep& rbeCore::cfgStep::operator << (const cfgHistoryConnection& _connection) {
	m_connectionData.add(_connection);
	return *this;
}

rbeCore::cfgStep& rbeCore::cfgStep::operator << (const ConnectionConfigurationData& _connectionData) {
	m_connectionData = _connectionData;
	return *this;
}

rbeCore::cfgStep& rbeCore::cfgStep::operator << (const cfgLimit& _limit) {
	m_limits.push_back(_limit);
	return *this;
}

// ##################################################################################################################################################

// ##################################################################################################################################################

// ##################################################################################################################################################

rbeCore::cfgLimit::cfgLimit(AxisLimit _axisLimit, int _pointId)
	: m_axisLimit(_axisLimit)
{
	switch (_axisLimit)
	{
	case rbeCore::Umin: m_formula = "$" + std::to_string(_pointId) + "U"; break;
	case rbeCore::Umax: m_formula = "$" + std::to_string(_pointId) + "U"; break;
	case rbeCore::Vmin: m_formula = "$" + std::to_string(_pointId) + "V"; break;
	case rbeCore::Vmax: m_formula = "$" + std::to_string(_pointId) + "V"; break;
	case rbeCore::Wmin: m_formula = "$" + std::to_string(_pointId) + "W"; break;
	case rbeCore::Wmax: m_formula = "$" + std::to_string(_pointId) + "W"; break;
	default:
		rbeAssert(0, "Unknown limit @cfgLimit");
	}
}

rbeCore::cfgLimit::cfgLimit(AxisLimit _axisLimit, const std::string& _formula)
	: m_axisLimit(_axisLimit), m_formula(_formula) {}

rbeCore::cfgLimit::cfgLimit(const cfgLimit& _other) 
	: m_axisLimit(_other.m_axisLimit), m_formula(_other.m_formula) {}

rbeCore::cfgLimit::~cfgLimit(void) {}

rbeCore::cfgLimit& rbeCore::cfgLimit::operator = (const cfgLimit& _other) {
	m_axisLimit = _other.m_axisLimit;
	m_formula = _other.m_formula;
	return *this;
}

// ###########################################################################################################

// Getter

void rbeCore::cfgLimit::addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object) const {
	if (!_object.IsObject()) { rbeAssert(0, "The provided json value is not an object @cfgStep"); return; }

	switch (m_axisLimit)
	{
	case rbeCore::Umin: _object.AddMember(RBE_JSON_LIMIT_Axis, RBE_JSON_VALUE_AxisLimit_Umin, _doc.GetAllocator()); break;
	case rbeCore::Umax: _object.AddMember(RBE_JSON_LIMIT_Axis, RBE_JSON_VALUE_AxisLimit_Umax, _doc.GetAllocator()); break;
	case rbeCore::Vmin: _object.AddMember(RBE_JSON_LIMIT_Axis, RBE_JSON_VALUE_AxisLimit_Vmin, _doc.GetAllocator()); break;
	case rbeCore::Vmax: _object.AddMember(RBE_JSON_LIMIT_Axis, RBE_JSON_VALUE_AxisLimit_Vmax, _doc.GetAllocator()); break;
	case rbeCore::Wmin: _object.AddMember(RBE_JSON_LIMIT_Axis, RBE_JSON_VALUE_AxisLimit_Wmin, _doc.GetAllocator()); break;
	case rbeCore::Wmax: _object.AddMember(RBE_JSON_LIMIT_Axis, RBE_JSON_VALUE_AxisLimit_Wmax, _doc.GetAllocator()); break;
	default: rbeAssert(0, "Unknown axis limit");
	}

	rapidjson::Value f(m_formula.c_str(), _doc.GetAllocator());
	//f.SetString(m_formula.c_str(), _doc.GetAllocator());
	_object.AddMember(RBE_JSON_LIMIT_Value, f, _doc.GetAllocator());
}

// ##################################################################################################################################################

// ##################################################################################################################################################

// ##################################################################################################################################################

rbeCore::cfgPoint::cfgPoint(int _id, const std::string& _formulaU, const std::string& _formulaV, const std::string& _formulaW)
	: m_id(_id), m_formulaU(_formulaU), m_formulaV(_formulaV), m_formulaW(_formulaW) {}

rbeCore::cfgPoint::cfgPoint(int _id, DefaultPoint _formulaU, const std::string& _formulaV, const std::string& _formulaW) 
	: m_id(_id), m_formulaU(toString(_formulaU, U)), m_formulaV(_formulaV), m_formulaW(_formulaW) {}

rbeCore::cfgPoint::cfgPoint(int _id, const std::string& _formulaU, DefaultPoint _formulaV, const std::string& _formulaW) 
	: m_id(_id), m_formulaU(_formulaU), m_formulaV(toString(_formulaV, V)), m_formulaW(_formulaW) {}

rbeCore::cfgPoint::cfgPoint(int _id, DefaultPoint _formulaU, DefaultPoint _formulaV, const std::string& _formulaW)
	: m_id(_id), m_formulaU(toString(_formulaU, U)), m_formulaV(toString(_formulaV, V)), m_formulaW(_formulaW) {}

rbeCore::cfgPoint::cfgPoint(int _id, const std::string& _formulaU, const std::string& _formulaV, DefaultPoint _formulaW) 
	: m_id(_id), m_formulaU(_formulaU), m_formulaV(_formulaV), m_formulaW(toString(_formulaW, W)) {}

rbeCore::cfgPoint::cfgPoint(int _id, DefaultPoint _formulaU, const std::string& _formulaV, DefaultPoint _formulaW) 
	: m_id(_id), m_formulaU(toString(_formulaU, U)), m_formulaV(_formulaV), m_formulaW(toString(_formulaW, W)) {}

rbeCore::cfgPoint::cfgPoint(int _id, const std::string& _formulaU, DefaultPoint _formulaV, DefaultPoint _formulaW) 
	: m_id(_id), m_formulaU(_formulaU), m_formulaV(toString(_formulaV, V)), m_formulaW(toString(_formulaW, W)) {}

rbeCore::cfgPoint::cfgPoint(int _id, DefaultPoint _formulaU, DefaultPoint _formulaV, DefaultPoint _formulaW) 
	: m_id(_id), m_formulaU(toString(_formulaU, U)), m_formulaV(toString(_formulaV, V)), m_formulaW(toString(_formulaW, W)) {}

rbeCore::cfgPoint::cfgPoint(const cfgPoint& _other)
	: m_id(_other.m_id), m_formulaU(_other.m_formulaU), m_formulaV(_other.m_formulaV), m_formulaW(_other.m_formulaW) {}

rbeCore::cfgPoint::~cfgPoint() {}

rbeCore::cfgPoint& rbeCore::cfgPoint::operator = (const cfgPoint& _other) {
	m_id = _other.m_id;
	m_formulaU = _other.m_formulaU;
	m_formulaV = _other.m_formulaV;
	m_formulaW = _other.m_formulaW;
	return *this;
}

// ###########################################################################################################

// Getter

void rbeCore::cfgPoint::addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object) const {
	if (!_object.IsObject()) { rbeAssert(0, "The provided json value is not an object @cfgStep"); return; }

	_object.AddMember(RBE_JSON_Point_ID, m_id, _doc.GetAllocator());
	_object.AddMember(RBE_JSON_Point_U, rapidjson::Value(m_formulaU.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
	_object.AddMember(RBE_JSON_Point_V, rapidjson::Value(m_formulaV.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
	_object.AddMember(RBE_JSON_Point_W, rapidjson::Value(m_formulaW.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
}

std::string rbeCore::cfgPoint::toString(DefaultPoint _f, eAxis _axis) {
	std::string ret;
	switch (_f)
	{
	case DefaultPoint::CURRENT: ret = "Current."; break;
	case DefaultPoint::ORIGIN: ret = "Origin."; break;
	default: rbeAssert(0, "Unknown default point @cfgPoint");
	}

	switch (_axis)
	{
	case rbeCore::U: ret.append("U"); break;
	case rbeCore::V: ret.append("V"); break;
	case rbeCore::W: ret.append("W"); break;
	default: rbeAssert(0, "Unknown axis @cfgPoint"); break;
	}
	return ret;
}

// ##################################################################################################################################################

// ##################################################################################################################################################

// ##################################################################################################################################################

rbeCore::ConnectionConfigurationData::ConnectionConfigurationData() {}

rbeCore::ConnectionConfigurationData::ConnectionConfigurationData(const ConnectionConfigurationData& _other) {
	for (auto connection : _other.m_circles) { m_circles.push_back(connection); }
	for (auto connection : _other.m_history) { m_history.push_back(connection); }
	for (auto connection : _other.m_lines) { m_lines.push_back(connection); }
}

rbeCore::ConnectionConfigurationData::~ConnectionConfigurationData() {

}

rbeCore::ConnectionConfigurationData& rbeCore::ConnectionConfigurationData::operator = (const ConnectionConfigurationData& _other) {
	m_circles.clear();
	m_history.clear();
	m_lines.clear();
	for (auto connection : _other.m_circles) { m_circles.push_back(connection); }
	for (auto connection : _other.m_history) { m_history.push_back(connection); }
	for (auto connection : _other.m_lines) { m_lines.push_back(connection); }
	return *this;
}

// ###########################################################################################################

// Setter

void rbeCore::ConnectionConfigurationData::add(const cfgCircleConnection& _connection) {
	m_circles.push_back(_connection);
}

void rbeCore::ConnectionConfigurationData::add(const cfgLineConnection& _connection) {
	m_lines.push_back(_connection);
}

void rbeCore::ConnectionConfigurationData::add(const cfgHistoryConnection& _connection) {
	m_history.push_back(_connection);
}

// ###########################################################################################################

// Getter

void rbeCore::ConnectionConfigurationData::addToJsonArray(rapidjson::Document& _doc, rapidjson::Value& _array) const {
	for (auto connection : m_history) {
		rapidjson::Value connectionEntry(rapidjson::kObjectType);
		connection.addToJsonObject(_doc, connectionEntry);
		_array.PushBack(connectionEntry, _doc.GetAllocator());
	}
	for (auto connection : m_lines) {
		rapidjson::Value connectionEntry(rapidjson::kObjectType);
		connection.addToJsonObject(_doc, connectionEntry);
		_array.PushBack(connectionEntry, _doc.GetAllocator());
	}
	for (auto connection : m_circles) {
		rapidjson::Value connectionEntry(rapidjson::kObjectType);
		connection.addToJsonObject(_doc, connectionEntry);
		_array.PushBack(connectionEntry, _doc.GetAllocator());
	}
}

// ##################################################################################################################################################

// ##################################################################################################################################################

// ##################################################################################################################################################

rbeCore::cfgLineConnection::cfgLineConnection(int _pointFromId, int _pointToId, bool _ignoreInHistory)
	: m_ignoreInHistory(_ignoreInHistory)
{
	m_from = "$" + std::to_string(_pointFromId);
	m_to = "$" + std::to_string(_pointToId);
}

rbeCore::cfgLineConnection::cfgLineConnection(DefaultPoint _pointFrom, int _pointToId, bool _ignoreInHistory)
	: m_ignoreInHistory(_ignoreInHistory)
{
	switch (_pointFrom)
	{
	case rbeCore::ORIGIN: m_from = "Origin"; break;
	case rbeCore::CURRENT: m_from = "Current"; break;
	default: rbeAssert(0, "Invalid point type"); break;
	}
	m_to = "$" + std::to_string(_pointToId);
}

rbeCore::cfgLineConnection::cfgLineConnection(int _pointFromId, DefaultPoint _pointTo, bool _ignoreInHistory)
	: m_ignoreInHistory(_ignoreInHistory)
{
	m_from = "$" + std::to_string(_pointFromId);
	switch (_pointTo)
	{
	case rbeCore::ORIGIN: m_to = "Origin"; break;
	case rbeCore::CURRENT: m_to = "Current"; break;
	default: rbeAssert(0, "Invalid point type"); break;
	}
}

rbeCore::cfgLineConnection::cfgLineConnection(DefaultPoint _pointFrom, DefaultPoint _pointTo, bool _ignoreInHistory)
	: m_ignoreInHistory(_ignoreInHistory)
{
	switch (_pointFrom)
	{
	case rbeCore::ORIGIN: m_from = "Origin"; break;
	case rbeCore::CURRENT: m_from = "Current"; break;
	default: rbeAssert(0, "Invalid point type"); break;
	}
	switch (_pointTo)
	{
	case rbeCore::ORIGIN: m_to = "Origin"; break;
	case rbeCore::CURRENT: m_to = "Current"; break;
	default: rbeAssert(0, "Invalid point type"); break;
	}
}

rbeCore::cfgLineConnection::cfgLineConnection(const cfgLineConnection& _other) 
	: m_from(_other.m_from), m_to(_other.m_to), m_ignoreInHistory(_other.m_ignoreInHistory) {}

rbeCore::cfgLineConnection::~cfgLineConnection() {}

rbeCore::cfgLineConnection& rbeCore::cfgLineConnection::operator = (const cfgLineConnection& _other) {
	m_from = _other.m_from;
	m_to = _other.m_to;
	m_ignoreInHistory = _other.m_ignoreInHistory;
	return *this;
}

void rbeCore::cfgLineConnection::addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object) const {
	if (!_object.IsObject()) { rbeAssert(0, "The provided json value is not an object @cfgStep"); return; }

	_object.AddMember(RBE_JSON_CONNECTION_Type, RBE_JSON_VALUE_ConnectionType_Line, _doc.GetAllocator());
	_object.AddMember(RBE_JSON_CONNECTION_IgnoreInHistory, m_ignoreInHistory, _doc.GetAllocator());
	_object.AddMember(RBE_JSON_CONNECTION_LINE_From, rapidjson::Value(m_from.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
	_object.AddMember(RBE_JSON_CONNECTION_LINE_To, rapidjson::Value(m_to.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
}

// ##################################################################################################################################################

// ##################################################################################################################################################

// ##################################################################################################################################################

rbeCore::cfgHistoryConnection::cfgHistoryConnection(int _stepId, bool _ignoreInHistory) : m_stepId(_stepId), m_ignoreInHistory(_ignoreInHistory) {}

rbeCore::cfgHistoryConnection::cfgHistoryConnection(const cfgHistoryConnection& _other) 
	: m_stepId(_other.m_stepId), m_ignoreInHistory(_other.m_ignoreInHistory) {}

rbeCore::cfgHistoryConnection::~cfgHistoryConnection() {

}

rbeCore::cfgHistoryConnection& rbeCore::cfgHistoryConnection::operator = (const cfgHistoryConnection& _other) {
	m_stepId = _other.m_stepId;
	m_ignoreInHistory = _other.m_ignoreInHistory;
	return *this;
}

void rbeCore::cfgHistoryConnection::addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object) const {
	if (!_object.IsObject()) { rbeAssert(0, "The provided json value is not an object @cfgStep"); return; }

	_object.AddMember(RBE_JSON_CONNECTION_Type, RBE_JSON_VALUE_ConnectionType_History, _doc.GetAllocator());
	_object.AddMember(RBE_JSON_CONNECTION_IgnoreInHistory, m_ignoreInHistory, _doc.GetAllocator());
	_object.AddMember(RBE_JSON_STEP_Step, m_stepId, _doc.GetAllocator());
}

// ##################################################################################################################################################

// ##################################################################################################################################################

// ##################################################################################################################################################

rbeCore::cfgCircleConnection::cfgCircleConnection(int _centerPointId, const std::string& _radius, CircleOrientation _circleOrientation, bool _ignoreInHistory)
	: m_radius(_radius), m_orientation(_circleOrientation), m_ignoreInHistory(_ignoreInHistory) 
{
	m_centerPoint = "$" + std::to_string(_centerPointId);
}

rbeCore::cfgCircleConnection::cfgCircleConnection(DefaultPoint _centerPoint, const std::string& _radius, CircleOrientation _circleOrientation, bool _ignoreInHistory)
	: m_radius(_radius), m_orientation(_circleOrientation), m_ignoreInHistory(_ignoreInHistory)
{
	switch (_centerPoint)
	{
	case rbeCore::ORIGIN: m_centerPoint = "Origin"; break;
	case rbeCore::CURRENT: m_centerPoint = "Current"; break;
	default: rbeAssert(0, "Invalid point type"); break;
	}
}

rbeCore::cfgCircleConnection::cfgCircleConnection(const cfgCircleConnection& _other)
	: m_centerPoint(_other.m_centerPoint), m_radius(_other.m_radius), m_orientation(_other.m_orientation), m_ignoreInHistory(_other.m_ignoreInHistory) {}

rbeCore::cfgCircleConnection::~cfgCircleConnection() {

}

rbeCore::cfgCircleConnection& rbeCore::cfgCircleConnection::operator = (const cfgCircleConnection& _other) {
	m_centerPoint = _other.m_centerPoint;
	m_radius = _other.m_radius;
	m_orientation = _other.m_orientation;
	m_ignoreInHistory = _other.m_ignoreInHistory;
	return *this;
}

void rbeCore::cfgCircleConnection::addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object) const {
	if (!_object.IsObject()) { rbeAssert(0, "The provided json value is not an object @cfgStep"); return; }

	_object.AddMember(RBE_JSON_CONNECTION_Type, RBE_JSON_VALUE_ConnectionType_Circle, _doc.GetAllocator());
	_object.AddMember(RBE_JSON_CONNECTION_IgnoreInHistory, m_ignoreInHistory, _doc.GetAllocator());
	_object.AddMember(RBE_JSON_CONNECTION_CIRCLE_Midpoint, rapidjson::Value(m_centerPoint.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
	_object.AddMember(RBE_JSON_CONNECTION_CIRCLE_Radius, rapidjson::Value(m_radius.c_str(), _doc.GetAllocator()), _doc.GetAllocator());
	
	switch (m_orientation)
	{
	case rbeCore::coUV: _object.AddMember(RBE_JSON_CONNECTION_CIRCLE_Orientation, RBE_JSON_VALUE_CircleOrientation_UV, _doc.GetAllocator());	break;
	case rbeCore::coUW: _object.AddMember(RBE_JSON_CONNECTION_CIRCLE_Orientation, RBE_JSON_VALUE_CircleOrientation_UW, _doc.GetAllocator());	break;
	case rbeCore::coVW: _object.AddMember(RBE_JSON_CONNECTION_CIRCLE_Orientation, RBE_JSON_VALUE_CircleOrientation_VW, _doc.GetAllocator());	break;
	default: rbeAssert(0, "Invalid orientation"); break;
	}
}
