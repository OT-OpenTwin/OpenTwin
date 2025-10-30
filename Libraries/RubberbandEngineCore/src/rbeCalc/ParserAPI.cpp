// @otlicense
// File: ParserAPI.cpp
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
#include <rbeCalc/ParserAPI.h>
#include <rbeCalc/OperatorAdd.h>
#include <rbeCalc/OperatorDivide.h>
#include <rbeCalc/OperatorMultiply.h>
#include <rbeCalc/OperatorSubtract.h>
#include <rbeCalc/PointDistanceOperator.h>
#include <rbeCalc/PointReference.h>
#include <rbeCalc/VariableValue.h>

#include <rbeCore/Point.h>
#include <rbeCore/Step.h>
#include <rbeCore/NumericPoint.h>
#include <rbeCore/RubberbandEngine.h>
#include <rbeCore/rbeAssert.h>

// C++ header
#include <cctype>
#include <algorithm>

using namespace rbeCalc;

PointDistanceOperator * parseDistanceOperator(rbeCore::RubberbandEngine * _engine, rbeCore::Step * _step, const std::string& _lhv, const std::string& _rhv) {
	rbeCore::eAxisDistance dL, dR;
	rbeCore::AbstractPoint * pL = ParserAPI::parsePoint(_engine, _step, _lhv, dL);
	rbeCore::AbstractPoint * pR = ParserAPI::parsePoint(_engine, _step, _rhv, dR);

	if (dL == dR) {
		return new PointDistanceOperator(pL, pR, dL, true);
	}
	else if (dL == rbeCore::dAll) {
		return new PointDistanceOperator(pL, pR, dR, true);
	}
	else if (dR == rbeCore::dAll) {
		return new PointDistanceOperator(pL, pR, dL, true);
	}
	else if (dL == rbeCore::dU) {
		if (dR == rbeCore::dV) {
			return new PointDistanceOperator(pL, pR, rbeCore::dUV, false);
		} else if (dR == rbeCore::dW) {
			return new PointDistanceOperator(pL, pR, rbeCore::dUW, false);
		}
		else {
			rbeAssert(0, "Invalid distance value @ParserAPI");
			return nullptr;
		}
	}
	else if (dL == rbeCore::dV) {
		if (dR == rbeCore::dU) {
			return new PointDistanceOperator(pL, pR, rbeCore::dVU, false);
		}
		else if (dR == rbeCore::dW) {
			return new PointDistanceOperator(pL, pR, rbeCore::dVW, false);
		}
		else {
			rbeAssert(0, "Invalid distance value @ParserAPI");
			return nullptr;
		}
	}
	else if (dL == rbeCore::dW) {
		if (dR == rbeCore::dV) {
			return new PointDistanceOperator(pL, pR, rbeCore::dWV, false);
		}
		else if (dR == rbeCore::dU) {
			return new PointDistanceOperator(pL, pR, rbeCore::dWU, false);
		}
		else {
			rbeAssert(0, "Invalid distance value @ParserAPI");
			return nullptr;
		}
	}
	else {
		rbeAssert(0, "Invalid distance value @ParserAPI");
		return nullptr;
	}
}

AbstractCalculationItem * ParserAPI::parseFormula(rbeCore::RubberbandEngine * _engine, rbeCore::Step * _step, const std::string& _string) {
	if (_string.empty()) { return new VariableValue(0); }

	size_t idx = _string.rfind('+');
	if (idx != std::string::npos) {
		return new OperatorAdd(
			parseFormula(_engine, _step, _string.substr(0, idx)),
			parseFormula(_engine, _step, _string.substr(idx + 1))
		);
	}
	idx = _string.rfind('-');
	if (idx != std::string::npos) {
		return new OperatorSubtract(
			parseFormula(_engine, _step, _string.substr(0, idx)),
			parseFormula(_engine, _step, _string.substr(idx + 1))
		);
	}
	idx = _string.rfind('/');
	if (idx != std::string::npos) {
		return new OperatorDivide(
			parseFormula(_engine, _step, _string.substr(0, idx)),
			parseFormula(_engine, _step, _string.substr(idx + 1))
		);
	}
	idx = _string.rfind('*');
	if (idx != std::string::npos) {
		return new OperatorMultiply(
			parseFormula(_engine, _step, _string.substr(0, idx)),
			parseFormula(_engine, _step, _string.substr(idx + 1))
		);
	}
	idx = _string.rfind('>');
	if (idx != std::string::npos) {
		return parseDistanceOperator(_engine, _step, _string.substr(0, idx), _string.substr(idx + 1));
	}

	rbeCore::eAxisDistance ax;
	rbeCore::AbstractPoint * pt = parsePoint(_engine, _step, _string, ax);

	if (pt) {
		switch (ax)
		{
		case rbeCore::dU: return new PointReference(pt, rbeCore::U);
		case rbeCore::dV: return new PointReference(pt, rbeCore::V);
		case rbeCore::dW: return new PointReference(pt, rbeCore::W);
		default:
			rbeAssert(0, "Invalid point axis provided");
			return nullptr;
		}
	}
	else {
		bool fail;
		coordinate_t v = rbeCore::toNumber<coordinate_t>(_string, fail);
		if (fail) {
			rbeAssert(0, "Syntax error while parsing function");
			return nullptr;
		}
		return new VariableValue(v);
	}
}

rbeCore::AbstractPoint * ParserAPI::parsePoint(rbeCore::RubberbandEngine * _engine, rbeCore::Step * _step, const std::string& _str, rbeCore::eAxisDistance& _distance) {
	if (_str.empty()) {
		rbeAssert(0, "Provided point is empty @ParserAPI");
		return nullptr;
	}
	std::string str = _str;
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {return std::tolower(c); });

	if (str == "current") {
		_distance = rbeCore::dAll;
		return _engine->current();
	}
	else if (str == "current.u") {
		_distance = rbeCore::dU;
		return _engine->current();
	}
	else if (str == "current.v") {
		_distance = rbeCore::dV;
		return _engine->current();
	}
	else if (str == "current.w") {
		_distance = rbeCore::dW;
		return _engine->current();
	}
	else if (str == "origin") {
		_distance = rbeCore::dAll;
		return _engine->origin();
	}
	else if (str == "origin.u") {
		_distance = rbeCore::dU;
		return _engine->origin();
	}
	else if (str == "origin.v") {
		_distance = rbeCore::dV;
		return _engine->origin();
	}
	else if (str == "origin.w") {
		_distance = rbeCore::dW;
		return _engine->origin();
	}
	else if (str[0] == '$') {
		std::string pt;
		std::string ax;
		pt = str.substr(1);
		size_t idx = pt.rfind('.');
		if (idx != std::string::npos) {
			ax = pt.substr(idx + 1);
			pt = pt.substr(0, idx);
		}
		if (ax == "u") {
			_distance = rbeCore::dU;
		}
		else if (ax == "v") {
			_distance = rbeCore::dV;
		}
		else if (ax == "w") {
			_distance = rbeCore::dW;
		}
		else if (ax.empty()) {
			_distance = rbeCore::dAll;
		}
		else {
			rbeAssert(0, "Invalid projection syntax @ParserAPI");
		}

		bool fail;
		int pId = rbeCore::toNumber<int>(pt, fail);
		if (fail) {
			rbeAssert(0, "Invalid number format @ParserAPI");
			return nullptr;
		}

		if (_engine->hasPoint(pId)) {
			return _engine->point(pId);
		}
		else if (_step) {
			if (_step->hasPoint(pId)) {
				return _step->point(pId);
			}
		}

		rbeAssert(0, "Data error: Proivded point reference could not be resolved @ParserAPI");
		return nullptr;
	}
	else {
		return nullptr;
	}
}
