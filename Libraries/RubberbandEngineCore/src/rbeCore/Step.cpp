// @otlicense

/*
 *	File:		Step.h
 *	Package:	rbeCore
 *
 *  Created on: September 07, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// RBE header
#include <rbeCore/Step.h>
#include <rbeCore/Point.h>
#include <rbeCore/RubberbandEngine.h>
#include <rbeCore/jsonMember.h>
#include <rbeCore/AbstractConnection.h>
#include <rbeCore/LineConnection.h>
#include <rbeCore/HistoryConnection.h>
#include <rbeCore/CircleConnection.h>
#include <rbeCore/Limit.h>
#include <rbeCore/rbeAssert.h>

#include <rbeCalc/AbstractCalculationItem.h>
#include <rbeCalc/ParserAPI.h>
#include <rbeCalc/VariableValue.h>

// rapidjson header
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

// C++ header
#include <map>
#include <vector>
#include <list>

using namespace rbeCore;

struct Step::sData {
	std::map<int, Point *>				points;
	std::vector<AbstractConnection *>	connections;
	std::list<Limit *>					limits;
};

bool isJsonDocumentValid(rapidjson::Document& _doc) {
	// Check member
	if (!_doc.IsObject()) {
		rbeAssert(0, "Invalid JSON Format: Document is not an object @Step"); return false;
	}
	if (!_doc.HasMember(RBE_JSON_STEP_Step)) {
		rbeAssert(0, "Invalid JSON Format: Member \"" RBE_JSON_STEP_Step "\" is missing @Step"); return false;
	}
	if (!_doc.HasMember(RBE_JSON_STEP_MayEndWithout)) {
		rbeAssert(0, "Invalid JSON Format: Member \"" RBE_JSON_STEP_MayEndWithout "\" is missing @Step"); return false;
	}
	if (!_doc.HasMember(RBE_JSON_STEP_Projection)) {
		rbeAssert(0, "Invalid JSON Format: Member \"" RBE_JSON_STEP_Projection "\" is missing @Step"); return false;
	}
	if (!_doc.HasMember(RBE_JSON_STEP_Limits)) {
		rbeAssert(0, "Invalid JSON Format: Member \"" RBE_JSON_STEP_Limits "\" is missing @Step"); return false;
	}
	if (!_doc.HasMember(RBE_JSON_STEP_Points)) {
		rbeAssert(0, "Invalid JSON Format: Member \"" RBE_JSON_STEP_Points "\" is missing @Step"); return false;
	}
	if (!_doc.HasMember(RBE_JSON_STEP_Connections)) {
		rbeAssert(0, "Invalid JSON Format: Member \"" RBE_JSON_STEP_Connections "\" is missing @Step"); return false;
	}

	// Check member types
	if (!_doc[RBE_JSON_STEP_Step].IsInt()) {
		rbeAssert(0, "Invalid JSON Format: Member \"" RBE_JSON_STEP_Step "\" is not a integer @Step"); return false;
	}
	if (!_doc[RBE_JSON_STEP_MayEndWithout].IsBool()) {
		rbeAssert(0, "Invalid JSON Format: Member \"" RBE_JSON_STEP_MayEndWithout "\" is not a boolean @Step"); return false;
	}
	if (!_doc[RBE_JSON_STEP_Projection].IsString()) {
		rbeAssert(0, "Invalid JSON Format: Member \"" RBE_JSON_STEP_Projection "\" is not a string @Step"); return false;
	}
	if (!_doc[RBE_JSON_STEP_Limits].IsArray()) {
		rbeAssert(0, "Invalid JSON Format: Member \"" RBE_JSON_STEP_Limits "\" is not a array @Step"); return false;
	}
	if (!_doc[RBE_JSON_STEP_Points].IsArray()) {
		rbeAssert(0, "Invalid JSON Format: Member \"" RBE_JSON_STEP_Points "\" is not a array @Step"); return false;
	}
	if (!_doc[RBE_JSON_STEP_Connections].IsArray()) {
		rbeAssert(0, "Invalid JSON Format: Member \"" RBE_JSON_STEP_Connections "\" is not a array @Step"); return false;
	}

	return true;
}

Step::Step()
	: m_pointsOwned(true), m_id(0), m_projection(pUV), m_mayEndWithout(true), m_lastCurrent(nullptr)
{
	m_data = new sData;
	m_lastCurrentU = new rbeCalc::VariableValue(0.f);
	m_lastCurrentV = new rbeCalc::VariableValue(0.f);
	m_lastCurrentW = new rbeCalc::VariableValue(0.f);
	m_lastCurrent = new Point(m_lastCurrentU, m_lastCurrentV, m_lastCurrentW);
}

Step::~Step() {
	if (m_pointsOwned) {
		for (auto p : m_data->points) {
			delete p.second;
		}
	}
	for (auto c : m_data->connections) {
		delete c;
	}
	for (auto l : m_data->limits) {
		delete l;
	}
	delete m_data;
	delete m_lastCurrent;
}

// ###################################################################################

// Setter

void Step::addPoint(Point * _p) {
	if (_p == nullptr) {
		rbeAssert(0, "nullptr provided as point @Step");
		return;
	}

	if (m_data->points.find(_p->id()) != m_data->points.end()) {
		rbeAssert(0, "Point with the given id already exists @Step");
		return;
	}

	m_data->points.insert_or_assign(_p->id(), _p);
}

void Step::addLimit(Limit * _limit) {
	if (_limit == nullptr) {
		rbeAssert(0, "nullptr provided as point @Step");
		return;
	}
	m_data->limits.push_back(_limit);
}

void Step::setupFromJson(RubberbandEngine * _engine, const std::string& _json) {
	if (_json.empty()) {
		rbeAssert(0, "Empty JSON string provided @Step");
		return;
	}

	rapidjson::Document doc;
	doc.Parse(_json.c_str());

	if (!isJsonDocumentValid(doc)) { return; }

	// Data
	m_id = doc[RBE_JSON_STEP_Step].GetInt();
	m_mayEndWithout = doc[RBE_JSON_STEP_MayEndWithout].GetBool();
	
	// Projection
	std::string projection = doc[RBE_JSON_STEP_Projection].GetString();
	if (projection == RBE_JSON_VALUE_ProjectionUV) {
		m_projection = pUV;
	}
	else if (projection == RBE_JSON_VALUE_ProjectionW) {
		m_projection = pW;
	}
	else {
		rbeAssert(0, "Invalid JSON Format: Invalid projection specified @Step");
		return;
	}

	// Limits
	auto limitData = doc[RBE_JSON_STEP_Limits].GetArray();
	for (rapidjson::SizeType i{ 0 }; i < limitData.Size(); i++) {
		if (!limitData[i].IsObject()) {
			rbeAssert(0, "Invalid JSON Format: Limit entry is not an object");
			return;
		}
		auto l = limitData[i].GetObject();

		// Check member
		if (!l.HasMember(RBE_JSON_LIMIT_Axis)) {
			rbeAssert(0, "Invalid JSON Format: Limit entry member \"" RBE_JSON_LIMIT_Axis "\" is missing @Step");
			return;
		}
		if (!l.HasMember(RBE_JSON_LIMIT_Value)) {
			rbeAssert(0, "Invalid JSON Format: Limit entry member \"" RBE_JSON_LIMIT_Value "\" is missing @Step");
			return;
		}

		// Check datatypes
		if (!l[RBE_JSON_LIMIT_Axis].IsString()) {
			rbeAssert(0, "Invalid JSON Format: Limit entry member \"" RBE_JSON_LIMIT_Axis "\" is not a string @Step");
			return;
		}
		if (!l[RBE_JSON_LIMIT_Value].IsString()) {
			rbeAssert(0, "Invalid JSON Format: Limit entry member \"" RBE_JSON_LIMIT_Value "\" is not a string @Step");
			return;
		}

		std::string axis = l[RBE_JSON_LIMIT_Axis].GetString();
		AxisLimit actualAxis;
		if (axis == RBE_JSON_VALUE_AxisLimit_Umax) { actualAxis = Umax; }
		else if (axis == RBE_JSON_VALUE_AxisLimit_Umin) { actualAxis = Umin; }
		else if (axis == RBE_JSON_VALUE_AxisLimit_Vmax) { actualAxis = Vmax; }
		else if (axis == RBE_JSON_VALUE_AxisLimit_Vmin) { actualAxis = Vmin; }
		else if (axis == RBE_JSON_VALUE_AxisLimit_Wmax) { actualAxis = Wmax; }
		else if (axis == RBE_JSON_VALUE_AxisLimit_Wmin) { actualAxis = Wmin; }
		else { rbeAssert(0, "Invalid JSON Format: Limit entry member \"" RBE_JSON_LIMIT_Axis "\" unknown value @Step"); return; }
		
		std::string valueFormula = l[RBE_JSON_LIMIT_Value].GetString();
		rbeCalc::AbstractCalculationItem  * actualValue = rbeCalc::ParserAPI::parseFormula(_engine, this, valueFormula);
		if (actualValue) {
			Limit * newLimit = new Limit(actualAxis, actualValue);
			m_data->limits.push_back(newLimit);
		}
	}

	// Points
	auto pointData = doc[RBE_JSON_STEP_Points].GetArray();
	for (rapidjson::SizeType i{ 0 }; i < pointData.Size(); i++) {
		if (!pointData[i].IsObject()) {
			rbeAssert(0, "Invalid JSON Format: Point is not an object @Step");
			return;
		}
		auto pt = pointData[i].GetObject();

		// Check member
		if (!pt.HasMember(RBE_JSON_Point_ID)) {
			rbeAssert(0, "Invalid JSON Format: Point entry member \"" RBE_JSON_Point_ID "\" is missing @Step");
			return;
		}
		if (!pt.HasMember(RBE_JSON_Point_U)) {
			rbeAssert(0, "Invalid JSON Format: Point entry member \"" RBE_JSON_Point_U "\" is missing @Step");
			return;
		}
		if (!pt.HasMember(RBE_JSON_Point_V)) {
			rbeAssert(0, "Invalid JSON Format: Point entry member \"" RBE_JSON_Point_V "\" is missing @Step");
			return;
		}
		if (!pt.HasMember(RBE_JSON_Point_W)) {
			rbeAssert(0, "Invalid JSON Format: Point entry member \"" RBE_JSON_Point_W "\" is missing @Step");
			return;
		}

		// Check type
		if (!pt[RBE_JSON_Point_ID].IsInt()) {
			rbeAssert(0, "Invalid JSON Format: Point entry member \"" RBE_JSON_Point_ID "\" is not a integer @Step");
			return;
		}
		if (!pt[RBE_JSON_Point_U].IsString()) {
			rbeAssert(0, "Invalid JSON Format: Point entry member \"" RBE_JSON_Point_U "\" is not a string @Step");
			return;
		}
		if (!pt[RBE_JSON_Point_V].IsString()) {
			rbeAssert(0, "Invalid JSON Format: Point entry member \"" RBE_JSON_Point_V "\" is not a string @Step");
			return;
		}
		if (!pt[RBE_JSON_Point_W].IsString()) {
			rbeAssert(0, "Invalid JSON Format: Point entry member \"" RBE_JSON_Point_W "\" is not a string @Step");
			return;
		}

		int pId = pt[RBE_JSON_Point_ID].GetInt();
		for (auto mPt : m_data->points) {
			if (mPt.first == pId) {
				rbeAssert(0, "Data error: Point with the current ID already exists in the step @Step");
				return;
			}
			if (_engine->hasPoint(pId)) {
				rbeAssert(0, "Data error: Point with the current ID already exists in the engine @Step");
				return;
			}
		}
		
		std::string formulaU = pt[RBE_JSON_Point_U].GetString();
		std::string formulaV = pt[RBE_JSON_Point_V].GetString();
		std::string formulaW = pt[RBE_JSON_Point_W].GetString();

		rbeCalc::AbstractCalculationItem * itmU = rbeCalc::ParserAPI::parseFormula(_engine, this, formulaU);
		rbeCalc::AbstractCalculationItem * itmV = rbeCalc::ParserAPI::parseFormula(_engine, this, formulaV);
		rbeCalc::AbstractCalculationItem * itmW = rbeCalc::ParserAPI::parseFormula(_engine, this, formulaW);

		Point * newPoint = new Point(itmU, itmV, itmW);
		newPoint->setId(pId);
		m_data->points.insert_or_assign(newPoint->id(), newPoint);
	}

	// Connections
	auto connectionData = doc[RBE_JSON_STEP_Connections].GetArray();
	for (rapidjson::SizeType i{ 0 }; i < connectionData.Size(); i++) {
		if (!connectionData[i].IsObject()) {
			rbeAssert(0, "Invalid JSON Format: Connection is not an object @Step"); return;
		}
		auto connection = connectionData[i].GetObject();

		// Member
		if (!connection.HasMember(RBE_JSON_CONNECTION_Type)) {
			rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_CONNECTION_Type "\" is missing @Step"); return;
		}
		if (!connection[RBE_JSON_CONNECTION_Type].IsString()) {
			rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_CONNECTION_Type "\" is not a string @Step"); return;
		}

		
		bool ignoreConnectionInHistory = false;

		if (connection.HasMember(RBE_JSON_CONNECTION_IgnoreInHistory)) {
			if (!connection[RBE_JSON_CONNECTION_IgnoreInHistory].IsBool()) {
				rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_CONNECTION_IgnoreInHistory "\" is not a boolean @Step"); return;
			}
			ignoreConnectionInHistory = connection[RBE_JSON_CONNECTION_IgnoreInHistory].GetBool();
		}

		// Data
		std::string connectionType = connection[RBE_JSON_CONNECTION_Type].GetString();
		if (connectionType == RBE_JSON_VALUE_ConnectionType_Line) {
			
			// Member
			if (!connection.HasMember(RBE_JSON_CONNECTION_LINE_From)) {
				rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_CONNECTION_LINE_From "\" is missing @Step"); return;
			}
			if (!connection.HasMember(RBE_JSON_CONNECTION_LINE_To)) {
				rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_CONNECTION_LINE_To "\" is missing @Step"); return;
			}
			if (!connection[RBE_JSON_CONNECTION_LINE_From].IsString()) {
				rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_CONNECTION_LINE_From "\" is not a string @Step"); return;
			}
			if (!connection[RBE_JSON_CONNECTION_LINE_To].IsString()) {
				rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_CONNECTION_LINE_To "\" is not a string @Step"); return;
			}
			std::string stringFrom = connection[RBE_JSON_CONNECTION_LINE_From].GetString();
			std::string stringTo = connection[RBE_JSON_CONNECTION_LINE_To].GetString();

			eAxisDistance distanceType;
			AbstractPoint * pFrom = rbeCalc::ParserAPI::parsePoint(_engine, this, stringFrom, distanceType);
			if (distanceType != dAll) {
				rbeAssert(0, "Data error: For a line connection the axis specifier must not be provided for member \"" RBE_JSON_CONNECTION_LINE_From "\" @Step"); return;
			}

			AbstractPoint * pTo = rbeCalc::ParserAPI::parsePoint(_engine, this, stringTo, distanceType);
			if (distanceType != dAll) {
				rbeAssert(0, "Data error: For a line connection the axis specifier must not be provided for member \"" RBE_JSON_CONNECTION_LINE_To "\" @Step"); return;
			}

			// Data
			LineConnection * newConnection = new LineConnection;
			newConnection->setFrom(pFrom);
			newConnection->setTo(pTo);
			newConnection->setIgnoreInHistory(ignoreConnectionInHistory);
			m_data->connections.push_back(newConnection);

		}
		else if (connectionType == RBE_JSON_VALUE_ConnectionType_Circle) {
			// Member
			if (!connection.HasMember(RBE_JSON_CONNECTION_CIRCLE_Midpoint)) {
				rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_CONNECTION_CIRCLE_Midpoint "\" is missing @Step"); return;
			}
			if (!connection.HasMember(RBE_JSON_CONNECTION_CIRCLE_Orientation)) {
				rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_CONNECTION_CIRCLE_Orientation "\" is missing @Step"); return;
			}
			if (!connection.HasMember(RBE_JSON_CONNECTION_CIRCLE_Radius)) {
				rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_CONNECTION_CIRCLE_Radius "\" is missing @Step"); return;
			}
			if (!connection[RBE_JSON_CONNECTION_CIRCLE_Midpoint].IsString()) {
				rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_CONNECTION_CIRCLE_Midpoint "\" is not a string @Step"); return;
			}
			if (!connection[RBE_JSON_CONNECTION_CIRCLE_Orientation].IsString()) {
				rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_CONNECTION_CIRCLE_Orientation "\" is not a string @Step"); return;
			}
			if (!connection[RBE_JSON_CONNECTION_CIRCLE_Radius].IsString()) {
				rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_CONNECTION_CIRCLE_Radius "\" is not a string @Step"); return;
			}

			// Data
			CircleConnection * newConnection = new CircleConnection;
			newConnection->setIgnoreInHistory(ignoreConnectionInHistory);
			
			std::string mid = connection[RBE_JSON_CONNECTION_CIRCLE_Midpoint].GetString();
			std::string orient = connection[RBE_JSON_CONNECTION_CIRCLE_Orientation].GetString();
			std::string radius = connection[RBE_JSON_CONNECTION_CIRCLE_Radius].GetString();

			if (orient == RBE_JSON_VALUE_CircleOrientation_UV) {
				newConnection->setOrientation(coUV);
			} else if (orient == RBE_JSON_VALUE_CircleOrientation_UW) {
				newConnection->setOrientation(coUW);
			} else if (orient == RBE_JSON_VALUE_CircleOrientation_VW) {
				newConnection->setOrientation(coVW);
			}
			else {
				delete newConnection;
				rbeAssert(0, "Data error: Invalid circle orientation provided @Step");
				return;
			}

			eAxisDistance midOrient;
			newConnection->setCenterpoint(rbeCalc::ParserAPI::parsePoint(_engine, this, mid, midOrient));
			if (midOrient != dAll) {
				delete newConnection;
				rbeAssert(0, "Data error: Centerpoint must not contain axis specifier @Step");
				return;
			}
			newConnection->setRadius(rbeCalc::ParserAPI::parseFormula(_engine, this, radius));

			m_data->connections.push_back(newConnection);
		}
		else if (connectionType == RBE_JSON_VALUE_ConnectionType_History) {
			// Member
			if (!connection.HasMember(RBE_JSON_STEP_Step)) {
				rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_STEP_Step "\" is missing @Step"); return;
			}
			if (!connection[RBE_JSON_STEP_Step].IsInt()) {
				rbeAssert(0, "Invalid JSON Format: Connection member \"" RBE_JSON_STEP_Step "\" is not a integer @Step"); return;
			}
			
			int sID = connection[RBE_JSON_STEP_Step].GetInt();
			if (!_engine->hasStep(sID)) {
				rbeAssert(0, "Data error: Step with provided ID was not found in the engine @Step"); return;
			}
			Step * step = _engine->step(sID);
			if (step) {
				HistoryConnection * newConnection = new HistoryConnection(_engine->step(sID));
				newConnection->setIgnoreInHistory(ignoreConnectionInHistory);
				m_data->connections.push_back(newConnection);
			}
		}
		else {
			rbeAssert(0, "Data error: Connection member \"" RBE_JSON_CONNECTION_Type "\" has a invalid value @Step"); return;
		}
	}
}

void Step::givePointOwnershipToEngine(RubberbandEngine * _engine) {
	if (!m_pointsOwned) {
		rbeAssert(0, "Point ownership already moved to engine @Step");
		return;
	}

	for (auto p : m_data->points) {
		rbeCalc::VariableValue * u = new rbeCalc::VariableValue(p.second->u());
		rbeCalc::VariableValue * v = new rbeCalc::VariableValue(p.second->v());
		rbeCalc::VariableValue * w = new rbeCalc::VariableValue(p.second->w());
		p.second->replaceU(u);
		p.second->replaceV(v);
		p.second->replaceW(w);
		_engine->addPoint(p.second);
	}
	m_pointsOwned = false;
}

// ###################################################################################

// Getter

Point * Step::point(int _id) {
	auto p = m_data->points.find(_id);
	if (p == m_data->points.end()) {
		rbeAssert(0, "Point with given ID not found @Step");
		return nullptr;
	}
	return p->second;
}

bool Step::hasPoint(int _id) {
	auto p = m_data->points.find(_id);
	return p != m_data->points.end();
}

std::string Step::debugInformation(const std::string& _prefix) {
	std::string ret{"{\n"};
	ret.append(_prefix).append("\t\"" RBE_JSON_STEP_Step "\": ");
	ret.append(std::to_string(m_id)).append(",\n").append(_prefix).append("\t\"" RBE_JSON_STEP_MayEndWithout "\": ");
	if (m_mayEndWithout) {
		ret.append("true,\n");
	}
	else {
		ret.append("false,\n");
	}
	ret.append(_prefix).append("\t\"" RBE_JSON_STEP_Projection "\": \"");
	switch (m_projection)
	{
	case rbeCore::Step::pUV: ret.append(RBE_JSON_VALUE_ProjectionUV); break;
	case rbeCore::Step::pW: ret.append(RBE_JSON_VALUE_ProjectionW); break;
	default:
		rbeAssert(0, "Data error: Unknown Project set @Step"); return "";
	}
	ret.append("\",\n").append(_prefix).append("\t\"" RBE_JSON_STEP_Points "\": [");
	bool first{ true };
	for (auto p : m_data->points) {
		if (first) {
			first = false;
			ret.append("\n").append(_prefix).append("\t\t");
			ret.append(p.second->debugInformation(_prefix + "\t\t"));
		}
		else {
			ret.append(",\n").append(_prefix).append("\t\t");
			ret.append(p.second->debugInformation(_prefix + "\t\t"));
		}
	}
	ret.append("\n").append(_prefix).append("\t],\n");

	ret.append(_prefix).append("\t\"" RBE_JSON_STEP_Connections "\": [");
	first = true;
	for (auto c : m_data->connections) {
		if (first) {
			first = false;
			ret.append("\n").append(_prefix).append("\t\t");
			ret.append(c->debugInformation(_prefix + "\t\t"));
		}
		else {
			ret.append(",\n").append(_prefix).append("\t\t");
			ret.append(c->debugInformation(_prefix + "\t\t"));
		}
	}
	ret.append("\n").append(_prefix).append("\t]\n").append(_prefix).append("}");

	return ret;
}

void Step::addConnectionsToJsonArray(RubberbandEngine * _engine, std::stringstream& _array, bool& _first, bool _isHistory) {
	for (auto c : m_data->connections) {
		if (!_isHistory || !c->ignoreInHistory()) {
			c->addToJsonArray(_engine, _array, _first);
		}
	}
}

void Step::addConnectionsToList(std::list<AbstractConnection *>& _list, bool _isHistory) {
	for (auto c : m_data->connections) {
		if (!_isHistory || !c->ignoreInHistory()) {
			if (c->type() == AbstractConnection::ctHistory) {
				HistoryConnection * actualConnection = dynamic_cast<HistoryConnection *>(c);
				if (actualConnection) {
					Step * s = actualConnection->reference();
					if (s) {
						s->addConnectionsToList(_list, true);
					}
					else {
						rbeAssert(0, "Data error: Step, referenced by connection, is NULL @Step");
					}
				}
				else {
					rbeAssert(0, "Fatal: Cast failed @Step");
				}
			}
			else {

				_list.push_back(c);
			}
		}
	}
}

void Step::adjustCoordinateToLimits(coordinate_t & _u, coordinate_t & _v, coordinate_t & _w) {
	for (auto l : m_data->limits) {
		switch (l->axis())
		{
		case Umin: if (_u < l->value()) { _u = l->value(); } break;
		case Umax: if (_u > l->value()) { _u = l->value(); } break;
		case Vmin: if (_v < l->value()) { _v = l->value(); } break;
		case Vmax: if (_v > l->value()) { _v = l->value(); } break;
		case Wmin: if (_w < l->value()) { _w = l->value(); } break;
		case Wmax: if (_w > l->value()) { _w = l->value(); } break;
		default: rbeAssert(0, "Unknown limit @Step");
		}
	}
}

void Step::setLastPosition(coordinate_t _u, coordinate_t _v, coordinate_t _w) {
	m_lastCurrentU->setValue(_u);
	m_lastCurrentV->setValue(_v);
	m_lastCurrentW->setValue(_w);
}

AbstractPoint * Step::lastPosition(void) { return m_lastCurrent; }
