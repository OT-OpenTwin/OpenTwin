// @otlicense

#pragma once

// RBE header
#include <rbeCore/dataTypes.h>

// C++ header
#include <string>

namespace rbeCore {
	class RubberbandEngine;
	class AbstractPoint;
	class Step;
}

namespace rbeCalc {

	class AbstractCalculationItem;

	namespace ParserAPI {

		RBE_API_EXPORT rbeCalc::AbstractCalculationItem * parseFormula(rbeCore::RubberbandEngine * _engine, rbeCore::Step * _step, const std::string& _string);
	
		RBE_API_EXPORT rbeCore::AbstractPoint * parsePoint(rbeCore::RubberbandEngine * _engine, rbeCore::Step * _step, const std::string& _str, rbeCore::eAxisDistance& _distance);

	}
}